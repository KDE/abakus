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
#include <kpopupmenu.h>
#include <klocale.h>

#include <qclipboard.h>
#include <qapplication.h>
#include <qevent.h>
#include <qcursor.h>
#include <qdragobject.h>
#include <qheader.h>

#include "resultlistview.h"
#include "resultlistviewtext.h"
#include "dragsupport.h"

using DragSupport::makePixmap;
using namespace ResultList;

ResultListView::ResultListView(QWidget *parent, const char *name) :
    KListView(parent, name), m_itemRightClicked(0)
{
    connect(this, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
                  SLOT(slotDoubleClicked(QListViewItem *, const QPoint &, int)));

    addColumn(i18n("Expression"));
    addColumn(i18n("Result"));
    addColumn(i18n("Shortcut"));

    header()->hide(); // I hate that header
    header()->setStretchEnabled(ResultColumn, true);

    setDragEnabled(true);
    setItemMargin(2);
    setColumnAlignment(ResultColumn, AlignLeft);
    setColumnAlignment(ShortcutColumn, AlignHCenter);
    setColumnWidthMode(ResultColumn, Maximum);
    setSortColumn(-1);
}

bool ResultListView::getStackValue(unsigned stackPosition, Abakus::number_t &result)
{
    QListViewItem *it = firstChild();
    for(; it; it = it->itemBelow()) {
	ResultListViewText *resultItem = dynamic_cast<ResultListViewText *>(it);
	if(!resultItem->wasError() && resultItem->stackPosition() == stackPosition) {
	    result = Abakus::number_t(resultItem->resultText().latin1());
	    return true;
	}
    }

    return false;
}

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

void ResultListView::contextMenuEvent(QContextMenuEvent *e)
{
    m_itemRightClicked = itemUnderCursor();
    KPopupMenu *menu = constructPopupMenu(m_itemRightClicked);

    menu->popup(e->globalPos());
}

void ResultListView::slotDoubleClicked(QListViewItem *item, const QPoint &, int c)
{
    ResultListViewText *textItem = dynamic_cast<ResultListViewText *>(item);
    if(!textItem)
	return;

    if(c == ExpressionColumn)
	emit signalEntrySelected(textItem->expressionText());
    else if(c == ResultColumn)
	emit signalResultSelected(textItem->resultText());
}

KPopupMenu *ResultListView::constructPopupMenu(const ResultListViewText *item)
{
    KPopupMenu *menu = new KPopupMenu(this, "list view context menu");

    menu->insertItem(i18n("Clear &History"), this, SLOT(clear()), ALT+Key_R);

    int id = menu->insertItem(i18n("Copy Result to Clipboard"), this, SLOT(slotCopyResult()));
    if(!item || item->wasError())
	menu->setItemEnabled(id, false);

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
    return static_cast<ResultListViewText *>(KListView::lastItem());
}

ResultListViewText *ResultListView::itemUnderCursor() const
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    QListViewItem *underCursor = itemAt(viewportPos);
    return static_cast<ResultListViewText *>(underCursor);
}

#include "resultlistview.moc"
