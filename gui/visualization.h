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

#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QStringListModel>
#include <QWidget>

namespace Ui {
class Visualization;
}

class Visualization : public QWidget
{
    Q_OBJECT

public:
    explicit Visualization(QWidget *parent = 0);
    ~Visualization();

    void updatePanel (QStringList list);
    void updateMainPtr (QWidget * main) { _main = main; }

public slots:
    void printFrameFromSelection (QModelIndex id);

private:
    void initPanel ();
    Ui::Visualization *ui;

    QStringListModel * _model;
    QWidget * _main;
};

#endif // VISUALIZATION_H
