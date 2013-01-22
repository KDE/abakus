#ifndef NUMERALMODELITEM_H
#define NUMERALMODELITEM_H
/*
 * numeralmodelitem.h - part of abakus
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

#include "numerictypes.h"

class NumeralModelItem
{
public:
    enum NumeralItemType { BuiltIn, UserDefined};
    
    NumeralModelItem(const QString &name, const Abakus::number_t &value, NumeralItemType type, const QString &description = "");

    QString name() const;
    QString valueString() const;
    QString description() const;
    QString typeString() const;

    void updateNumeral();
    Abakus::number_t value();
    void setValue(Abakus::number_t value);
    NumeralItemType type() const;

private:
    QString m_name;
    Abakus::number_t m_value;
    QString m_valueString;
    QString m_description;
    NumeralItemType m_type;
};

#endif // NUMERALMODELITEM_H
