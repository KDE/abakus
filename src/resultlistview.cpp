/*
 * resultlistview.cpp - part of abakus
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
#include <klocale.h>

#include <QtGui/QHeaderView>
#include <QtGui/QMenu>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QClipboard>
#include <QtGui/QApplication>
#include <QtGui/QCursor>
#include <QtGui/QContextMenuEvent>

#include "resultlistview.h"
#include "resultlistviewtext.h"
#include "dragsupport.h"

using DragSupport::makePixmap;
using namespace ResultList;

ResultListView::ResultListView(QWidget *parent) :
    QTreeWidget(parent), m_itemRightClicked(0)
{
    connect(this, SIGNAL(doubleClicked(QTreeWidgetItem *, const QPoint &, int)),
                  SLOT(slotDoubleClicked(QTreeWidgetItem *, const QPoint &, int)));

    QStringList labels;
    labels << i18n("Expression") << i18n("Result") << i18n("Shortcut");
    setHeaderLabels(labels);

    header()->hide(); // I hate that header
    header()->setStretchLastSection(false);
    header()->setResizeMode(ResultColumn, QHeaderView::Stretch);

//    setDragEnabled(true);
//    setItemMargin(2);
//    setColumnAlignment(ResultColumn, AlignLeft);
//    setColumnAlignment(ShortcutColumn, AlignHCenter);
//    setColumnWidthMode(ResultColumn, Maximum);
    setSortingEnabled(false);
}

bool ResultListView::getStackValue(unsigned stackPosition, Abakus::number_t &result)
{
    QTreeWidgetItem *it = topLevelItem(0);
    for(; it; it = itemBelow(it)) {
        ResultListViewText *resultItem = dynamic_cast<ResultListViewText *>(it);
        if(!resultItem->wasError() && resultItem->stackPosition() == stackPosition) {
            QByteArray resultText = resultItem->resultText().toLatin1();
            result = Abakus::number_t(resultText.data());
            return true;
        }
    }

    return false;
}

#if 0
QDragObject *ResultListView::dragObject()
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    ResultListViewText *item = itemUnderCursor();

    if(item) {
        QString text = item->resultText();

        int column = header()->sectionAt(viewportPos.x());

        if(column == ExpressionColumn)
            text = item->expressionText();

        QDragObject *drag = new QTextDrag(text, this);
        drag->setPixmap(makePixmap(text, font()));

        return drag;
    }

    return 0;
}
#endif

void ResultListView::contextMenuEvent(QContextMenuEvent *e)
{
    m_itemRightClicked = itemUnderCursor();
    QMenu *menu = constructPopupMenu(m_itemRightClicked);

    menu->popup(e->globalPos());
}

void ResultListView::slotDoubleClicked(QTreeWidgetItem *item, const QPoint &, int c)
{
    ResultListViewText *textItem = dynamic_cast<ResultListViewText *>(item);
    if(!textItem)
        return;

    if(c == ExpressionColumn)
        emit signalEntrySelected(textItem->expressionText());
    else if(c == ResultColumn)
        emit signalResultSelected(textItem->resultText());
}

QMenu *ResultListView::constructPopupMenu(const ResultListViewText *item)
{
    QMenu *menu = new QMenu(this);

    menu->addAction(i18n("Clear &History"), this, SLOT(clear()), Qt::Key_Alt + Qt::Key_R);

    QAction *a = menu->addAction(i18n("Copy Result to Clipboard"), this, SLOT(slotCopyResult()));
    if(!item || item->wasError())
        a->setEnabled(false);

    return menu;
}

void ResultListView::slotCopyResult()
{
    if(!m_itemRightClicked)
        return;

    QClipboard *clipboard = QApplication::clipboard();

    clipboard->setText(m_itemRightClicked->resultText(), QClipboard::Clipboard);
}

ResultListViewText *ResultListView::lastItem() const
{
    return static_cast<ResultListViewText *>(topLevelItem(topLevelItemCount() - 1));
}

ResultListViewText *ResultListView::itemUnderCursor() const
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    QTreeWidgetItem *underCursor = itemAt(viewportPos);
    return static_cast<ResultListViewText *>(underCursor);
}

#include "resultlistview.moc"

// vim: set et sw=4 ts=8:
