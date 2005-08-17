#ifndef ABAKUS_MAINWINDOW_H
#define ABAKUS_MAINWINDOW_H
/*
 * mainwindow.h - part of abakus
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

#include <kmainwindow.h>

#include "numerictypes.h"

class QPoint;
class QVBox;
class QCheckBox;
class QRadioButton;
class QBoxLayout;
class QListViewItem;
class QSplitter;
class QTimer;

//class KComboBox;
class Editor;
class KPopupMenu;
class KAction;
class KListView;
class ResultListView;
class ResultListViewText;

class AbakusIface;

// Main window class, handles events and layout and stuff
class MainWindow : public KMainWindow
{
    Q_OBJECT
    public:
    MainWindow();

    bool inRPNMode() const;

    protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual bool eventFilter(QObject *o, QEvent *e);
    virtual bool queryExit();
    virtual void polish();

    private slots:
    void slotReturnPressed();
    void slotTextChanged();
    void slotEvaluate();

    void slotPrecisionAuto();
    void slotPrecision3();
    void slotPrecision8();
    void slotPrecision15();
    void slotPrecision50();
    void slotPrecisionCustom();

    void slotUpdateSize();

    void slotDegrees();
    void slotRadians();

    void slotEntrySelected(const QString &text);
    void slotResultSelected(const QString &text);

    void slotToggleMenuBar();
    void slotToggleFunctionList();
    void slotToggleVariableList();
    void slotToggleHistoryList();
    void slotToggleCompactMode();
    void slotToggleExpressionMode();

    void slotNewValue(const QString &name, Abakus::number_t value);
    void slotChangeValue(const QString &name, Abakus::number_t value);
    void slotRemoveValue(const QString &name);

    void slotNewFunction(const QString &name);
    void slotRemoveFunction(const QString &name);

    void slotFnRightClick(QListViewItem *item, const QPoint &point);
    void slotVarRightClick(QListViewItem *item, const QPoint &point);

    void slotRemoveSelectedFn();
    void slotRemoveSelectedVar();

    private:
    int getParenthesesLevel(const QString &str);

    void redrawResults();
    void selectCorrectPrecisionAction();

    void loadConfig();
    void saveConfig();
    void setupLayout();
    void populateListViews();
    QString interpolateExpression(const QString &text, ResultListViewText *after);

    // Donated via JuK
    KAction *action(const char *key) const;

    template <class T> T *action(const char *key) const
    {
	return dynamic_cast<T *>(action(key));
    }

    private:
    QVBox *m_history;
    QRadioButton *m_degrees;
    QRadioButton *m_radians;
    Editor *m_edit;
    KPopupMenu *m_popup;
    ResultListView *m_result;
    QString m_lastError;
    QBoxLayout *m_layout;
    KListView *m_fnList, *m_varList;
    QSplitter *m_mainSplitter, *m_listSplitter;
    QSize m_newSize, m_oldSize;

    AbakusIface *m_dcopInterface;

    bool m_wasFnShown, m_wasVarShown, m_wasHistoryShown;
    bool m_compactMode;

    bool m_insert;
};

#endif
