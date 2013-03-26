#ifndef FUNCTIONMODEL_H
#define FUNCTIONMODEL_H
/*
 * functionmodel.h - part of abakus
 * Copyright (C) 2012 Mathias Kraus <k.hias@gmx.net>
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

#include "function.h"
#include "functionmodelitem.h"

#include <QAbstractListModel>

class FunctionModel : public QAbstractListModel
{
    Q_OBJECT
    public:
    enum NumeralRoles {
        NameRole = Qt::UserRole + 1,
        VariableRole,
        ValueRole,
        DescriptionRole,
        TypeStringRole
    };

    static FunctionModel *instance();

    Function *function(const QString &name) const;

    bool isFunction(const QString &name);
    bool isFunctionUserDefined(const QString &name);

    bool addFunction(BaseFunction *fn, const QString &dependantVar);
    bool addFunction(const QString &name, function_t fn, const QString &desc);
    void removeFunction(const QString &name);
    
    void addBuitInFunctions();

    typedef enum { Builtin, UserDefined, All } FunctionType;

    QStringList functionList(FunctionType type);

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;

    private:
    FunctionModel(QObject *parent = 0);
    ~FunctionModel();

    int functionModelItemIndex(const QString &name) const;
    int functionModelItemIndex(const FunctionModelItem::FunctionItemType &type) const;

    static FunctionModel *m_manager;
    QList<FunctionModelItem*> m_functionModelItems;
};

#endif

// vim: set et sw=4 ts=8:
