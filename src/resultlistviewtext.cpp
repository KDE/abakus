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

#include <QtCore/QRegExp>
#include <QtGui/QPainter>
#include <QtGui/QFontMetrics>
#include <QtGui/QFont>
#include <QtGui/QPalette>

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
    setText(ExpressionColumn, text);
    setText(ResultColumn, result);
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
        ResultListViewText *item = static_cast<ResultListViewText *>(listView->topLevelItem(0));
        for (; item && item != this; item = static_cast<ResultListViewText *>(listView->itemBelow(item))) {
            if(!item->wasError()) {
                item->setStackPosition(item->stackPosition() + 1);
            }
        }
    }

    setStackPosition(0);

    // Call this manually to be rid of trailing zeroes.
    setText(ExpressionColumn, text);
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

#if 0
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
        f.setPointSize(QMIN(f.pointSize() * 9 / 11, 10));
        p->setFont(f);
    }

    QTreeWidgetItem::paintCell(p, group, column, width, align);
}

int ResultListViewText::width(const QFontMetrics &fm, const QListView *lv, int c) const
{
    // Simulate painting the text to get accurate results.
    if(c == ResultColumn) {
        QFont f = lv->font();
        f.setBold(true);
        return QTreeWidgetItem::width(QFontMetrics(f), lv, c);
    }

    if(c == ShortcutColumn) {
        QFont f = lv->font();
        f.setItalic(true);
        f.setPointSize(QMIN(f.pointSize() * 9 / 11, 10));
        return QTreeWidgetItem::width(QFontMetrics(f), lv, c);
    }

    return QTreeWidgetItem::width(fm, lv, c);
}
#endif

void ResultListViewText::setText(int column, const QString &text)
{
    if(!m_wasError && column == ResultColumn) {
        QTreeWidgetItem::setText(column, m_value.toString());
        return;
    }

    QTreeWidgetItem::setText(column, text);
}

// vim: set et sw=4 ts=8:
