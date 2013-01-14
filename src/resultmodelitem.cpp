/*
 * resultmodelitem.cpp - part of abakus
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

#include "resultmodelitem.h"

static int globalIndex = 0;

void resetResultModelItemIndex()
{
    globalIndex = 0;
}

ResultModelItem::ResultModelItem(const QString &expression, const Abakus::number_t &result)
    : m_expression(expression), m_resultValue(result), m_result(result.toString()), m_tag(0), m_index(globalIndex++), m_type(Result)
{
}

ResultModelItem::ResultModelItem(const QString &expression, const QString &result)
: m_expression(expression), m_resultValue(0), m_result(result), m_tag(0), m_index(globalIndex++), m_type(Message)
{
}

QString ResultModelItem::expression() const
{
    return m_expression;
}

QString ResultModelItem::result() const
{
    return m_result;
}

QString ResultModelItem::tag() const
{
    return m_type == Result ? QString("$%1").arg(m_tag) : "";
}

int ResultModelItem::index() const
{
    return m_index;
}

void ResultModelItem::incrementTag()
{
    if(m_type == Result)
    {
        ++m_tag;
    }
}

void ResultModelItem::updateResult()
{
    m_result = resultValue().toString();
}

Abakus::number_t ResultModelItem::resultValue()
{
    return m_resultValue;
}

ResultModelItem::ResultItemType ResultModelItem::type()
{
    return m_type;
}
