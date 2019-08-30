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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <QMessageBox>
#include <QThread>
#include <QFile>
#include <QDomDocument>
#include <QProcess>
#include <QDebug>

const QString _GPH = "graph";

using namespace std;

void
_retrieveGraphPaths (QDomNode node, QStringList & list) {
    if (node.isNull () ) return;

    if (! node.nodeName () .compare (_GPH) )
        list << node.firstChild () .nodeValue ();

    _retrieveGraphPaths (node.firstChild (), list);
    _retrieveGraphPaths (node.nextSibling (), list);
}

QStringList
retrieveGraphPaths (QString cfgFile) {
    QDomDocument doc ("cfg");
    QFile file (cfgFile);
    QStringList list;

    file.open (QIODevice::ReadOnly);
    doc.setContent (&file);
    file.close ();

    _retrieveGraphPaths (doc.firstChild (), list);
    doc.firstChild () .nextSibling ();

    return list;
}

void
Launcher::runSimulation (QString cfgFile) {
    QProcess p;

#ifdef Q_OS_LINUX
    QString cmd = "cd ../..;./OGSSim " + cfgFile;
    p.start ("bash", QStringList () << "-c" << cmd);
    p.waitForFinished (-1);
    emit simulationDone ();
#endif
}

MainWindow::MainWindow (QWidget *parent) :
    QMainWindow (parent),
    ui (new Ui::MainWindow) {
    ui->setupUi (this);
    ui->tabSim->updateMainPtr (this);
    ui->tabViz->updateMainPtr (this);
    ui->tabEdt->updateMainPtr (this);
}

MainWindow::~MainWindow () {
    delete ui;
}

void
MainWindow::simulate (QString cfgFile) {
    QMessageBox box (QMessageBox::Information, "Simulation launching",
        "Simulation ongoing, please wait.", QMessageBox::NoButton, this);
    Launcher * simulator = new Launcher ();
    QThread worker;
    QStringList list;

    simulator->moveToThread (&worker);

    connect (this, &MainWindow::operate, simulator, &Launcher::runSimulation);
    connect (simulator, &Launcher::simulationDone, &box, &QMessageBox::accept);
//    connect (&sim, QOverload <int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::simulationComplete);
//    connect (this, &MainWindow::quitMessageBox, &box, &QMessageBox::accept);
//    connect (&sim, &QProcess::started, &box, &QMessageBox::exec);
//    connect (&worker, &QThread::started, &box, &QMessageBox::exec);
    connect (&worker, &QThread::finished, simulator, &QObject::deleteLater);
    connect (&box, &QMessageBox::rejected, &worker, &QThread::terminate);
//    connect (&box, &QMessageBox::rejected, &sim, &QProcess::terminate);

    box.addButton ("Abort", QMessageBox::RejectRole);

    worker.start ();
    emit operate (cfgFile);

    box.exec ();

    worker.quit ();
    worker.wait ();

    list = retrieveGraphPaths (cfgFile);

    ui->tabViz->updatePanel (list);
}

void
MainWindow::simulationComplete (
    int exitCode,
    QProcess::ExitStatus exitStatus) {
    (void) exitCode; (void) exitStatus;
    emit quitMessageBox ();
}
