#ifndef ABAKUS_RESULTMODEL_H
#define ABAKUS_RESULTMODEL_H
/*
 * resultlistmodel.h - part of abakus
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

#include <QtCore/QObject>

#include "numerictypes.h"

class QStandardItemModel;
class QString;
class QModelIndex;

class ResultModel : public QObject
{
    Q_OBJECT
    public:

    ResultModel(QObject *parent);

    QStandardItemModel *model();
    void addResult(const QString &expr, const Abakus::number_t &result);
    void addMessage(const QString &msg);

    bool stackValue(unsigned position, Abakus::number_t &result);

    signals:
    /// Only emitted when the result column changes in width due to the user
    /// selecting a different precision.
    void signalColumnChanged(int);

    public slots:
    void slotRedrawItems(); ///< Called when precision length changes.
    void clear(); ///< Removes all non-header items.

    private:
    void updateStackMarkers(); ///< Called to update stack positions.

    private:
    QStandardItemModel *m_model;
};

#endif

// vim: set et sw=4 ts=8:
