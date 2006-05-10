/*
 * resultlistviewtext.cpp - part of abakus
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

#include <qregexp.h>
#include <qpainter.h>
#include <qfontmetrics.h>
#include <qfont.h>
#include <qpalette.h>
#include <kvbox.h>

#include "resultlistviewtext.h"

using namespace ResultList;

ResultListViewText::ResultListViewText(K3ListView *listView,
                                     const QString &text,
				     const QString &result,
				     ResultListViewText *after,
				     bool isError)
    : K3ListViewItem(listView, after, text, result), m_text(text),
      m_result(result), m_wasError(isError), m_stackPosition(0)
{
    // This is some kind of non-result answer, don't bother worrying about the
    // stack status, it hasn't changed.
}

ResultListViewText::ResultListViewText(K3ListView *listView,
                                     const QString &text,
				     const Abakus::number_t &result,
				     ResultListViewText *after,
				     bool isError)
    : K3ListViewItem(listView, after, text), m_text(text),
      m_result(result.toString()), m_wasError(isError), m_stackPosition(0),
      m_value(result)
{
    if(after) {
	ResultListViewText *item = static_cast<ResultListViewText *>(listView->firstChild());
	for (; item && item != this; item = static_cast<ResultListViewText *>(item->itemBelow())) {
	    if(!item->wasError()) {
		item->setStackPosition(item->stackPosition() + 1);
		item->repaint();
	    }
	}
    }

    setStackPosition(0);

    // Call this manually to be rid of trailing zeroes.
    setText(ResultColumn, m_value.toString());
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

void ResultListViewText::paintCell(QPainter *p, const QColorGroup &cg, int column, int width, int align)
{
    QColorGroup group(cg);

    // XXX: The Qt::red may not provide good contrast with weird color schemes.
    // If so I apologize.
    if(m_wasError && column == ResultColumn)
	group.setColor(QColorGroup::Text, m_result == "OK" ? cg.link() : Qt::red);

    if(column == ResultColumn) {
	QFont f = p->font();
	f.setBold(true);
	p->setFont(f);
    }

    if(column == ShortcutColumn) {
	QFont f = p->font();
	f.setItalic(true);
	f.setPointSize(qMin(f.pointSize() * 9 / 11, 10));
	p->setFont(f);
    }

    K3ListViewItem::paintCell(p, group, column, width, align);
}

int ResultListViewText::width(const QFontMetrics &fm, const Q3ListView *lv, int c) const
{
    // Simulate painting the text to get accurate results.
    if(c == ResultColumn) {
	QFont f = lv->font();
	f.setBold(true);
	return K3ListViewItem::width(QFontMetrics(f), lv, c);
    }

    if(c == ShortcutColumn) {
	QFont f = lv->font();
	f.setItalic(true);
	f.setPointSize(qMin(f.pointSize() * 9 / 11, 10));
	return K3ListViewItem::width(QFontMetrics(f), lv, c);
    }

    return K3ListViewItem::width(fm, lv, c);
}

void ResultListViewText::setText(int column, const QString &text)
{
    if(!m_wasError && column == ResultColumn) {
	K3ListViewItem::setText(column, m_value.toString());
	return;
    }

    K3ListViewItem::setText(column, text);
}
