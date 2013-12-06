#ifndef ABAKUS_MAINWINDOW_H
#define ABAKUS_MAINWINDOW_H
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

#ifdef ABAKUS_QTONLY
    #include <QtQuick/QQuickView>
    #define KMainWindow QQuickView
    
    class QQmlContext;
#else
    #include <KMainWindow>
    
    class QDeclarativeContext;
    class QDeclarativeView;
    class QMenu;
#endif

class ResultModel;

// Main window class, handles events and stuff
class MainWindow : public KMainWindow
{
    Q_OBJECT
    
public:
    MainWindow();

protected:
    virtual bool queryExit();
    
    Q_INVOKABLE void slotEvaluate(const QString &expression);
    Q_INVOKABLE void slotTextChanged(const QString &str);
    Q_INVOKABLE QString getTag(const int &index);
    
    Q_INVOKABLE void showHelpMenu(int xPosition, int yPosition);
    Q_INVOKABLE void configureShortcuts();
    
    Q_INVOKABLE void showToolTip(const int xPosition, const int yPosition, const QString& toolTipText);
    Q_INVOKABLE void hideToolTip();
    
    Q_INVOKABLE void clearHistory();
    
    Q_INVOKABLE void historyPrevious();
    Q_INVOKABLE void historyNext();
    
    Q_INVOKABLE void removeNumeral(const QString &name);
    Q_INVOKABLE void removeFunction(const QString &name);

    Q_INVOKABLE int getVisibleHistoryItemIndex(int listIndex);
    Q_INVOKABLE void addVisibleHistoryItemIndex(int itemIndex);
    Q_INVOKABLE void removeVisibleHistoryItemIndex(int itemIndex);
    
signals:
    void setFocusToEditor();
    void setEditorText(const QString &editorText);
    
private slots:
    void slotSetDegrees();
    void slotSetRadians();
    void slotRedrawResults();
    void slotUpdateSize();
    void slotToggleCompactMode();
    void slotToggleMathematicalSidebar();

private:
    int getParenthesesLevel(const QString &str);

    void setupShortcuts();
    QString interpolateExpression(const QString &text);
    
    void setMathematicalSidebarVisible(const bool &visible);

#ifdef ABAKUS_QTONLY
    QQmlContext *m_declarativeContext;
#else
    QDeclarativeView* m_declarativeView;
    QDeclarativeContext *m_declarativeContext;
    QMenu* m_helpMenu; 
#endif
    
    ResultModel *m_resultItemModel;

    SettingsCore* m_settingscore;

    bool m_insert;

    QList<int> m_visibleHistoryItemIndices;
};

#endif

// vim: set et ts=8 sw=4:
