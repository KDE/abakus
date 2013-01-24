#ifndef ABAKUS_VALUEMANAGER_H
#define ABAKUS_VALUEMANAGER_H
/*
 * valuemanager.h - part of abakus
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

#include "numeralmodelitem.h"
#include "numerictypes.h"

#include <QAbstractListModel>

class NumeralModel : public QAbstractListModel
{
    Q_OBJECT
    public:
    enum NumeralRoles {
        NameRole = Qt::UserRole + 1,
        ValueStringRole,
        DescriptionRole,
        TypeStringRole
    };

    static NumeralModel *instance();

    Abakus::Number value(const QString &name) const;

    bool isValueSet(const QString &name) const;
    bool isValueReadOnly(const QString &name) const;

    void setValue(const QString &name, const Abakus::Number value);
    void removeValue(const QString &name);

    QStringList valueNames() const;

    /**
     * Returns a textual description of a constant built-into abakus.
     */
    QString description(const QString &name);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    public slots:
    void slotRedrawItems();
    void slotRemoveUserVariables();

    private:
    NumeralModel(QObject *parent = 0);
    ~NumeralModel();

    int numeralModelItemIndex(const QString &name) const;
    int numeralModelItemIndex(const NumeralModelItem::NumeralItemType &type) const;

    static NumeralModel *m_manager;
    QList<NumeralModelItem*> m_numeralModelItems;
};

#endif

// vim: set et sw=4 ts=8:
