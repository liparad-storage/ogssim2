/*
 * Copyright UVSQ - CEA/DAM/DIF (2018)
 * Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
 *                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "templateparser.h"

#include <iostream>

#include <QFile>

using namespace std;

inline string
_getAttributeValue (
    QDomNode				node,
    const string			name) {

    if (! node.hasAttributes () ) return "";

    QDomNamedNodeMap attrL = node.attributes ();
    for (auto i = 0; i < attrL.length (); ++i) {
        if (! name.compare (attrL.item (i) .nodeName () .toStdString () ) )
            return attrL.item (i) .nodeValue () .toStdString ();
    }

    return "";
}

inline void
_extractSuggestedValues (
    string					values,
    shared_ptr <TemplateElement>	ptr) {
    while (values.find (';') != string::npos) {
        ptr->_values.insert (QString::fromStdString (values.substr (0, values.find_first_of (';') ) ) );
        values = values.substr (values.find_first_of (';') + 1);
    }
    ptr->_values.insert (QString::fromStdString (values) );
}

shared_ptr <TemplateElement> _getTemplate (
    shared_ptr <Template>   temp,
    QDomElement             node) {
    shared_ptr <TemplateElement> root {nullptr};
    shared_ptr <TemplateElement> ptr {nullptr};
    string					tmp;

    if (node.isNull () ) return nullptr;

    tmp = _getAttributeValue (node, "field");

    auto findRes = find_if (EltFieldNameMap.begin (), EltFieldNameMap.end (),
        [&] (const pair <OGSG_EltField, string> & elt)
        { return ! elt.second.compare (tmp); } );

    if (findRes != EltFieldNameMap.end () && findRes->first != FLD_TOTAL) {
        root = make_shared <TemplateElement> (
            QString::fromStdString (node.nodeName () .toStdString () ),
            findRes->first, QString::fromStdString (_getAttributeValue (node, "desc") ) );

        root->_key = temp->_keyCounter ++;
        temp->_keyToElt [root->_key] = root;

        root->_param = 0;

        if (root->_field == FLD_FTXT)
            root->_format = QString::fromStdString (_getAttributeValue (node, "fmt") );

        if (! _getAttributeValue (node, "mandatory") .compare ("y") )
            root->_param |= PRM_MAND;
        if (! _getAttributeValue (node, "multiple") .compare ("y") )
            root->_param |= PRM_MULT;
        if (! _getAttributeValue (node, "parameter") .compare ("y") )
            root->_param |= PRM_PARM;

        if (root->_field != FLD_TOTAL) {
            QDomElement child = node.firstChildElement ();

            while (! child.isNull () ) {
                ptr = _getTemplate (temp, child);
                if (ptr != nullptr) {
                    root->_children.push_back (ptr);
                    ptr->_parent = root;
                }
                child = child.nextSiblingElement ();
            }
        }

        tmp = _getAttributeValue (node, "values");
        if (tmp.compare ("") )
            _extractSuggestedValues (tmp, root);
    }

    return root;
}

void
TemplateParser::getTemplate (
    shared_ptr <Template>   temp,
    const QString           & filepath) {
    QDomDocument			doc ("template");
    shared_ptr <TemplateElement> root {nullptr};
    QFile					file (filepath);

    temp->_tree = nullptr;
    temp->_keyCounter = 0;
    temp->_keyToElt.clear ();

    if (! file.open (QIODevice::ReadOnly) )
        { cout << "Error" << endl; return; }
    if (! doc.setContent (&file) )
        { cout << "Error2" << endl; file.close (); return; }
    file.close ();

    if (!doc.firstChildElement ("configuration") .isNull()) {
        temp->_tree = _getTemplate (temp, doc.firstChildElement ("configuration") );
        temp->_tree->_parent = nullptr;
    } else if (!doc.firstChildElement ("hardware") .isNull () ) {
        temp->_tree = _getTemplate (temp, doc.firstChildElement ("hardware") );
        temp->_tree->_parent = nullptr;
    } else if (!doc.firstChildElement ("device") .isNull()) {
        temp->_tree = _getTemplate (temp, doc.firstChildElement ("device") );
        temp->_tree->_parent = nullptr;
    }
}

void
_printTemplate (
    std::shared_ptr <TemplateElement> & elt,
    const int					indent) {
    if (elt == nullptr) return;

    for (auto i {indent}; i != 0; --i) cout << ' ';

    if (elt->_param & PRM_PARM) cout << "{";
    cout << elt->_name.toStdString ();

    switch (elt->_field) {
        case FLD_TEXT: case FLD_FILE:
            cout << ": < >";
            cout << " (" << elt->_desc.toStdString () << ")";
            break;
        case FLD_FTXT:
            cout << ": < " << elt->_format.toStdString () << " >";
            cout << " (" << elt->_desc.toStdString () << ")";
            break;
        case FLD_CBOX:
            cout << ": < >";
            if (elt->_values.size ()) {
                cout << " [";
                unsigned i {0};
                for (auto & flt: elt->_values) {
                    cout << flt.toStdString ();
                    if (i != elt->_values.size () - 1) cout << "; ";
                    ++i;
                }
                cout << "]";
            }
            cout << " (" << elt->_desc.toStdString () << ")";
            break;
        case FLD_DIR: case FLD_TOTAL: default:;// nothing
    }

    if (elt->_param & PRM_PARM) cout << "}";
    cout << endl;
    for (auto & flt: elt->_children) _printTemplate (flt, indent + 4);
}

void
TemplateParser::printTemplate (
    std::shared_ptr <TemplateElement> & tree) {
    _printTemplate (tree, 0);
}
