#ifndef ABAKUS_LISTVIEW_H
#define ABAKUS_LISTVIEW_H
/*
 * abakuslistview.h - part of abakus
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

#include <QtGui/QTreeWidget>

#include "numerictypes.h"

class QMenu;
class QAction;

class ListView : public QTreeWidget
{
    Q_OBJECT

    public:
    ListView(QWidget *parent);

    public slots:
    /**
     * Causes all items to be redrawn.  Should be used for when the precision of a value
     * changes for example, or other actions that would cause the display of all items
     * to become inaccurate.
     */
    void redrawItems();

    protected:
//    virtual QDragObject *dragObject();

    /**
     * Used to enable fancy popup handling support in subclasses.  Subclasses
     * also need to reimplement a few functions if they want to use this.
     *
     * This should be called in the subclass's constructor.
     */
    void enablePopupHandler(bool enable);

    /**
     * If using the popup menu handling, the subclass needs to return a
     * translated string of the form "Remove selected <itemtype>".
     */
    virtual QString removeItemString() const;

    /**
     * If using the popup menu handling, the subclass needs to return a
     * translated string of the form "Remove all <itemtype>s."  I recommend
     * also appending a " (%n <itemtype>s), which you can use the @p count
     * parameter for.
     */
    virtual QString removeAllItemsString(unsigned count) const;

    protected slots:
    /**
     * If using the popup menu handing, the subclass needs to reimplement this
     * function to remove the selected item, which is passed in as a
     * parameter.
     */
    virtual void removeSelectedItem(QTreeWidgetItem *item);

    /**
     * If using the popup menu handling, the subclass needs to reimplement this
     * function to remove all items.
     */
    virtual void removeAllItems();

    /**
     * If using the popup menu handling, this function may be called to
     * determine whether the selected item given by @p item is removable.
     */
    virtual bool isItemRemovable(QTreeWidgetItem *item) const;

    private slots:
    void slotItemClicked(QTreeWidgetItem *item);
    void removeSelected();

    private:
    QMenu *m_menu;
    bool m_usePopup;

    QAction *m_removeSingleId;
    QAction *m_removeAllId;
};

class ValueTreeWidgetItem : public QTreeWidgetItem
{
    public:
    ValueTreeWidgetItem (QTreeWidget *listView, const QString &name, const Abakus::number_t &value);

    // Will cause the list item to rethink the text.
    void valueChanged();
    void valueChanged(const Abakus::number_t &newValue);

    Abakus::number_t itemValue() const;

    private:
    Abakus::number_t m_value;
};

/**
 * Subclass used for the list of variables.
 */
class VariableListView : public ListView
{
    Q_OBJECT
    public:

    VariableListView(QWidget *parent);

    protected:
    virtual QString removeItemString() const;
    virtual QString removeAllItemsString(unsigned count) const;
    virtual bool isItemRemovable(QTreeWidgetItem *item) const;

    protected slots:
    virtual void removeSelectedItem(QTreeWidgetItem *item);
    virtual void removeAllItems();
};

/**
 * Subclass used for the list of functions.
 */
class FunctionListView : public ListView
{
    Q_OBJECT
    public:

    FunctionListView(QWidget *parent);

    protected:
    virtual QString removeItemString() const;
    virtual QString removeAllItemsString(unsigned count) const;
    virtual bool isItemRemovable(QTreeWidgetItem *item) const;

    protected slots:
    virtual void removeSelectedItem(QTreeWidgetItem *item);
    virtual void removeAllItems();
};

#endif
