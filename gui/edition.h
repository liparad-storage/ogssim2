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

#ifndef EDITION_H
#define EDITION_H

#include <memory>

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QWidget>

#include "templateitemdelegate.h"
#include "templateitemmodel.h"
#include "templateparser.h"

namespace Ui {
class Edition;
}

class Edition: public QWidget {
    Q_OBJECT

public:
    explicit Edition (QWidget * parent = 0);
    ~Edition ();

    void updateMainPtr (QWidget * main) { _main = main; }

public slots:
    void actionNewConfiguration ();
    void actionNewHardware ();
    void actionNewHDD ();
    void actionNewSSD ();
    void actionAddItem ();
    void actionNewNVRAM ();
    void actionRemoveItem ();
    void actionSave ();
    void actionLoad ();
    void disableSaveButton ();
    void enableSaveButton ();
    void manageContextMenu (const QPoint &);

private:
    void setupTreeView ();

    Ui::Edition * ui;
    QWidget * _main;
    std::shared_ptr <TemplateItemModel> _model;
    std::shared_ptr <TemplateItemDelegate> _style;
    std::shared_ptr <Template> _template;
};

#endif // EDITION_H
