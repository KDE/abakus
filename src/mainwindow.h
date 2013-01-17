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

#include <kmainwindow.h>

#include "numerictypes.h"

class QPoint;
class QLineEdit;
class QCheckBox;
class QRadioButton;
class QBoxLayout;
class QListViewItem;
class QSplitter;
class QTimer;
class QMenu;
class QModelIndex;
class QAction;
class QDeclarativeView;
class QDeclarativeContext;

//class KComboBox;
//class Editor;
class KVBox;
class KActionCollection;

class ResultModel;
class ListView;

class AbakusIface;

namespace Ui {
    class MainWindow;
}

// Main window class, handles events and layout and stuff
class MainWindow : public KMainWindow
{
    Q_OBJECT
    public:
    MainWindow();

    bool inRPNMode() const;

    protected:
    virtual bool queryExit();
    
    Q_INVOKABLE void slotEvaluate(const QString &expression);
    Q_INVOKABLE void slotTextChanged(const QString &str);
    Q_INVOKABLE QString getTag(const int &index);
    
    Q_INVOKABLE void showHelpMenu(int xPosition, int yPosition);
    Q_INVOKABLE void configureShortcuts();
    Q_INVOKABLE void setPrecision(int precision);

    Q_INVOKABLE void setDegrees();
    Q_INVOKABLE void setRadians();
    
    Q_INVOKABLE void clearHistory();
    
    Q_INVOKABLE void historyPrevious();
    Q_INVOKABLE void historyNext();
    
    Q_INVOKABLE void setCompactMode(bool newMode);
    Q_INVOKABLE void slotToggleCompactMode();
    Q_INVOKABLE void slotToggleMathematicalSidebar();
    Q_INVOKABLE void mathematicalSidebarActiveTabChanged(const QString &activeTab);
    Q_INVOKABLE void slotToggleExpressionMode();
    
    Q_INVOKABLE void removeNumeral(const QString &name);
    Q_INVOKABLE void removeFunction(const QString &name);

    Q_INVOKABLE int getVisibleHistoryItemIndex(int listIndex);
    Q_INVOKABLE void addVisibleHistoryItemIndex(int itemIndex);
    Q_INVOKABLE void removeVisibleHistoryItemIndex(int itemIndex);
    
    signals:
    void setEditorText(const QString &editorText);
    void trigModeChanged(const int &mode);//Abakus::TrigMode &mode);
    void historyVisibleChanged(const bool &visible);
    void mathematicalSidebarVisibleChanged(const bool &visible);
    void setMathematicalActiveTab(const QString &activeTab);
    void compactModeChanged(const bool &active);
    void precisionChanged(int newPrecision);
    
    private slots:
    void slotUpdateSize();

    void slotDegrees();
    void slotRadians();

    private:
    int getParenthesesLevel(const QString &str);

    void redrawResults();

    void loadConfig();
    void saveConfig();
    void setupLayout();
    QString interpolateExpression(const QString &text);
    
    void setHistoryVisible(const bool &visible);
    void setMathematicalSidebarVisible(const bool &visible);

    private:
    QMenu* m_helpMenu;
    KActionCollection* m_actionCollection;
    ResultModel *m_resultItemModel;
    QSize m_size;

//    AbakusIface *m_dcopInterface;
    
    QDeclarativeView* m_declarativeView;
    QDeclarativeContext *m_declarativeContext;

    QString m_mathematicalSidebarActiveTab;
    bool m_mathematicalSidebarVisible;
    bool m_wasMathematicalSidebarShown;
    bool m_compactMode;
    bool m_rpnMode;
    int m_historyLimit;

    bool m_insert;

    QList<int> m_visibleHistoryItemIndices;
};

#endif

// vim: set et ts=8 sw=4:
