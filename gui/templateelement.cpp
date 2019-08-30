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

#include "templateelement.h"

#include <iostream>

#include <QDebug>

using namespace std;

TemplateElement::TemplateElement (
    const QString           name,
    const OGSG_EltField		field,
    const QString           desc):
    _name (name), _field (field), _desc (desc) {
    _children = std::vector <std::shared_ptr <TemplateElement>> ();
}

shared_ptr <TemplateElement>
Template::searchForTemplateElement (
    const QString           name,
    const QString           parent) {

    for (auto & i: _keyToElt) {
        if (! parent.compare ("") && ! i.second->_name.compare (name)
            && i.second->_parent == nullptr)
            return i.second;
        else if (parent.compare ("") && ! i.second->_name.compare (name)
            && ! i.second->_parent->_name.compare (parent) )
            return i.second;
    }

    return nullptr;
}
