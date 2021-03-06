#ifndef ABAKUS_RESULTMODEL_H
#define ABAKUS_RESULTMODEL_H
/*
 * resultlistmodel.h - part of abakus
 * Copyright (C) 2012 Mathias Kraus <k.hias@gmx.net>
 * Copyright (C) 2007 Michael Pyne <michael.pyne@kdemail.net>
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

#include "numerictypes.h"
#include "resultmodelitem.h"

#include <QAbstractListModel>

class QString;

class ResultModel : public QAbstractListModel
{
    Q_OBJECT
    public:
    enum ResultRoles {
        ExpressionRole = Qt::UserRole + 1,
        ResultRole,
        TagRole,
        IndexRole
    };

    static ResultModel* instance();

    virtual QHash<int, QByteArray> roleNames() const;
    
    void addResultModelItem(ResultModelItem* item);
    
    void addResult(const QString &expr, const Abakus::Number &result);
    void addMessage(const QString &expr, const QString &result);
    
    QString previousExpression();
    QString nextExpression();

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    bool stackValue(unsigned position, Abakus::Number &result);
    
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    
    QList<ResultModelItem*> resultList();

    public slots:
    void slotRedrawItems(); ///< Called when precision length changes.
    void clear(); ///< Removes all non-header items.

    private:
    void updateStackMarkers(); ///< Called to update stack positions.

    private:
    ResultModel(QObject *parent = 0);
    ~ResultModel();
    
    static ResultModel* m_instance;
    
    QList<ResultModelItem*> m_resultModelItems;
    int m_historyIndex;
};

#endif

// vim: set et sw=4 ts=8:
