/*
 * resultlistmodel.h - part of abakus
 * Copyright (C) 2012 Mathias Kraus <k.hias@gmx.net>
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

ResultModel::ResultModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[ExpressionRole] = "expression";
    roles[ResultRole] = "result";
    roles[TagRole] = "tag";
    roles[IndexRole] = "index";
    setRoleNames(roles);
}

ResultModel::~ResultModel()
{
    clear();
}

void ResultModel::addResult(const QString &expr, const Abakus::number_t &result)
{
    updateStackMarkers();
    
    ResultModelItem* resultModelItem = new ResultModelItem(expr, result);
    
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_resultModelItems << resultModelItem;
    endInsertRows();
}

void ResultModel::addMessage(const QString &msg)
{
    ResultModelItem* resultModelItem = new ResultModelItem(msg);
    
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_resultModelItems << resultModelItem;
    endInsertRows();
}

int ResultModel::rowCount(const QModelIndex & parent) const
{
    return m_resultModelItems.count();
}

bool ResultModel::stackValue(unsigned position, Abakus::number_t &result)
{
    if(position > 20) // Don't search forever
    {
        return false;
    }

    unsigned stackCount = 0;
    
    foreach(ResultModelItem* resultModelItem, m_resultModelItems)
    {
        if(resultModelItem->type() == ResultModelItem::Result)
        {
            if(stackCount == position)
            {
                result = resultModelItem->resultValue();
                return true;
            }
            
            ++stackCount;
        }
    }

    return false;
}

void ResultModel::updateStackMarkers()
{
    foreach(ResultModelItem* resultModelItem, m_resultModelItems)
    {
        resultModelItem->incrementTag();
    }
    
    emit dataChanged(index(0), index(rowCount() - 1));
}

QVariant ResultModel::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() > m_resultModelItems.count())
    {
        return QVariant();
    }
    
    ResultModelItem *resultModelItem = m_resultModelItems[index.row()];
    if (role == ExpressionRole)
    {
        return resultModelItem->expression();
    }
    else if (role == ResultRole)
    {
        return resultModelItem->result();
    }
    else if (role == TagRole)
    {
        return resultModelItem->tag();
    }
    else if (role == IndexRole)
    {
        return resultModelItem->index();
    }
    return QVariant();
}

void ResultModel::slotRedrawItems()
{
    if(rowCount() == 0)
    {
        return; // Don't emit signal if nothing changed.
    }

    foreach(ResultModelItem* resultModelItem, m_resultModelItems)
    {
        if(resultModelItem->type() == ResultModelItem::Result)
        {
            resultModelItem->updateResult();
        }
    }

    emit dataChanged(index(0), index(rowCount() - 1));
}

void ResultModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    while(!m_resultModelItems.isEmpty())
    {
        delete m_resultModelItems.takeFirst();
    }
    resetResultModelItemIndex();
    endRemoveRows();
}

#include "resultmodel.moc"

// vim: set et sw=4 ts=8:
