#ifndef ABAKUS_RESULTLISTVIEWTEXT_H
#define ABAKUS_RESULTLISTVIEWTEXT_H
/*
 * resultlistviewtext.h - part of abakus
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

#include "resultlistview.h"
#include "numerictypes.h"

#include <QTreeWidgetItem>

class QPainter;
class QColorGroup;
class QFontMetrics;

// This class shows the results shown in the MainWindow result pane.
class ResultListViewText : public QTreeWidgetItem
{
    public:
    ResultListViewText(QTreeWidget *listView,
                      const QString &text,
		      const QString &result,
		      ResultListViewText *after,
		      bool isError = false);

    ResultListViewText(QTreeWidget *listView,
                      const QString &text,
		      const Abakus::number_t &result,
		      ResultListViewText *after,
		      bool isError = false);

    QString expressionText() const { return m_text; }
    QString resultText() const { return m_result; }

    bool wasError() const { return m_wasError; }
    unsigned stackPosition() const { return m_stackPosition; }

    void setStackPosition(unsigned pos);

    // Redisplays the text by calling value.toString again.
    void precisionChanged();

    // Reimplemented to remove trailing zeroes from results.
    virtual void setText(int column, const QString &text);

    private:
    QString m_text, m_result;
    bool m_wasError;
    unsigned m_stackPosition;
    Abakus::number_t m_value;
};

#endif
