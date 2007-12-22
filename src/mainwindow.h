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

//class KComboBox;
//class Editor;
class KVBox;

class ResultModel;
class ListView;

class AbakusIface;

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

    private slots:
    void slotReturnPressed();
    void slotTextChanged(const QString &);
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

    void itemClicked(const QModelIndex &);
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

    private:
    int getParenthesesLevel(const QString &str);

    void redrawResults();
    void selectCorrectPrecisionAction();

    void loadConfig();
    void saveConfig();
    void setupLayout();
    void populateListViews();
    QString interpolateExpression(const QString &text);

    // Donated via JuK
    QAction *action(const char *key) const;

    template <class T> T *action(const char *key) const
    {
        return dynamic_cast<T *>(action(key));
    }

    private:
    KVBox *m_history;
    QRadioButton *m_degrees;
    QRadioButton *m_radians;
    QLineEdit *m_edit;
    QMenu *m_popup;
    ResultModel *m_resultItemModel;
    QString m_lastError;
    QBoxLayout *m_layout;
    ListView *m_fnList, *m_varList;
    QSplitter *m_mainSplitter, *m_listSplitter;
    QSize m_newSize, m_oldSize;

//    AbakusIface *m_dcopInterface;

    bool m_wasFnShown, m_wasVarShown, m_wasHistoryShown;
    bool m_compactMode;

    bool m_insert;
};

#endif

// vim: set et ts=8 sw=4:
