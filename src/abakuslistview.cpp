/*
 * abakuslistview.cpp - part of abakus
 * Copyright (C) 2004, 2005, 2007 Michael Pyne <michael.pyne@kdemail.net>
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
#include <kdebug.h>

#include <QtGui/QCursor>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidgetItemIterator>
#include <QtCore/QList>
//#include <qdragobject.h>
//#include <qheader.h>

#include "dragsupport.h"
#include "abakuslistview.h"
#include "valuemanager.h"
#include "function.h"

ListView::ListView(QWidget *parent) :
    QTreeWidget(parent), m_menu(0), m_usePopup(false), m_removeSingleId(0),
    m_removeAllId(0)
{
//    setResizeMode(LastColumn);
//    setDragEnabled(true);

    connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
                  SLOT(slotItemClicked(QTreeWidgetItem *)));
}

void ListView::redrawItems()
{
    QTreeWidgetItemIterator it(this);

    while(*it) {
        ValueTreeWidgetItem *item = dynamic_cast<ValueTreeWidgetItem *>(*it);
        ++it;

        if(!item)
            continue;

        item->valueChanged();
    }
}

#if 0
QDragObject *ListView::dragObject()
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    QTreeWidgetItem *item = itemAt(viewportPos);

    if(!item)
        return 0;

    int column = header()->sectionAt(viewportPos.x());
    QString dragText = item->text(column);

    QDragObject *drag = new QTextDrag(dragText, this, "list item drag");
    drag->setPixmap(DragSupport::makePixmap(dragText, font()));

    return drag;
}
#endif

void ListView::enablePopupHandler(bool enable)
{
    if(enable == m_usePopup)
        return;

    m_usePopup = enable;

    if(m_usePopup) {
        if(m_menu)
            kError() << "ListView menu shouldn't exist here!\n";

        m_menu = new QMenu(this);

        m_removeSingleId = m_menu->addAction(removeItemString(), this, SLOT(removeSelected()));
        m_removeAllId = m_menu->addAction("Placeholder", this, SLOT(removeAllItems()));
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

void ListView::removeSelectedItem(QTreeWidgetItem *item)
{
    Q_UNUSED(item);
}

void ListView::removeAllItems()
{
}

bool ListView::isItemRemovable(QTreeWidgetItem *item) const
{
    Q_UNUSED(item);

    return false;
}

void ListView::slotItemClicked(QTreeWidgetItem *item)
{
    if(!m_usePopup)
        return;

    QPoint pt = QCursor::pos();

    m_removeSingleId->setEnabled(isItemRemovable(item));
    m_removeAllId->setText(removeAllItemsString(topLevelItemCount()));

    m_menu->popup(pt);
}

void ListView::removeSelected()
{
    QList<QTreeWidgetItem *> items = selectedItems();

    // We should only be able to select one.
    if(!items.isEmpty())
        removeSelectedItem(items.at(0));
}

ValueTreeWidgetItem::ValueTreeWidgetItem(
    QTreeWidget *listView, const QString &name, const Abakus::number_t &value
)
: QTreeWidgetItem(listView, QStringList(name)), m_value(value)
{
    valueChanged();
}

void ValueTreeWidgetItem::valueChanged()
{
    setText(1, m_value.toString());
}

void ValueTreeWidgetItem::valueChanged(const Abakus::number_t &newValue)
{
    m_value = newValue;

    valueChanged();
}

Abakus::number_t ValueTreeWidgetItem::itemValue() const
{
    return m_value;
}

FunctionListView::FunctionListView(QWidget *parent) : ListView(parent)
{
    enablePopupHandler(true);
}

QString FunctionListView::removeItemString() const
{
    return i18n("Remove selected function");
}

QString FunctionListView::removeAllItemsString(unsigned count) const
{
    return i18np("Remove all functions (1 function)",
                "Remove all functions (%1 functions)",
                count);
}

bool FunctionListView::isItemRemovable(QTreeWidgetItem *item) const
{
    // Hurry up, null_ptr. :)
    return item != static_cast<QTreeWidgetItem *>(0);
}

void FunctionListView::removeSelectedItem(QTreeWidgetItem *item)
{
    if(!item)
        return;

    // Use section to get the beginning of the string up to (and not
    // including) the first (
    QString name = item->text(0).section('(', 0, 0);

    FunctionManager::instance()->removeFunction(name);
}

void FunctionListView::removeAllItems()
{
    QStringList fns = FunctionManager::instance()->functionList(FunctionManager::UserDefined);

    foreach(QString fn, fns)
       FunctionManager::instance()->removeFunction(fn);
}

#include "abakuslistview.moc"

// vim: set et sw=4 ts=8:
