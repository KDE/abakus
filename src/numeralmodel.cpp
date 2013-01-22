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

#include "numeralmodel.h"

#include <KLocale>

NumeralModel *NumeralModel::m_manager = 0;

NumeralModel *NumeralModel::instance()
{
    if(!m_manager)
        m_manager = new NumeralModel;

    return m_manager;
}

NumeralModel::NumeralModel(QObject *parent) : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ValueStringRole] = "valueString";
    roles[DescriptionRole] = "description";
    roles[TypeStringRole] = "typeString";
    setRoleNames(roles);

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_numeralModelItems << new NumeralModelItem("ans", Abakus::number_t(42), NumeralModelItem::BuiltIn);
    m_numeralModelItems << new NumeralModelItem("pi", Abakus::number_t::PI, NumeralModelItem::BuiltIn, i18n("pi (π): 3.1415926"));
    m_numeralModelItems << new NumeralModelItem("e", Abakus::number_t::E, NumeralModelItem::BuiltIn, i18n("Natural exponential base: 2.7182818"));
    endInsertRows();

    setObjectName ("ValueManager");
}

NumeralModel::~NumeralModel()
{
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    while(!m_numeralModelItems.isEmpty())
    {
        delete m_numeralModelItems.takeFirst();
    }
    endRemoveRows();
}

int NumeralModel::numeralModelItemIndex(const QString& name) const
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

int NumeralModel::numeralModelItemIndex(const NumeralModelItem::NumeralItemType& type) const
{
    for(int i = 0; i < m_numeralModelItems.count(); ++i)
    {
        if(m_numeralModelItems[i]->type() == type)
        {
            return i;
        }
    }
    
    return -1;
}

int NumeralModel::rowCount(const QModelIndex & parent) const
{
    return m_numeralModelItems.count();
}

QVariant NumeralModel::data(const QModelIndex & index, int role) const
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

Abakus::number_t NumeralModel::value(const QString &name) const
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        return m_numeralModelItems[numeralIndex]->value();
    }

    return Abakus::number_t();
}

bool NumeralModel::isValueSet(const QString &name) const
{
    return numeralModelItemIndex(name) != -1;
}

bool NumeralModel::isValueReadOnly(const QString &name) const
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        return m_numeralModelItems[numeralIndex]->type() != NumeralModelItem::UserDefined;
    }

    return false;
}

void NumeralModel::setValue(const QString &name, const Abakus::number_t value)
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1 && m_numeralModelItems[numeralIndex]->value() != value)
    {
        m_numeralModelItems[numeralIndex]->setValue(value);
        emit dataChanged(index(numeralIndex), index(numeralIndex));
    }
    else if(numeralIndex == -1)
    {
        NumeralModelItem* numeralModelItem = new NumeralModelItem(name, value, NumeralModelItem::UserDefined);
        int numeralIndexOfFirstBuildInNumeral = numeralModelItemIndex(NumeralModelItem::BuiltIn);
        if(numeralIndexOfFirstBuildInNumeral == -1)
        {
            numeralIndexOfFirstBuildInNumeral = rowCount();
        }
        
        beginInsertRows(QModelIndex(), numeralIndexOfFirstBuildInNumeral, numeralIndexOfFirstBuildInNumeral);
        m_numeralModelItems.insert(numeralIndexOfFirstBuildInNumeral, numeralModelItem);
        endInsertRows();
    }
}

void NumeralModel::removeValue(const QString &name)
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        beginRemoveRows(QModelIndex(), numeralIndex, numeralIndex);
        delete m_numeralModelItems.takeAt(numeralIndex);
        endRemoveRows();
    }
}

void NumeralModel::slotRemoveUserVariables()
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

void NumeralModel::slotRedrawItems()
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

QStringList NumeralModel::valueNames() const
{
    QStringList strList;
    foreach(NumeralModelItem* numeralModelItem, m_numeralModelItems)
    {
        strList << numeralModelItem->name();
    }

    return strList;
}

QString NumeralModel::description(const QString &name)
{
    int numeralIndex = numeralModelItemIndex(name);
    if(numeralIndex != -1)
    {
        return m_numeralModelItems[numeralIndex]->description();
    }

    return QString();
}

#include "numeralmodel.moc"

// vim: set et ts=8 sw=4 encoding=utf-8:
