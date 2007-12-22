/*
 * resultlistmodel.h - part of abakus
 * Copyright (C) 2007 Michael Pyne <michael.pyne@kdemail.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "resultmodel.h"

#include <kdebug.h>
#include <klocale.h>

#include <QtGui/QStandardItemModel>

// Ensure Abakus::number_t is added to Qt's meta object system for QVariant use
#include "numerictypes.h"
Q_DECLARE_METATYPE(Abakus::number_t);
#include <QtCore/QVariant>

static const int ResultStatusType = QStandardItem::UserType + 2;

ResultModel::ResultModel(QObject *parent)
    : QObject(parent), m_model(new QStandardItemModel(0, 3, this))
{
    QStringList labels;

    labels << i18nc("Typed in math formula", "Expression")
           << i18nc("Result of math formula", "Result")
           << i18nc("Placeholder of result for use in later formulas", "Tag");
    m_model->setHorizontalHeaderLabels(labels);
}

QStandardItemModel *ResultModel::model()
{
    return m_model;
}

void ResultModel::addResult(const QString &expr, const Abakus::number_t &result)
{
    QStandardItemModel *m = model();

    QList<QStandardItem *> items;
    QStandardItem *item;

    items << new QStandardItem(expr);

    items << (item = new QStandardItem(result.toString()));
    item->setData(QVariant::fromValue(result)); // Save the actual value for later.

    items << (item = new QStandardItem("$0"));
    item->setTextAlignment(Qt::AlignRight);

    m->invisibleRootItem()->appendRow(items);
    updateStackMarkers();
}

class ResultTextItem : public QStandardItem
{
    public:
    ResultTextItem(const QString &text) : QStandardItem(text)
    {
    }

    virtual int type() const
    {
        return ResultStatusType;
    }
};

void ResultModel::addMessage(const QString &msg)
{
    QStandardItemModel *m = model();

    QStandardItem *item = new ResultTextItem(msg);

    m->invisibleRootItem()->appendRow(item);
}

bool ResultModel::stackValue(unsigned position, Abakus::number_t &result)
{
    if(position > 20) // Don't search forever
        return false;

    unsigned stackCount = 0;
    for(int i = model()->invisibleRootItem()->rowCount() - 1; i >= 0; --i) {
        QStandardItem *item = model()->invisibleRootItem()->child(i, 1);
        if(!item || item->type() == ResultStatusType)
            continue; // Don't increment stackCount for these.

        if(stackCount == position) {
            result = item->data().value<Abakus::number_t>();
            return true;
        }

        ++stackCount;
    }

    return false;
}

void ResultModel::updateStackMarkers()
{
    // Update stack stuff
    unsigned stackCount = 0;
    for(int i = model()->invisibleRootItem()->rowCount() - 1; i >= 0; --i) {
        QStandardItem *item = model()->invisibleRootItem()->child(i, 2);
        if(!item || item->type() == ResultStatusType) {
            continue; // Don't increment stackCount for these.
        }

        item->setText(QString("$%1").arg(stackCount));
        ++stackCount;
    }
}

void ResultModel::slotRedrawItems()
{
    if(model()->invisibleRootItem()->rowCount() == 0)
        return; // Don't emit signal if nothing changed.

    for(int i = 0; i < model()->invisibleRootItem()->rowCount(); ++i) {
        QStandardItem *item = model()->invisibleRootItem()->child(i, 1);
        if(!item || item->type() == ResultStatusType)
            continue;

        Abakus::number_t value = item->data().value<Abakus::number_t>();
        item->setText(value.toString());
    }

    emit signalColumnChanged(1);
}

void ResultModel::clear()
{
    m_model->setRowCount(0);
}

#include "resultmodel.moc"

// vim: set et sw=4 ts=8:
