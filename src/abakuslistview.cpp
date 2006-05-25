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
#include <klocalizedstring.h>
#include <kmenu.h>
#include <kdebug.h>

#include <QContextMenuEvent>
#include <QCursor>
#include <QHeaderView>
#include <QDrag>
#include <QMimeData>
#include <QAction>

#include "dragsupport.h"
#include "abakuslistview.h"
#include "valuemanager.h"
#include "function.h"

ListView::ListView(QWidget *parent) :
    QTreeWidget(parent), m_menu(0), m_usePopup(false), m_removeSingle(0),
    m_removeAll(0)
{
    header()->setResizeMode(QHeaderView::Stretch);
    header()->setStretchLastSection(true);
}

void ListView::startDrag(Qt::DropActions supportedActions)
{
    QPoint viewportPos = viewport()->mapFromGlobal(QCursor::pos());
    QTreeWidgetItem *item = itemAt(viewportPos);

    if(!item)
	return;

    int column = columnAt(viewportPos.x());
    QString dragText = item->text(column);

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;

    mimeData->setText(dragText);
    drag->setMimeData(mimeData);
    drag->setPixmap(DragSupport::makePixmap(dragText, font()));

    drag->start(supportedActions);
}

void ListView::enablePopupHandler(bool enable)
{
    if(enable == m_usePopup)
	return;

    m_usePopup = enable;

    if(m_usePopup) {
	if(m_menu)
	    kError() << "ListView menu shouldn't exist here!\n";

	m_menu = new KMenu(this);

	m_removeSingle = m_menu->addAction(removeItemString(), this, SLOT(removeSelected()));
	m_removeAll = m_menu->addAction("Placeholder", this, SLOT(removeAllItems()));
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

QString ListView::removeAllItemsString(unsigned) const
{
    return QString();
}

void ListView::removeSelectedItem(QTreeWidgetItem *)
{
}

void ListView::removeAllItems()
{
}

bool ListView::isItemRemovable(QTreeWidgetItem *) const
{
    return false;
}

void ListView::contextMenuEvent(QContextMenuEvent *e)
{
    if(!m_usePopup) {
	QTreeWidget::contextMenuEvent(e);
	return;
    }

    QTreeWidgetItem *item = itemAt(e->pos());

    m_removeSingle->setEnabled(item && isItemRemovable(item));
    m_removeAll->setText(removeAllItemsString(topLevelItemCount()));

    m_menu->popup(e->globalPos());
}

void ListView::removeSelected()
{
    QList<QTreeWidgetItem *> items = selectedItems();

    foreach(QTreeWidgetItem *item, items)
	removeSelectedItem(item); // Don't delete here, MainWindow does it.
}

ValueListViewItem::ValueListViewItem(
	QTreeWidget *treeWidget,
	const QString &name,
	const Abakus::number_t &value)
    : QTreeWidgetItem(treeWidget, QStringList(name)), m_value(value)
{
    updateText();
}

void ValueListViewItem::updateText()
{
    setText(1, m_value.toString());
}

void ValueListViewItem::valueChanged(const Abakus::number_t &newValue)
{
    m_value = newValue;
    updateText();
}

Abakus::number_t ValueListViewItem::itemValue() const
{
    return m_value;
}

VariableListView::VariableListView(QWidget *parent) :
    ListView(parent)
{
    enablePopupHandler(true);

    QStringList labels;

    labels << i18n("Variables") << i18nc("Value of a variable", "Value");
    setHeaderLabels(labels);
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

    return i18np("Remove all variables (1 variable)",
	        "Remove all variables (%n variables)",
		count);
}

bool VariableListView::isItemRemovable(QTreeWidgetItem *item) const
{
    return !ValueManager::instance()->isValueReadOnly(item->text(0));
}

void VariableListView::removeSelectedItem(QTreeWidgetItem *item)
{
    ValueManager::instance()->removeValue(item->text(0));
}

void VariableListView::removeAllItems()
{
    ValueManager::instance()->slotRemoveUserVariables();
}

FunctionListView::FunctionListView(QWidget *parent) :
    ListView(parent)
{
    enablePopupHandler(true);

    QStringList labels;

    labels << i18n("Function Name") << i18nc("Function Definition (e.g. sin(x))", "Definition");
    setHeaderLabels(labels);
}

QString FunctionListView::removeItemString() const
{
    return i18n("Remove selected function");
}

QString FunctionListView::removeAllItemsString(unsigned count) const
{
    return i18np("Remove all functions (1 function)",
	        "Remove all functions (%n functions)",
		count);
}

bool FunctionListView::isItemRemovable(QTreeWidgetItem *) const
{
    return true;
}

void FunctionListView::removeSelectedItem(QTreeWidgetItem *item)
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
