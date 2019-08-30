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

#ifndef TEMPLATEELEMENT_H
#define TEMPLATEELEMENT_H

#include <map>
#include <memory>
#include <set>
#include <vector>

#include <QString>

enum OGSG_EltField {
    FLD_TEXT,
    FLD_CBOX,
    FLD_FTXT,
    FLD_FILE,
    FLD_DIR,
    FLD_LEAF,
    FLD_TOTAL
};

const std::map <OGSG_EltField, std::string>
                                EltFieldNameMap = {
    {FLD_TEXT,					"text"},
    {FLD_CBOX,					"cbox"},
    {FLD_FTXT,          		"ftxt"},
    {FLD_FILE,              	"file"},
    {FLD_DIR,					"diry"},
    {FLD_LEAF,                  "leaf"},
    {FLD_TOTAL,					"und."}
};

const int PRM_MAND = 0b0001;
const int PRM_MULT = 0b0010;
const int PRM_PARM = 0b0100;

struct TemplateElement {
    TemplateElement (
        const QString           name,
        const OGSG_EltField		field,
        const QString           desc = "");

    std::vector <std::shared_ptr <TemplateElement>>	_children;
    std::shared_ptr <TemplateElement>               _parent;
    QString                     _name;
    OGSG_EltField				_field;
    int                         _param;
    QString                 	_desc;
    std::set <QString>          _values;
    QString                 	_format;
    unsigned                    _key;
};

struct Template {
    std::shared_ptr <TemplateElement> searchForTemplateElement (const QString name, const QString parent);

    std::shared_ptr <TemplateElement> _tree;
    std::map <unsigned, std::shared_ptr <TemplateElement>> _keyToElt;
    unsigned _keyCounter;
};

#endif // TEMPLATEELEMENT_H
