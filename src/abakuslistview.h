#ifndef ABAKUS_LISTVIEW_H
#define ABAKUS_LISTVIEW_H
/*
 * abakuslistview.h - part of abakus
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

#include <klistview.h>

#include "numerictypes.h"

class ListView : public KListView
{
    public:
    ListView(QWidget *parent, const char *name = 0);

    protected:
    virtual QDragObject *dragObject();
};

class ValueListViewItem : public KListViewItem
{
    public:
    ValueListViewItem (QListView *listView, const QString &name, const Abakus::number_t &value);

    // Will cause the list item to rethink the text.
    void valueChanged();

    void valueChanged(const Abakus::number_t &newValue) { m_value = newValue; valueChanged(); }

    private:
    Abakus::number_t m_value;
};

#endif
