/*
 * resultlistviewtext.cpp - part of abakus
 * Copyright (C) 2004, 2005, 2006 Michael Pyne <michael.pyne@kdemail.net>
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

#include <QRegExp>
#include <QPainter>
#include <QFontMetrics>
#include <QFont>
#include <QPalette>

#include "resultlistviewtext.h"

using namespace ResultList;

ResultListViewText::ResultListViewText(QTreeWidget *listView,
                                     const QString &text,
				     const QString &result,
				     ResultListViewText *after,
				     bool isError)
    : QTreeWidgetItem(listView, after), m_text(text),
      m_result(result), m_wasError(isError), m_stackPosition(0)
{
    // This is some kind of non-result answer, don't bother worrying about the
    // stack status, it hasn't changed.
    setText(ResultColumn, result);
    setText(ExpressionColumn, text);
}

ResultListViewText::ResultListViewText(QTreeWidget *listView,
                                     const QString &text,
				     const Abakus::number_t &result,
				     ResultListViewText *after,
				     bool isError)
    : QTreeWidgetItem(listView, after), m_text(text),
      m_result(result.toString()), m_wasError(isError), m_stackPosition(0),
      m_value(result)
{
    if(after) {
	// We need to adjust the stack position of items in front of us to
	// be higher now.

	for(int i = 0; i < listView->topLevelItemCount(); ++i) {
	    ResultListViewText *item = static_cast<ResultListViewText *>(listView->topLevelItem(i));
	    if(item == this)
		break;

	    if(!item->wasError())
		item->setStackPosition(item->stackPosition() + 1);
	}
    }

    // Setup default text alignment and fonts.

    setTextAlignment(ExpressionColumn, Qt::AlignLeft);

    // XXX: The Qt::red may not provide good contrast with weird color schemes.
    // If so I apologize.
    if(m_wasError)
	setTextColor(ResultColumn, m_result == "OK" ? Qt::blue : Qt::red);

    QFont f = font(ResultColumn);
    f.setBold(true);
    setFont(ResultColumn, f);
    setTextAlignment(ResultColumn, Qt::AlignRight);

    f = font(ShortcutColumn);
    f.setItalic(true);
    f.setPointSize(qMin(f.pointSize() * 9 / 11, 10));
    setFont(ShortcutColumn, f);
    setTextAlignment(ShortcutColumn, Qt::AlignHCenter);

    setStackPosition(0);

    // Call this manually to be rid of trailing zeroes.
    setText(ResultColumn, m_value.toString());
    setText(ExpressionColumn, text);
}

void ResultListViewText::setStackPosition(unsigned pos)
{
    setText(ShortcutColumn, QString("$%1").arg(pos));
    m_stackPosition = pos;
}

void ResultListViewText::precisionChanged()
{
    if(m_wasError)
	return;

    m_result = m_value.toString();
    setText(ResultColumn, m_result);
}

void ResultListViewText::setText(int column, const QString &text)
{
    if(!m_wasError && column == ResultColumn) {
	QTreeWidgetItem::setText(column, m_value.toString());
	return;
    }

    QTreeWidgetItem::setText(column, text);
}
