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

#ifndef TEMPLATEITEMMODEL_H
#define TEMPLATEITEMMODEL_H

#include <memory>

#include <QDomNode>
#include <QStandardItemModel>

#include "templateelement.h"

class TemplateItemModel:
public QStandardItemModel {
    Q_OBJECT
public:
    TemplateItemModel (
        std::shared_ptr <Template> temp,
        QObject * parent = 0);
    ~TemplateItemModel ();

    Qt::ItemFlags flags (const QModelIndex & index) const;

    bool saveModelToFile (const QString filename);
    bool loadModelFromFile (const QString filename);
    void generateModelFromTemplate ();

    void addItem (
        std::shared_ptr <TemplateElement> node,
        QStandardItem * elt);
    void removeItem (
        QStandardItem * elt);

public slots:

private:
    void _load (
        QDomElement root,
        QStandardItem * parent);
    void _save (
        QDomDocument doc,
        QDomElement root,
        QStandardItem * elt);
    void _generate (
        std::shared_ptr <TemplateElement> node,
        QStandardItem * parent);

    std::shared_ptr <Template> _template;
};

#endif // TEMPLATEITEMMODEL_H
