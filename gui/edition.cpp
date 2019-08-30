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

#include "edition.h"
#include "ui_edition.h"

#include <iostream>

#include <QMessageBox>
#include <QFileDialog>
#include <QMenu>

#define MDF_COLUMN 2

using namespace std;

Edition::Edition (QWidget * parent):
    QWidget (parent),
    ui (new Ui::Edition) {
    ui->setupUi(this);
    setupTreeView ();
}

Edition::~Edition () {
    delete ui;
}

void
Edition::actionNewConfiguration () {
    TemplateParser::getTemplate (_template, "cfg/template-config.xml");
    _model->generateModelFromTemplate ();
    enableSaveButton ();
}

void
Edition::actionNewHardware () {
    TemplateParser::getTemplate (_template, "cfg/template-hardware.xml");
    _model->generateModelFromTemplate ();
    enableSaveButton ();
}

void
Edition::actionNewHDD () {
    TemplateParser::getTemplate (_template, "cfg/template-hdd.xml");
    _model->generateModelFromTemplate ();
    enableSaveButton ();
}

void
Edition::actionNewSSD () {
    TemplateParser::getTemplate (_template, "cfg/template-ssd.xml");
    _model->generateModelFromTemplate ();
    enableSaveButton ();
}

void
Edition::actionNewNVRAM () {
    TemplateParser::getTemplate (_template, "cfg/template-nvram.xml");
    _model->generateModelFromTemplate ();
    enableSaveButton ();
}

void
Edition::actionAddItem () {
    std::shared_ptr <TemplateElement> elt =
        _template->_keyToElt.at (ui->treeView->currentIndex () .siblingAtColumn (3) .data () .toInt () );

    QAction * act = qobject_cast <QAction *> (sender () );

    for (auto & i: elt->_children)
        if (! i->_name.compare (act->text () ) ) { elt = i; break; }

    _model->addItem (elt, _model->itemFromIndex (
        ui->treeView->currentIndex () .siblingAtColumn (0) ) );
}

void
Edition::actionRemoveItem () {
    _model->removeItem (_model->itemFromIndex (
        ui->treeView->currentIndex () .siblingAtColumn (0) ) );
}

void
Edition::actionSave () {
    QString fname = QFileDialog::getSaveFileName (this, "Save file");
    _model->saveModelToFile (fname);
    ui->lineEdit->setText (fname);
}

void
Edition::actionLoad () {
    QString fname = QFileDialog::getOpenFileName (this, "Open file");
    if (! _model->loadModelFromFile (fname) ) {
        QMessageBox::critical (this, "File error", "The selected file is not valid!");
        return;
    }
    ui->lineEdit->setText (fname);
}

void
Edition::disableSaveButton () {
    ui->butSave->setEnabled (false);
    ui->butSave->repaint ();
}

void
Edition::enableSaveButton () {
    if (ui->butSave->isEnabled () ) return;

    ui->butSave->setEnabled (true);
    ui->butSave->repaint ();
}

void
Edition::setupTreeView () {
    _template = make_shared <Template> ();
    _model = make_shared <TemplateItemModel> (_template, ui->treeView);
    _style = make_shared <TemplateItemDelegate> (_template, ui->treeView);

    ui->treeView->setModel (_model .get () );
    ui->treeView->setItemDelegate (_style .get () );
    ui->treeView->setColumnHidden (3, true);
    ui->treeView->setColumnWidth (0, ui->treeView->columnWidth (0) * 2);
    ui->treeView->setColumnWidth (1, ui->treeView->columnWidth (1) * 2);

    ui->treeView->setContextMenuPolicy (Qt::CustomContextMenu);
    connect (ui->treeView, &QTreeView::customContextMenuRequested,
             this, &Edition::manageContextMenu);

    QMenu * menu = new QMenu (ui->butNew);
    QAction * act;

    act = menu->addAction ("Configuration");
    connect (act, &QAction::triggered, this, &Edition::actionNewConfiguration);
    act = menu->addAction ("Hardware");
    connect (act, &QAction::triggered, this, &Edition::actionNewHardware);
    act = menu->addAction ("HDD");
    connect (act, &QAction::triggered, this, &Edition::actionNewHDD);
    act = menu->addAction ("SSD");
    connect (act, &QAction::triggered, this, &Edition::actionNewSSD);
    act = menu->addAction ("NVRAM");
    connect (act, &QAction::triggered, this, &Edition::actionNewNVRAM);
    ui->butNew->setMenu (menu);
}

void
Edition::manageContextMenu (
    const QPoint & point) {
    QModelIndex index = ui->treeView->indexAt (point);
    QMenu * contextMenu, * men;
    QAction * act;
    shared_ptr <TemplateElement> elt;
    int nb {0};
    if (index.isValid () ) {
        elt = _template->_keyToElt.at (index.siblingAtColumn (3) .data () .toInt () );
        contextMenu = new QMenu (ui->treeView);
        men = contextMenu->addMenu ("Add");
        for (auto & i: elt->_children) {
            if (i->_param & PRM_MULT) {
                act = men->addAction (i->_name);
                connect (act, &QAction::triggered, this, &Edition::actionAddItem);
                ++nb;
            }
        }
        if (! nb)
            men->setEnabled (false);

        act = contextMenu->addAction ("Remove");
        connect (act, &QAction::triggered, this, &Edition::actionRemoveItem);
        if (elt->_param & PRM_MAND)
            act->setEnabled(false);

        contextMenu->exec (ui->treeView->mapToGlobal (point) );
    }
}
