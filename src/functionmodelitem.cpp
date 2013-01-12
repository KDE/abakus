/*
 * functionmodelitem.cpp - part of abakus
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

#include "functionmodelitem.h"
#include "node.h"

#include <KLocale>

FunctionModelItem::FunctionModelItem(Function* function, FunctionItemType type)
    : m_type(type)
{
    m_function = 0;
    setFunction(function);
}

FunctionModelItem::~FunctionModelItem()
{
    delete m_function;
}

QString FunctionModelItem::name() const
{
    return m_function->name;
}

QString FunctionModelItem::value() const
{
    return m_value;
}

QString FunctionModelItem::description() const
{
    return m_function->description;
}

QString FunctionModelItem::typeString() const
{
    switch(m_type)
    {
        case BuiltInFunction: return i18n("Built-In");
        case UserFunction: return i18n("User Defined");
        default: return QString();
    }
}

Function* FunctionModelItem::function() const
{
    return m_function;
}

void FunctionModelItem::setFunction(Function* function)
{
    delete m_function;
    m_function = function;
    if(m_type == BuiltInFunction)
    {
        m_value = m_function->description;
    }
    else
    {
        m_value = dynamic_cast<UnaryFunction *> (function->userFn->fn)->operand()->infixString();
    }
}

void FunctionModelItem::setValue(const QString &value)
{
    m_value = value;
}

FunctionModelItem::FunctionItemType FunctionModelItem::type() const
{
    return m_type;
}
