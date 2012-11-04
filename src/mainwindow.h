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

#include <kxmlguiwindow.h>

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

class ResultModel;
class ListView;

class AbakusIface;

namespace Ui {
    class MainWindow;
}

// Main window class, handles events and layout and stuff
class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    public:
    MainWindow();

    bool inRPNMode() const;

    protected:
    virtual void contextMenuEvent(QContextMenuEvent *);
    virtual bool queryExit();
    
    Q_INVOKABLE void slotEvaluate(const QString &expression);
    Q_INVOKABLE void slotTextChanged(const QString &str);
    Q_INVOKABLE QString getTag(const int &index);

    Q_INVOKABLE void setDegrees();
    Q_INVOKABLE void setRadians();
    
    Q_INVOKABLE void setHistoryVisible(const bool &visible);
    Q_INVOKABLE void setNumeralsVisible(const bool &visible);
    Q_INVOKABLE void setFunctionsVisible(const bool &visible);

    Q_INVOKABLE int getVisibleHistoryItemIndex(int listIndex);
    Q_INVOKABLE void addVisibleHistoryItemIndex(int itemIndex);
    Q_INVOKABLE void removeVisibleHistoryItemIndex(int itemIndex);
    
    signals:
    void setEditorText(const QString &editorText);
    void trigModeChanged(const int &mode);//Abakus::TrigMode &mode);
    void historyVisibleChanged(const bool &visible);
    void numeralsVisibleChanged(const bool &visible);
    void functionsVisibleChanged(const bool &visible);
    
    private slots:
    void slotPrecisionAuto();
    void slotPrecision3();
    void slotPrecision8();
    void slotPrecision15();
    void slotPrecision50();
    void slotPrecisionCustom();

    void slotUpdateSize();

    void slotDegrees();
    void slotRadians();

    void slotToggleMenuBar();
    void slotToggleFunctionList();
    void slotToggleVariableList();
    void slotToggleHistoryList();
    void slotToggleCompactMode();
    void slotToggleExpressionMode();

    private:
    int getParenthesesLevel(const QString &str);

    void redrawResults();
    void selectCorrectPrecisionAction();

    void loadConfig();
    void saveConfig();
    void setupLayout();
    QString interpolateExpression(const QString &text);

    // Donated via JuK
    QAction *action(const char *key) const;

    template <class T> T *action(const char *key) const
    {
        return dynamic_cast<T *>(action(key));
    }

    private:
    QMenu *m_popup;
    ResultModel *m_resultItemModel;
    QSize m_newSize, m_oldSize;

//    AbakusIface *m_dcopInterface;
    
    QDeclarativeView* m_declarativeView;
    QDeclarativeContext *m_declarativeContext;

    bool m_historyVisible;
    bool m_numeralsVisible;
    bool m_functionsVisible;
    bool m_wasFnShown, m_wasVarShown, m_wasHistoryShown;
    bool m_compactMode;

    bool m_insert;

    QList<int> m_visibleHistoryItemIndices;
};

#endif

// vim: set et ts=8 sw=4:
