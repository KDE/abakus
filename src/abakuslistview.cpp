/*
 * abakuslistview.cpp - part of abakus
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
#include <qdragobject.h>
#include <qcursor.h>
#include <qheader.h>

#include "dragsupport.h"
#include "abakuslistview.h"

ListView::ListView(QWidget *parent, const char *name) :
    KListView(parent, name)
{
    setResizeMode(LastColumn);
    setDragEnabled(true);
}

QDragObject *ListView::dragObject()
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    QListViewItem *item = itemAt(viewportPos);

    if(!item)
	return 0;

    int column = header()->sectionAt(viewportPos.x());
    QString dragText = item->text(column);

    QDragObject *drag = new QTextDrag(dragText, this, "list item drag");
    drag->setPixmap(DragSupport::makePixmap(dragText, font()));

    return drag;
}

ValueListViewItem::ValueListViewItem(QListView *listView, const QString &name,
	const Abakus::number_t &value) :
    KListViewItem(listView, name), m_value(value)
{
    valueChanged();
}

void ValueListViewItem::valueChanged()
{
    setText(1, m_value.toString());
    repaint();
}
