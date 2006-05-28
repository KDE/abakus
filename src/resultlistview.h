#ifndef ABAKUS_RESULTLISTVIEW_H
#define ABAKUS_RESULTLISTVIEW_H
/*
 * resultlistview.h - part of abakus
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

#include <QTreeWidget>

#include "numerictypes.h"

class QContextMenuEvent;
class KMenu;
class QLabel;
class ResultListViewText;

namespace ResultList {
    enum { ExpressionColumn = 0, ResultColumn, ShortcutColumn };
}

class ResultListView : public QTreeWidget
{
    Q_OBJECT

    public:
    ResultListView(QWidget *parent = 0);

    bool getStackValue(unsigned stackPosition, Abakus::number_t &result);

    ResultListViewText *lastItem() const;

    protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void startDrag(Qt::DropActions supportedActions);
    virtual int sizeHintForColumn(int column);

    signals:
    void signalEntrySelected(const QString &text);
    void signalResultSelected(const QString &text);

    private slots:
    void slotDoubleClicked(QTreeWidgetItem *item, int c);
    void slotCopyResult();

    private:
    KMenu *constructPopupMenu(const ResultListViewText *item);
    ResultListViewText *itemUnderCursor() const;

    ResultListViewText *m_itemRightClicked;
};

#endif
