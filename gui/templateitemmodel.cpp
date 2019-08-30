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

#include "templateitemmodel.h"

#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QDebug>

#include "templateparser.h"

using namespace std;

TemplateItemModel::TemplateItemModel (
    std::shared_ptr <Template> temp,
    QObject * parent):
    QStandardItemModel (parent) {
    QStringList list;
    list << "Name" << "Value" << "Description" << "ID";
    setHorizontalHeaderLabels (list);
    _template = temp;
}

TemplateItemModel::~TemplateItemModel () {  }

Qt::ItemFlags
TemplateItemModel::flags (
    const QModelIndex & index) const {
    if (index.column () == 1 && _template->_keyToElt.at (
                index.siblingAtColumn (3) .data () .toInt () ) ->_field != FLD_DIR)
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    else
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void
TemplateItemModel::addItem (
    std::shared_ptr <TemplateElement> node,
    QStandardItem * elt) {
    QList <QStandardItem *> list;
    QStandardItem * e1, * e2, * e3, * e4;
    QFont f;

    e1 = new QStandardItem (node->_name);
    e2 = new QStandardItem ();
    e3 = new QStandardItem (node->_desc);
    e4 = new QStandardItem (QString::number (node->_key) );

    if (node->_param & PRM_PARM) {
        f = e1->font (); f.setItalic (true);
        e1->setFont (f); e2->setFont (f); e3->setFont (f);
    }

    list.append (e1); list.append (e2); list.append (e3); list.append (e4);

    elt->appendRow (list);

    for (auto & flt: node->_children) {
        if (flt->_param & PRM_MAND)
            addItem (flt, elt->child (elt->rowCount () - 1, 0) );
    }
}

void
TemplateItemModel::removeItem (
    QStandardItem * elt) {
    QStandardItem * parent = elt->parent ();
    QModelIndex ind = elt->index ();
    QList <QStandardItem *> list = parent->takeRow (ind.row () );

    for (auto i = list.begin (); i != list.end (); ++i)
        delete *i;
}

void
TemplateItemModel::_save (
    QDomDocument doc,
    QDomElement root,
    QStandardItem * elt) {
    if (elt == nullptr) return;

    QStandardItem * tmp;

    if (_template->_keyToElt.at (
                elt->index () .siblingAtColumn (3) .data () .toInt () ) ->_param & PRM_PARM) {
        QDomAttr attr = doc.createAttribute (elt->index () .data () .toString () );
        attr.setValue (elt->index () .siblingAtColumn (1) .data () .toString () );
        root.setAttributeNode (attr);
    } else {
        QDomElement node = doc.createElement (elt->index () .data () .toString() );
        if (_template->_keyToElt.at (
            elt->index () .siblingAtColumn (3) .data () .toInt () ) ->_field != FLD_DIR) {
            QDomText txt = doc.createTextNode (elt->index () .siblingAtColumn (1) .data () .toString () );
            //node.setNodeValue (elt->index () .siblingAtColumn (1) .data () .toString () );
            node.appendChild (txt);
        }
        root.appendChild (node);

        for (auto i = 0; i < elt->rowCount (); ++i) {
            tmp = elt->child (i);
            _save (doc, node, tmp);
        }
    }
}

bool
TemplateItemModel::saveModelToFile (
    const QString filename) {

    QDomDocument doc;
    doc.appendChild (doc.createProcessingInstruction("xml", "version=\"1.0\"") );

    QFile file (filename);

    if (! file.open (QIODevice::WriteOnly) )
        { cout << "Error on opening" << endl; return false; }

    QStandardItem * elt = invisibleRootItem () ->child (0);
    QStandardItem * tmp;

    doc.appendChild (doc.createElement (elt->index () .data () .toString () ) );

    for (auto i = 0; i < elt->rowCount (); ++i) {
        tmp = elt->child (i);
        _save (doc, doc.documentElement (), tmp);
    }

    QTextStream stream (&file);
    stream.setCodec ("UTF-8");
    doc.save (stream, 4);
    file.close ();

    return true;
}

void
TemplateItemModel::_load (
    QDomElement             root,
    QStandardItem           * parent) {
    QStandardItem * e1, * e2, * e3, * e4;
    QStandardItem * elt;
    QList <QStandardItem *> list;
    std::shared_ptr <TemplateElement> ptr;

    if (root.isNull () ) return;

    if (! root.parentNode () .nodeName () .compare ("#document") )
        ptr = _template->searchForTemplateElement (root.tagName (), "");
    else
        ptr = _template->searchForTemplateElement (
            root.tagName (), root.parentNode () .nodeName () );

    e1 = new QStandardItem (root.tagName () );      list.append (e1);
    e2 = new QStandardItem ();    list.append (e2);
    e3 = new QStandardItem (ptr->_desc);            list.append (e3);
    e4 = new QStandardItem (QString::number (ptr->_key) );             list.append (e4);

    parent->appendRow (list);
    elt = e1;

    if (root.hasAttributes () ) {
        QDomNamedNodeMap attrMap;
        attrMap = root.attributes ();

        for (auto i = 0; i < attrMap.length (); ++i) {
            list.clear ();

            ptr = _template->searchForTemplateElement (
                attrMap.item (i) .nodeName (), root.tagName () );

            e1 = new QStandardItem (attrMap.item (i) .nodeName () );
            e2 = new QStandardItem (attrMap.item (i) .nodeValue () );
            e3 = new QStandardItem (ptr->_desc);
            e4 = new QStandardItem (QString::number (ptr->_key) );

            QFont f = e1->font (); f.setItalic (true);
            e1->setFont (f); e2->setFont (f); e3->setFont (f);

            list.append (e1); list.append (e2); list.append (e3); list.append (e4);

            elt->appendRow (list);
        }
    }

    if (root.hasChildNodes () ) {
        QDomNode tch = root.firstChild ();

        while (! tch.isNull () ) {
            if (tch.isText () ) {
                setData (elt->index () .siblingAtColumn (1), tch.toText () .data () );
                break;
            }
            tch = tch.nextSibling ();
        }

        QDomElement child = root.firstChildElement ();

        while (! child.isNull () ) {
            _load (child, elt);

            child = child.nextSiblingElement ();
        }
    }

}

bool
TemplateItemModel::loadModelFromFile (
    const QString           filename) {
    QDomDocument            doc ("template");
    QFile                   file (filename);
    QDomElement             root;

    invisibleRootItem () ->removeRow (0);

    if (! file.open (QIODevice::ReadOnly) )
    { cout << "File '" << filename.toStdString () << "' not openable" << endl; return false; }
    if (! doc.setContent (&file) )
    { cout << "File '" << filename.toStdString () << "' not XML format" << endl; return false; }
    file.close ();

    root = doc.firstChildElement ("configuration");
    if (! root.isNull () ) {
        TemplateParser::getTemplate (_template, "cfg/template-config.xml");
        _load (root, invisibleRootItem () );
        return true;
    }

    root = doc.firstChildElement ("hardware");
    if (! root.isNull () ) {
        TemplateParser::getTemplate (_template, "cfg/template-hardware.xml");
        _load (root, invisibleRootItem () );
        return true;
    }

    root = doc.firstChildElement ("device");
    if (! root.isNull () ) {
        if (root.hasAttributes () ) {
            QDomNamedNodeMap attrL = root.attributes ();
            for (auto i = 0; i < attrL.length (); ++i) {
                if (! attrL.item (i) .nodeName () .compare ("type") ) {
                    if (! attrL.item (i) .nodeValue () .compare ("hdd") ) {
                        TemplateParser::getTemplate (_template, "cfg/template-hdd.xml");
                        _load (root, invisibleRootItem () );
                        return true;
                    }
                    if (! attrL.item (i) .nodeValue () .compare ("ssd") ) {
                        TemplateParser::getTemplate (_template, "cfg/template-ssd.xml");
                        _load (root, invisibleRootItem () );
                        return true;
                    }
                    if (! attrL.item (i) .nodeValue () .compare ("nvram") ) {
                        TemplateParser::getTemplate (_template, "cfg/template-nvram.xml");
                        _load (root, invisibleRootItem () );
                        return true;
                    }
                }
            }
        }
    }

    qDebug () << "File '" << filename << "' not an OGSSim configuration file";
    return false;
}

void
TemplateItemModel::_generate (
    std::shared_ptr <TemplateElement> node,
    QStandardItem * parent) {
    QStandardItem * e1, * e2, * e3, * e4;
    QList <QStandardItem *> list;
    QFont f;
    if (node == nullptr) return;

    if (! (node->_param & PRM_MAND) ) return;

    e1 = new QStandardItem (node->_name);
    e2 = new QStandardItem ();
    e3 = new QStandardItem (node->_desc);
    e4 = new QStandardItem (QString::number (node->_key) );

    if (node->_param & PRM_PARM) {
        f = e1->font (); f.setItalic (true);
        e1->setFont (f); e2->setFont (f); e3->setFont (f);
    }

    list.append (e1); list.append (e2); list.append (e3); list.append (e4);
    parent->appendRow (list);

    if (! node->_children.empty() )
        for (auto & i: node->_children)
            _generate (i, list.at (0) );
}

void
TemplateItemModel::generateModelFromTemplate () {
    invisibleRootItem () ->removeRow (0);
    _generate (_template->_tree, invisibleRootItem () );
}
