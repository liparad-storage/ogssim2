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

#include "templateitemdelegate.h"

#include <iostream>

#include <QComboBox>
#include <QToolButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFileDialog>

using namespace std;

TemplateItemDelegate::TemplateItemDelegate (
    shared_ptr <Template> temp,
    QObject * parent):
    QStyledItemDelegate (parent) {
    _template = temp;
}

TemplateItemDelegate::~TemplateItemDelegate () {  }

QWidget *
TemplateItemDelegate::createEditor (
    QWidget * parent,
    const QStyleOptionViewItem & option,
    const QModelIndex & index) const {
    QComboBox * cb;
    QLineEdit * le;
    QToolButton * tb;
    QWidget * wg;
    QHBoxLayout * hl;
    if (index.column () != 1)
        return QStyledItemDelegate::createEditor (parent, option, index);

    switch (_template->_keyToElt.at (
        index.siblingAtColumn (3) .data () .toInt () ) ->_field) {
        case FLD_CBOX:
            cb = new QComboBox (parent);
            for (auto & elt: _template->_keyToElt.at (
                 index.siblingAtColumn (3) .data () .toInt () ) ->_values)
                cb->addItem (elt);
            return cb;
        case FLD_TEXT: case FLD_FTXT:
            le = new QLineEdit (parent);
            le->setText ("und.");
            return le;
        case FLD_FILE:
            wg = new QWidget (parent);
            le = new QLineEdit ("", wg);
            le->setObjectName ("pathline");
            tb = new QToolButton (wg);
            hl = new QHBoxLayout ();
            hl->addWidget (le);
            hl->addWidget (tb);
            wg->setLayout (hl);

            hl->setContentsMargins (0, 0, 0, 0);
            hl->setSpacing (0);

            tb->setText ("...");
            tb->resize (20, 18);

            connect (tb, &QToolButton::pressed, this, &TemplateItemDelegate::actionBrowse);
            connect (this, &TemplateItemDelegate::updatePath, le, &QLineEdit::setText);

            return wg;

        default:
            return QStyledItemDelegate::createEditor (parent, option, index);
    }
}

void
TemplateItemDelegate::setEditorData (
    QWidget * editor,
    const QModelIndex & index) const {
    QString currentText;
    int currentIndex;
    QComboBox * cb;
    QLineEdit * le;
    switch (_template->_keyToElt.at (
        index.siblingAtColumn (3) .data () .toInt () ) ->_field) {
        case FLD_CBOX:
            cb = qobject_cast <QComboBox *> (editor);
            currentText = index.data (Qt::EditRole) .toString ();
            currentIndex = cb->findText (currentText);
            if (currentIndex >= 0)
                cb->setCurrentIndex (currentIndex);
        break;
        case FLD_FILE:
            le = editor->findChild <QLineEdit *> ("pathline");
            le->setText (index.data (Qt::EditRole) .toString () );
        break;
        case FLD_TEXT: case FLD_FTXT:
            le = qobject_cast <QLineEdit *> (editor);
            currentText = index.data (Qt::EditRole) .toString ();
            le->setText (currentText);
            break;

        default:
        QStyledItemDelegate::setEditorData (editor, index);
    }
}

void
TemplateItemDelegate::setModelData (
    QWidget * editor,
    QAbstractItemModel * model,
    const QModelIndex & index) const {
    QComboBox * cb;
    QLineEdit * le;
    switch (_template->_keyToElt.at (
        index.siblingAtColumn (3) .data () .toInt () ) ->_field) {
        case FLD_CBOX:
            cb = qobject_cast <QComboBox *> (editor);
            model->setData (index, cb->currentText () );
        break;
        case FLD_FILE:
            le = editor->findChild <QLineEdit *> ("pathline");
            model->setData (index, le->text () );
        break;
        case FLD_TEXT: case FLD_FTXT:
            le = qobject_cast <QLineEdit *> (editor);
            model->setData (index, le->text () );
        break;
        default:
            QStyledItemDelegate::setModelData (editor, model, index);
    }
}

void
TemplateItemDelegate::actionBrowse () {
    QString fname = QFileDialog::getOpenFileName (qobject_cast <QWidget *> (parent () ), "Browse file");

    cout << "Chosen file is: " << fname.toStdString () << endl;

    emit updatePath (fname);
}
