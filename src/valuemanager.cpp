/*
 * valuemanager.cpp - part of abakus
 * Copyright (C) 2004, 2005 Michael Pyne <michael.pyne@kdemail.net>
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
#include <kdebug.h>
#include <klocale.h>

#include <QtCore/QRegExp>

#include "numerictypes.h"
#include "valuemanager.h"

ValueManager *ValueManager::m_manager = 0;

ValueManager *ValueManager::instance()
{
    if(!m_manager)
        m_manager = new ValueManager;

    return m_manager;
}

ValueManager::ValueManager(QObject *parent) : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueStringRole] = "valueString";
    roles[DescriptionRole] = "description";
    roles[TypeStringRole] = "typeString";
    setRoleNames(roles);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_numeralModelItems << new NumeralModelItem("ans", Abakus::number_t(42), NumeralModelItem::AbakusVariable);
    m_numeralModelItems << new NumeralModelItem("pi", Abakus::number_t::PI, NumeralModelItem::Constant, i18n("pi (π): 3.1415926"));
    m_numeralModelItems << new NumeralModelItem("e", Abakus::number_t::E, NumeralModelItem::Constant, i18n("Natural exponential base: 2.7182818"));
    endInsertRows();

    setObjectName ("ValueManager");
}

ValueManager::~ValueManager()
{
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    while(!m_numeralModelItems.isEmpty())
    {
        delete m_numeralModelItems.takeFirst();
    }
    endRemoveRows();
}

int ValueManager::numeralModelItemIndex(const QString& name) const
{
    for(int i = 0; i < m_numeralModelItems.count(); ++i)
    {
        if(m_numeralModelItems[i]->name() == name)
        {
            return i;
        }
    }
    
    return -1;
}

int ValueManager::rowCount(const QModelIndex & parent) const
{
    return m_numeralModelItems.count();
}

QVariant ValueManager::data(const QModelIndex & index, int role) const
{
    if (index.row() < 0 || index.row() > m_numeralModelItems.count())
    {
        return QVariant();
    }
    
    NumeralModelItem *numeralModelItem = m_numeralModelItems[index.row()];
    if (role == NameRole)
    {
        return numeralModelItem->name();
    }
    else if (role == ValueStringRole)
    {
        return numeralModelItem->valueString();
    }
    else if (role == DescriptionRole)
    {
        return numeralModelItem->description();
    }
    else if (role == TypeStringRole)
    {
        return numeralModelItem->typeString();
    }
    return QVariant();
}

Abakus::number_t ValueManager::value(const QString &name) const
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        return m_numeralModelItems[numeralIndex]->value();
    }

    return Abakus::number_t();
}

bool ValueManager::isValueSet(const QString &name) const
{
    return numeralModelItemIndex(name) != -1;
}

bool ValueManager::isValueReadOnly(const QString &name) const
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        return m_numeralModelItems[numeralIndex]->type() != NumeralModelItem::UserVariable;
    }

    return false;
}

void ValueManager::setValue(const QString &name, const Abakus::number_t value)
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1 && m_numeralModelItems[numeralIndex]->value() != value)
    {
        m_numeralModelItems[numeralIndex]->setValue(value);
        emit dataChanged(index(numeralIndex), index(numeralIndex));
    }
    else if(numeralIndex == -1)
    {
        NumeralModelItem* numeralModelItem = new NumeralModelItem(name, value, NumeralModelItem::UserVariable);
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_numeralModelItems << numeralModelItem;
        endInsertRows();
    }
}

void ValueManager::removeValue(const QString &name)
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        beginRemoveRows(QModelIndex(), numeralIndex, numeralIndex);
        delete m_numeralModelItems.takeAt(numeralIndex);
        endRemoveRows();
    }
}

void ValueManager::slotRemoveUserVariables()
{
    QStringList vars = valueNames();

    foreach(QString var, vars)
    {
        if(!isValueReadOnly(var))
        {
            removeValue(var);
        }
    }
}

void ValueManager::slotRedrawItems()
{
    if(rowCount() == 0)
    {
        return; // Don't emit signal if nothing changed.
    }
    
    foreach(NumeralModelItem* numeralModelItem, m_numeralModelItems)
    {
        numeralModelItem->updateNumeral();
    }
    
    emit dataChanged(index(0), index(rowCount() - 1));
}

QStringList ValueManager::valueNames() const
{
    QStringList strList;
    foreach(NumeralModelItem* numeralModelItem, m_numeralModelItems)
    {
        strList << numeralModelItem->name();
    }

    return strList;
}

QString ValueManager::description(const QString &name)
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        return m_numeralModelItems[numeralIndex]->description();
    }

    return QString();
}

#include "valuemanager.moc"

// vim: set et ts=8 sw=4 encoding=utf-8:
