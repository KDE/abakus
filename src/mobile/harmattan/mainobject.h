#ifndef ABAKUS_MAINOBJECT_H
#define ABAKUS_MAINOBJECT_H
/*
 * mainwindow.h - part of abakus
 * Copyright (C) 2012 Mathias Kraus <k.hias@gmx.net>
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

#include "numerictypes.h"
#include "settingscore.h"

#include <QObject>

class ResultModel;

class QDeclarativeContext;
class QDeclarativeView;
class QMenu;

// Main window class, handles events and stuff
class MainObject : public QObject
{
    Q_OBJECT
    
public:
    MainObject();
    ~MainObject();

    void setView(QDeclarativeView* declarativeView);

protected:
    Q_INVOKABLE void slotEvaluate(const QString &expression);
    Q_INVOKABLE void slotTextChanged(const QString &str);
    Q_INVOKABLE QString getTag(const int &index);
    
    Q_INVOKABLE void clearHistory();
    
    Q_INVOKABLE void historyPrevious();
    Q_INVOKABLE void historyNext();
    
    Q_INVOKABLE void removeNumeral(const QString &name);
    Q_INVOKABLE void removeFunction(const QString &name);

    Q_INVOKABLE int getVisibleHistoryItemIndex(int listIndex);
    Q_INVOKABLE void addVisibleHistoryItemIndex(int itemIndex);
    Q_INVOKABLE void removeVisibleHistoryItemIndex(int itemIndex);
    
signals:
    void setEditorText(const QString &editorText);
    
private slots:
    void slotRedrawResults();

private:
    int getParenthesesLevel(const QString &str);
    QString interpolateExpression(const QString &text);

    ResultModel *m_resultItemModel;
    
    QDeclarativeContext *m_declarativeContext;

    SettingsCore* m_settingscore;

    bool m_insert;

    QList<int> m_visibleHistoryItemIndices;
};

#endif

// vim: set et ts=8 sw=4:
