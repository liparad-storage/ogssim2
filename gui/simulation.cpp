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

#include "simulation.h"
#include "ui_simulation.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QTextCodec>
#include <iostream>

#include "mainwindow.h"

using namespace std;

Simulation::Simulation (QWidget *parent) :
    QWidget (parent),
    ui (new Ui::Simulation) {
    ui->setupUi (this);
}

Simulation::~Simulation () {
    delete ui;
}

void Simulation::actionBrowse () {
    _fname = QFileDialog::getOpenFileName (this, "Open file");

    emit updatePath (_fname);
    emit updateFile (loadFileContents ());
    disableSaveButton ();
    enableLaunchButton ();
}
void Simulation::actionLaunch () {
    if (! _fname.compare ("") )
        QMessageBox::warning (this, "Simulation launching", "No configuration file selected!");
    else {
        qobject_cast <MainWindow *> (_main) ->simulate (_fname);
    }
}
void Simulation::actionRefresh () {
    emit updateFile (loadFileContents ());
    disableSaveButton ();
}
void Simulation::actionSaveAs () {
    _fname = QFileDialog::getSaveFileName (this, "Save file");

    updateFileContents (ui->fileBrowser->document () ->toPlainText () );

    emit updatePath (_fname);
    disableSaveButton ();
}
void Simulation::actionSave () {
    updateFileContents (ui->fileBrowser->document () ->toPlainText () );

    disableSaveButton ();
}
void Simulation::disableSaveButton () {
    ui->butSave->setEnabled (false);
    ui->butSave->repaint ();
}
void Simulation::enableSaveButton () {
    if (ui->butSave->isEnabled()) return;

    ui->butSave->setEnabled (true);
    ui->butSave->repaint ();
}

void Simulation::enableLaunchButton () {
    if (ui->butLaunch->isEnabled ()) return;

    ui->butLaunch->setEnabled (true);
    ui->butLaunch->repaint ();
}

QString Simulation::loadFileContents () {
    QFile file (_fname);

    if (!file.open (QIODevice::ReadOnly | QIODevice::Text) )
        return "";

    return QTextCodec::codecForMib (106) ->toUnicode (file.readAll () );
}
void Simulation::createFileContents (QString contents) {
    QFile file (_fname);

    if (!file.open (QIODevice::WriteOnly | QIODevice::Text) )
        return;

    file.write (contents.toUtf8 () );
}
void Simulation::updateFileContents (QString contents) {
    QFile file (_fname);

    if (!file.open (QIODevice::WriteOnly | QIODevice::Text) )
        return;

    file.resize (0);
    file.write (contents.toUtf8 () );
}
