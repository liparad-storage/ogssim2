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

#ifndef SIMULATION_H
#define SIMULATION_H

#include <QWidget>

namespace Ui {
class Simulation;
}

class Simulation: public QWidget {
    Q_OBJECT

public:
    explicit Simulation(QWidget *parent = 0);
    ~Simulation();

    void updateMainPtr (QWidget * main) { _main = main; }

public slots:
    void actionBrowse ();
    void actionLaunch ();
    void actionRefresh ();
    void actionSaveAs ();
    void actionSave ();
    void disableSaveButton ();
    void enableSaveButton ();
    void enableLaunchButton ();

signals:
    void updatePath (QString fname);
    void updateFile (QString contents);

private:
    QString loadFileContents ();
    void createFileContents (QString contents);
    void updateFileContents (QString contents);

    Ui::Simulation *ui;
    QString _fname;
    QWidget * _main;
};

#endif // SIMULATION_H
