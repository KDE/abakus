/*
 * numeralmodelitem.cpp - part of abakus
 * Copyright (C) 2012 Mathias Kraus <k.hias@gmx.net>
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

#include "numeralmodelitem.h"

#include <KLocale>

NumeralModelItem::NumeralModelItem(const QString &name, const Abakus::number_t &value, NumeralItemType type, const QString &description)
    : m_name(name), m_value(value), m_valueString(value.toString()), m_description(description), m_type(type)
{
}

QString NumeralModelItem::name() const
{
    return m_name;
}

QString NumeralModelItem::valueString() const
{
    return m_valueString;
}

QString NumeralModelItem::description() const
{
    return m_description;
}

QString NumeralModelItem::typeString() const
{
    switch(m_type)
    {
        case BuiltIn: return i18n("Built-In");
        case UserDefined: return i18n("User Defined");
        default: return QString();
    }
}

void NumeralModelItem::updateNumeral()
{
    m_valueString = m_value.toString();
}

Abakus::number_t NumeralModelItem::value()
{
    return m_value;
}

void NumeralModelItem::setValue(Abakus::number_t value)
{
    m_value = value;
    updateNumeral();
}

NumeralModelItem::NumeralItemType NumeralModelItem::type() const
{
    return m_type;
}
