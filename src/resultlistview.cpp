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
#include <kmenu.h>
#include <klocalizedstring.h>

#include <QAction>
#include <QClipboard>
#include <QApplication>
#include <QCursor>
#include <QHeaderView>
#include <QContextMenuEvent>

#include "resultlistview.h"
#include "resultlistviewtext.h"
#include "dragsupport.h"

using DragSupport::makePixmap;
using namespace ResultList;

ResultListView::ResultListView(QWidget *parent) :
    QTreeWidget(parent), m_itemRightClicked(0)
{
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
                  SLOT(slotDoubleClicked(QTreeWidgetItem *, int)));

    QStringList columns;

    columns << i18nc("not visible, math expression", "Expression")
	    << i18nc("not visible, math result", "Result")
	    << i18nc("not visible, shortcut to previous answer, not keyboard shortcut", "Shortcut");

    setHeaderLabels(columns);

    header()->hide(); // I hate that header
    header()->setResizeMode(ResultColumn, QHeaderView::Stretch);
    header()->setResizeMode(ExpressionColumn, QHeaderView::Stretch);
    header()->setResizeMode(ShortcutColumn, QHeaderView::Custom);
    header()->resizeSection(ShortcutColumn, sizeHintForColumn(ShortcutColumn));

    setDragEnabled(true);
    setObjectName("resultListView");
    setSortingEnabled(false);
}

bool ResultListView::getStackValue(unsigned stackPosition, Abakus::number_t &result)
{
    QTreeWidgetItemIterator it(this);

    for(; *it; ++it) {
	ResultListViewText *resultItem = static_cast<ResultListViewText *>(*it);
	if(!resultItem->wasError() && resultItem->stackPosition() == stackPosition) {

	    // TODO: ResultItem should have the number value, not just the text.
	    result = Abakus::number_t(resultItem->resultText());
	    return true;
	}
    }

    return false;
}

void ResultListView::startDrag(Qt::DropActions supportedActions)
{
    ResultListViewText *item = itemUnderCursor();
    if(!item)
	return;

    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    int column = columnAt(viewportPos.x());
    QString text = item->resultText();

    if(column == ExpressionColumn)
	text = item->expressionText();

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText(text);
    drag->setMimeData(mimeData);
    drag->setPixmap(makePixmap(text, font()));

    drag->start(supportedActions);
}

int ResultListView::sizeHintForColumn(int column)
{
    if(column == ShortcutColumn) {
	QFont f = font();
	f.setItalic(true);
	f.setPointSize(qMin(f.pointSize() * 9 / 11, 10));

	QFontMetrics fm(f);
	return fm.width("  $99  ");
    }
    else
	return QTreeWidget::sizeHintForColumn(column);
}

void ResultListView::contextMenuEvent(QContextMenuEvent *e)
{
    m_itemRightClicked = itemUnderCursor();
    KMenu *menu = constructPopupMenu(m_itemRightClicked);

    menu->popup(e->globalPos());
    delete menu;
}

void ResultListView::slotDoubleClicked(QTreeWidgetItem *item, int c)
{
    ResultListViewText *textItem = dynamic_cast<ResultListViewText *>(item);
    if(!textItem)
	return;

    if(c == ExpressionColumn)
	emit signalEntrySelected(textItem->expressionText());
    else if(c == ResultColumn)
	emit signalResultSelected(textItem->resultText());
}

KMenu *ResultListView::constructPopupMenu(const ResultListViewText *item)
{
    KMenu *menu = new KMenu(this);

    menu->addAction(i18n("Clear &History"), this, SLOT(clear()), Qt::ALT+Qt::Key_R);
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
    QTreeWidgetItem *item = topLevelItem(topLevelItemCount() - 1);
    return static_cast<ResultListViewText *>(item);
}

ResultListViewText *ResultListView::itemUnderCursor() const
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    QTreeWidgetItem *underCursor = itemAt(viewportPos);
    return static_cast<ResultListViewText *>(underCursor);
}

#include "resultlistview.moc"
