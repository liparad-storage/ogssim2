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

#ifndef TEMPLATEITEMDELEGATE_H
#define TEMPLATEITEMDELEGATE_H

#include <memory>

#include <QStyledItemDelegate>

#include "templateelement.h"

class TemplateItemDelegate:
public QStyledItemDelegate {
    Q_OBJECT
public:
    TemplateItemDelegate (
        std::shared_ptr <Template> temp,
        QObject * parent = 0);
    ~TemplateItemDelegate ();

    virtual QWidget * createEditor (
        QWidget * parent,
        const QStyleOptionViewItem & option,
        const QModelIndex & index) const;
    virtual void setEditorData (
        QWidget * editor,
        const QModelIndex & index) const;
    virtual void setModelData (
        QWidget * editor,
        QAbstractItemModel * model,
        const QModelIndex & index) const;
public slots:
    void actionBrowse ();

signals:
    void updatePath (QString fname);

private:
    std::shared_ptr <Template> _template;
};

#endif // TEMPLATEITEMDELEGATE_H
