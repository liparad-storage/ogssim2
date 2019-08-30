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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QProcess>

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow (QWidget *parent = 0);
    ~MainWindow ();

    void simulate (QString cfgFile);

public slots:
    void simulationComplete (int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void quitMessageBox ();
    void operate (QString cfgFile);

private:
    Ui::MainWindow *ui;
};


class Launcher: public QObject {
    Q_OBJECT
public slots:
    void runSimulation (QString cfgFile);
signals:
    void simulationDone ();
};


#endif // MAINWINDOW_H
