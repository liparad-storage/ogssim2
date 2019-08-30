#
# Copyright UVSQ - CEA/DAM/DIF (2018)
# Contributors:  Sebastien GOUGEAUD  -- sebastien.gougeaud@uvsq.fr
#                Soraya ZERTAL       --      soraya.zertal@uvsq.fr
#
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
#

#-------------------------------------------------
#
# Project created by QtCreator 2018-06-21T10:17:02
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OGSSGui
TEMPLATE = app

unix {
INCLUDEPATH += /usr/include/poppler/qt5
LIBS += -L/usr/lib -lpoppler-qt5
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp \
    visualization.cpp \
    simulation.cpp \
    edition.cpp \
    templateparser.cpp \
    templateitemmodel.cpp \
    templateitemdelegate.cpp \
    templateelement.cpp

HEADERS += \
    mainwindow.h \
    visualization.h \
    simulation.h \
    edition.h \
    templateparser.h \
    templateitemmodel.h \
    templateitemdelegate.h \
    templateelement.h

FORMS += \
    mainwindow.ui \
    visualization.ui \
    simulation.ui \
    edition.ui

DISTFILES += \
    cfg/guiConfig.xml \
    cfg/template-config.xml \
    cfg/template-hardware.xml \
    cfg/template-hdd.xml \
    cfg/template-ssd.xml \
    cfg/template-nvram.xml \
    cfg/template-hardware.xml \
    cfg/template-hdd.xml \
    cfg/template-ssd.xml

unix {
QMAKE_POST_LINK += $${QMAKE_COPY_DIR} $$shell_path($${PWD}/cfg) $$shell_path($${OUT_PWD})
}
win32 {
exists($${OUT_PWD}/cfg){
} else {
QMAKE_POST_LINK += $${QMAKE_MKDIR} $$shell_path($${OUT_PWD}/cfg)$$escape_expand(\n\t)
}
QMAKE_POST_LINK += $${QMAKE_COPY_DIR} $$shell_path($${PWD}/cfg) $$shell_path($${OUT_PWD}/cfg)
}
