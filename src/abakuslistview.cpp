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
#include <klocale.h>
#include <kpopupmenu.h>
#include <kdebug.h>

#include <q3dragobject.h>
#include <qcursor.h>
#include <q3header.h>

#include "dragsupport.h"
#include "abakuslistview.h"
#include "valuemanager.h"
#include "function.h"

ListView::ListView(QWidget *parent, const char *name) :
    KListView(parent, name), m_menu(0), m_usePopup(false), m_removeSingleId(0),
    m_removeAllId(0)
{
    setResizeMode(LastColumn);
    setDragEnabled(true);

    connect(this, SIGNAL(contextMenuRequested(Q3ListViewItem *, const QPoint &, int)),
	          SLOT(rightClicked(Q3ListViewItem *, const QPoint &)));
}

Q3DragObject *ListView::dragObject()
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    Q3ListViewItem *item = itemAt(viewportPos);

    if(!item)
	return 0;

    int column = header()->sectionAt(viewportPos.x());
    QString dragText = item->text(column);

    Q3DragObject *drag = new Q3TextDrag(dragText, this, "list item drag");
    drag->setPixmap(DragSupport::makePixmap(dragText, font()));

    return drag;
}

void ListView::enablePopupHandler(bool enable)
{
    if(enable == m_usePopup)
	return;

    m_usePopup = enable;

    if(m_usePopup) {
	if(m_menu)
	    kdError() << "ListView menu shouldn't exist here!\n";

	m_menu = new KPopupMenu(this);

	m_removeSingleId = m_menu->insertItem(removeItemString(), this, SLOT(removeSelected()));
	m_removeAllId = m_menu->insertItem("Placeholder", this, SLOT(removeAllItems()));
    }
    else {
	delete m_menu;
	m_menu = 0;
    }
}

QString ListView::removeItemString() const
{
    return QString();
}

QString ListView::removeAllItemsString(unsigned count) const
{
    Q_UNUSED(count);

    return QString();
}

void ListView::removeSelectedItem(Q3ListViewItem *item)
{
    Q_UNUSED(item);
}

void ListView::removeAllItems()
{
}

bool ListView::isItemRemovable(Q3ListViewItem *item) const
{
    Q_UNUSED(item);

    return false;
}

void ListView::rightClicked(Q3ListViewItem *item, const QPoint &pt)
{
    if(!m_usePopup)
	return;

    m_menu->setItemEnabled(m_removeSingleId, item && isItemRemovable(item));
    m_menu->changeItem(m_removeAllId, removeAllItemsString(childCount()));
    m_menu->popup(pt);
}

void ListView::removeSelected()
{
    removeSelectedItem(selectedItem());
}

ValueListViewItem::ValueListViewItem(Q3ListView *listView, const QString &name,
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

void ValueListViewItem::valueChanged(const Abakus::number_t &newValue)
{
    m_value = newValue;

    valueChanged();
}

Abakus::number_t ValueListViewItem::itemValue() const
{
    return m_value;
}

VariableListView::VariableListView(QWidget *parent, const char *name) :
    ListView(parent, name)
{
    enablePopupHandler(true);
}

QString VariableListView::removeItemString() const
{
    return i18n("Remove selected variable");
}

QString VariableListView::removeAllItemsString(unsigned count) const
{
    // count is unreliable because not all of the elements in the list view
    // can be removed.
    count = 0;
    QStringList values = ValueManager::instance()->valueNames();

    for(QStringList::ConstIterator value = values.constBegin(); value != values.constEnd(); ++value)
	if(!ValueManager::instance()->isValueReadOnly(*value))
	    ++count;

    return i18n("Remove all variables (1 variable)",
	        "Remove all variables (%n variables)",
		count);
}

bool VariableListView::isItemRemovable(Q3ListViewItem *item) const
{
    return !ValueManager::instance()->isValueReadOnly(item->text(0));
}

void VariableListView::removeSelectedItem(Q3ListViewItem *item)
{
    ValueManager::instance()->removeValue(item->text(0));
}

void VariableListView::removeAllItems()
{
    ValueManager::instance()->slotRemoveUserVariables();
}

FunctionListView::FunctionListView(QWidget *parent, const char *name) :
    ListView(parent, name)
{
    enablePopupHandler(true);
}

QString FunctionListView::removeItemString() const
{
    return i18n("Remove selected function");
}

QString FunctionListView::removeAllItemsString(unsigned count) const
{
    return i18n("Remove all functions (1 function)",
	        "Remove all functions (%n functions)",
		count);
}

bool FunctionListView::isItemRemovable(Q3ListViewItem *item) const
{
    return true;
}

void FunctionListView::removeSelectedItem(Q3ListViewItem *item)
{
    // Use section to get the beginning of the string up to (and not
    // including) the first (
    QString name = item->text(0).section('(', 0, 0);

    FunctionManager::instance()->removeFunction(name);
}

void FunctionListView::removeAllItems()
{
    QStringList fns = FunctionManager::instance()->functionList(FunctionManager::UserDefined);

    for(QStringList::ConstIterator fn = fns.constBegin(); fn != fns.constEnd(); ++fn)
       FunctionManager::instance()->removeFunction(*fn);
}

#include "abakuslistview.moc"
