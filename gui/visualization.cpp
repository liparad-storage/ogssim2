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

#include "visualization.h"
#include "ui_visualization.h"

#include <iostream>

#include <QLabel>

#ifdef Q_OS_LINUX
#include <poppler/qt5/poppler-qt5.h>
#endif

using namespace std;

const QString FRAME_MESSAGE =   "Select a visualization graph on the left panel to observe it here.<br />"
                                "If there is no available graph on the left panel, it means either or both:<br />"
                                "  - you did not run a simulation;<br />"
                                "  - you did not inquire any PDF/PNG graphs as simulation output.";
const QString FRAME_UNKEXT =    "The selected graph can not be visualized (format not supported).<br />"
                                "Please select a PNG graph.";
const QString EXTPNG =          ".png";
const QString EXTPDF =          ".pdf";

Visualization::Visualization(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Visualization)
{
    ui->setupUi(this);
    _model = new QStringListModel (this);
    ui->listView->setModel (_model);
}

Visualization::~Visualization()
{
    delete _model;
    delete ui;
}

void
Visualization::printFrameFromSelection (QModelIndex id) {
//    ui->label->setText (model->data (id) .toString ());
    if (_model->data (id) .toString () .endsWith (EXTPNG) ) {
        QPixmap p ("../../" + _model->data (id) .toString () );
//    p = p.scaled (ui->frame->size (), Qt::KeepAspectRatio);
        ui->label->setPixmap (p);
#ifdef Q_OS_LINUX
    } else if (_model->data (id) .toString () .endsWith (EXTPDF) ) {
        Poppler::Document * document = Poppler::Document::load ("../../" + _model->data (id) .toString () );
        QImage image = document->page (0) ->renderToImage (
                    ui->frame->physicalDpiX (), ui->frame->physicalDpiY () );
        ui->label->setPixmap (QPixmap::fromImage (image) );
        delete document;
#endif
    } else {
        ui->label->setPixmap (QPixmap ());
        ui->label->setText (FRAME_UNKEXT);
    }
    repaint ();
}

void
Visualization::updatePanel (QStringList list) {
    _model->setStringList (list);
}

void
Visualization::initPanel () {
    QStringList l;
    l << "latency" << "device profile: 1-14" << "device profile: 15-18" << "bandwidth: 1-4";

    _model = new QStringListModel (this);
    _model->setStringList (l);

    ui->listView->setModel (_model);
}
