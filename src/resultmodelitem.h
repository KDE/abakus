#ifndef RESULTMODELITEM_H
#define RESULTMODELITEM_H
/*
 * resultmodelitem.h - part of abakus
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

#include <QString>

#include "numerictypes.h"

void resetResultModelItemIndex();

class ResultModelItem
{
public:
    enum ResultItemType { Result, Message };
    
    ResultModelItem(const QString &expression, const Abakus::number_t &result);
    ResultModelItem(const QString &message);

    QString expression() const;
    QString result() const;
    QString tag() const;
    int index() const;

    void incrementTag();
    void updateResult();
    Abakus::number_t resultValue();
    ResultItemType type();

private:
    QString m_expression;
    const Abakus::number_t m_resultValue;
    QString m_result;
    int m_tag;
    const int m_index;
    ResultItemType m_type;
};

#endif // RESULTMODELITEM_H
