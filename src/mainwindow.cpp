/*
 * mainwindow.cpp - part of abakus
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
#include "mainwindow.h"
#include "abakuscommon.h"

#include <kaccel.h>
#include <kmenubar.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstdaction.h>
#include <kshortcut.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kconfigbase.h>
#include <kactionclasses.h>
#include <kinputdialog.h>

#include <qlayout.h>


#include <qradiobutton.h>
#include <q3buttongroup.h>
#include <qsplitter.h>
//Added by qt3to4:
#include <QEvent>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QAction>
#include <QActionGroup>
#include <kvbox.h>

#include "editor.h"
#include "evaluator.h"
#include "function.h"
#include "resultlistview.h"
#include "resultlistviewtext.h"
#include "valuemanager.h"
#include "node.h"
#include "rpnmuncher.h"
#include "dcopIface.h"
#include "abakuslistview.h"
#include "result.h"

MainWindow::MainWindow() : KMainWindow(0, "abakus-mainwindow"), m_popup(0), m_insert(false)
{
    m_mainSplitter = new QSplitter(this);
    QWidget *box = new QWidget(m_mainSplitter);
    QVBoxLayout *layout = new QVBoxLayout(box);
    m_layout = layout;
    layout->setSpacing(6);
    layout->setMargin(6);

    QWidget *configBox = new QWidget(box);
    layout->addWidget(configBox);

    QHBoxLayout *configLayout = new QHBoxLayout(configBox);

    configLayout->addWidget(new QWidget(configBox));

    QLabel *label = new QLabel(i18n("History: "), configBox);
    label->setAlignment(Qt::AlignCenter);
    configLayout->addWidget(label);

    Q3ButtonGroup *buttonGroup = new Q3ButtonGroup(0);

    QWidget *buttonGroupBox = new QWidget(configBox);
    QHBoxLayout *buttonGroupLayout = new QHBoxLayout(buttonGroupBox);
    buttonGroupLayout->addStretch(0);

    configLayout->addWidget(buttonGroupBox);

    m_degrees = new QRadioButton(i18n("&Degrees"), buttonGroupBox);
    buttonGroup->insert(m_degrees);
    buttonGroupLayout->addWidget(m_degrees);
    slotDegrees();
    connect(m_degrees, SIGNAL(clicked()), SLOT(slotDegrees()));

    m_radians = new QRadioButton(i18n("&Radians"), buttonGroupBox);
    buttonGroup->insert(m_radians);
    buttonGroupLayout->addWidget(m_radians);
    connect(m_radians, SIGNAL(clicked()), SLOT(slotRadians()));

    m_history = new KVBox(box);
    layout->addWidget(m_history);
    m_history->setSpacing(6);
    m_history->setMargin(0);

    m_result = new ResultListView(m_history);
    m_result->setSelectionMode(Q3ListView::NoSelection);
    m_result->setHScrollBarMode(ResultListView::AlwaysOff);
    connect(m_result, SIGNAL(signalEntrySelected(const QString &)),
                      SLOT(slotEntrySelected(const QString &)));
    connect(m_result, SIGNAL(signalResultSelected(const QString &)),
                      SLOT(slotResultSelected(const QString &)));

    m_history->setStretchFactor(m_result, 1);
    layout->setStretchFactor(m_history, 1);

    KHBox *editBox = new KHBox(box);
    layout->addWidget(editBox);
    editBox->setSpacing(6);

    m_edit = new Editor(editBox);
    m_edit->setFocus();
    editBox->setStretchFactor(m_edit, 1);

    KPushButton *evalButton = new KPushButton(i18n("&Evaluate"), editBox);

    connect(evalButton, SIGNAL(clicked()), SLOT(slotEvaluate()));

    connect(m_edit, SIGNAL(returnPressed()), SLOT(slotReturnPressed()));
    connect(m_edit, SIGNAL(textChanged()), SLOT(slotTextChanged()));

    m_listSplitter = new QSplitter(Qt::Vertical, m_mainSplitter);
    m_fnList = new FunctionListView(m_listSplitter);
    m_fnList->addColumn("Functions");
    m_fnList->addColumn("Value");

    m_varList = new VariableListView(m_listSplitter);
    m_varList->addColumn("Variables");
    m_varList->addColumn("Value");

    connect(FunctionManager::instance(), SIGNAL(signalFunctionAdded(const QString &)),
            this, SLOT(slotNewFunction(const QString &)));
    connect(FunctionManager::instance(), SIGNAL(signalFunctionRemoved(const QString &)),
            this, SLOT(slotRemoveFunction(const QString &)));

    connect(ValueManager::instance(), SIGNAL(signalValueAdded(const QString &, Abakus::number_t)),
            this, SLOT(slotNewValue(const QString &, Abakus::number_t)));
    connect(ValueManager::instance(), SIGNAL(signalValueChanged(const QString &, Abakus::number_t)),
            this, SLOT(slotChangeValue(const QString &, Abakus::number_t)));
    connect(ValueManager::instance(), SIGNAL(signalValueRemoved(const QString &)),
            this, SLOT(slotRemoveValue(const QString &)));

    setupLayout();

    setCentralWidget(m_mainSplitter);

#if KDE_IS_VERSION(3,4,89)
    setupGUI(QSize(450, 400), Keys | StatusBar | Save | Create);
#else
    setupGUI(Keys | StatusBar | Save | Create);
#endif

    m_dcopInterface = new AbakusIface();
}

bool MainWindow::inRPNMode() const
{
    return action("toggleExpressionMode")->isChecked();
}

bool MainWindow::eventFilter(QObject *o, QEvent *e)
{
    return KMainWindow::eventFilter(o, e);
}

bool MainWindow::queryExit()
{
    saveConfig();
    return KMainWindow::queryExit();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    static KMenu *popup = 0;

    if(!popup) {
        popup = new KMenu(this);
        action("options_show_menubar")->plug(popup);
    }

    if(!action("options_show_menubar")->isChecked())
        popup->popup(e->globalPos());
}

void MainWindow::polish()
{
    KMainWindow::polish();
    loadConfig();
}

void MainWindow::slotReturnPressed()
{
    QString text = m_edit->text();

    text.replace("\n", "");

    m_edit->appendHistory(text);

    // Item to insert after
    ResultListViewText *after = m_result->lastItem();

    // Expand $foo references.
    QString str = interpolateExpression(text, after);

    QString resultVal;
    ResultListViewText *item = 0;

    if(str.isNull())
        return; // Error already has been posted

    m_insert = false; // Assume we failed

    if(inRPNMode()) {
        // We're in RPN mode.
        Abakus::number_t result = RPNParser::rpnParseString(str);

        if(!RPNParser::wasError()) {
            resultVal = result.toString();
            ValueManager::instance()->setValue("ans", result);
            m_insert = true;
        }
        else {
            m_insert = false;
            resultVal = i18n("Error: %1").arg(RPNParser::errorString());
        }

        // Skip creating list view items if in compact mode.
        if(!m_history->isShown()) {
            m_edit->setText(resultVal);
            QTimer::singleShot(0, m_edit, SLOT(selectAll()));

            return;
        }

        item = new ResultListViewText(m_result, str, resultVal, after, false);
    }
    else {

        // Check to see if it's just a function name, if so, add (ans).
        if(FunctionManager::instance()->isFunction(str))
            str += " ans";

        // Add right parentheses as needed to balance out the expression.
        int parenLevel = getParenthesesLevel(str);
        for(int i = 0; i < parenLevel; ++i)
            str += ')';

        Abakus::number_t result = parseString(str.toLatin1());

        bool compact = !m_history->isShown();

        switch(Result::lastResult()->type()) {
            case Result::Value:
                resultVal = result.toString();

                ValueManager::instance()->setValue("ans", result);
                if(!compact)
                    item = new ResultListViewText(m_result, str, result, after, false);

                m_insert = true;
            break;

            case Result::Null: // OK, no result to speak of
                resultVal = "OK";
                if(!compact)
                    item = new ResultListViewText(m_result, str, resultVal, after, true);
            break;

            default:
                resultVal = Result::lastResult()->message();
                if(!compact)
                    item = new ResultListViewText(m_result, str, resultVal, after, true);
        }

        // Skip creating list view items if in compact mode.
        if(compact) {
            m_edit->setText(resultVal);
            QTimer::singleShot(0, m_edit, SLOT(selectAll()));

            return;
        }
    }

    m_edit->setText(text);

    m_result->setCurrentItem(item);
    m_result->ensureItemVisible(item);

    QTimer::singleShot(0, m_edit, SLOT(selectAll()));
}

void MainWindow::slotTextChanged()
{
    QString str = m_edit->text();

    if(str.length() == 1 && m_insert) {
        m_insert = false;

        // Don't do anything if in RPN Mode.
        if(inRPNMode())
            return;

        if(str.find(QRegExp("^[-+*/^]")) != -1) {
            m_edit->setText("ans " + str + " ");
            m_edit->moveCursor(Q3TextEdit::MoveEnd, false);
        }
    }
}

void MainWindow::slotEvaluate()
{
    slotReturnPressed();
}

void MainWindow::slotUpdateSize()
{
    if(m_newSize != QSize(0, 0))
        resize(m_newSize);
    else
        resize(width(), minimumSize().height());
}

void MainWindow::slotDegrees()
{
    setTrigMode(Abakus::Degrees);
    m_degrees->setChecked(true);
    if(action("setDegreesMode"))
        action("setDegreesMode")->setChecked(true);
}

void MainWindow::slotRadians()
{
    setTrigMode(Abakus::Radians);
    m_radians->setChecked(true);
    if(action("setRadiansMode"))
        action("setRadiansMode")->setChecked(true);
}

int MainWindow::getParenthesesLevel(const QString &str)
{
    int level = 0;

    for(unsigned i = 0; i < str.length(); ++i)
        if(str[i] == '(')
            ++level;
        else if(str[i] == ')')
            --level;

    return level;
}

void MainWindow::loadConfig()
{
    {
        KConfigGroup config(KGlobal::config(), "Settings");

        QString mode = config.readEntry("Trigonometric mode", "Degrees");
        if(mode == "Degrees") {
            setTrigMode(Abakus::Degrees);
            m_degrees->setChecked(true);
        }
        else {
            setTrigMode(Abakus::Radians);
            m_radians->setChecked(true);
        }

        bool useRPN = config.readBoolEntry("Use RPN Mode", false);
        action("toggleExpressionMode")->setChecked(useRPN);

        int precision = config.readNumEntry("Decimal Precision", -1);
        if(precision < -1 || precision > 75)
            precision = -1;

        Abakus::m_prec = precision;
        selectCorrectPrecisionAction();
    }

    {
        KConfigGroup config(KGlobal::config(), "Variables");

        QStringList list = config.readListEntry("Saved Variables");
        for(QStringList::ConstIterator it = list.begin(); it != list.end(); ++it) {
            QStringList values = QStringList::split('=', *it);
            if(values.count() != 2) {
                kWarning() << "Your configuration file has somehow been corrupted!\n";
                continue;
            }

            ValueManager::instance()->setValue(values[0], Abakus::number_t(values[1]));
        }
    }

    {
        KConfigGroup config(KGlobal::config(), "GUI");

        bool showHistory = config.readBoolEntry("ShowHistory", true);
        action("toggleHistoryList")->setChecked(showHistory);
        m_history->setShown(showHistory);

        bool showFunctions = config.readBoolEntry("ShowFunctions", true);
        action("toggleFunctionList")->setChecked(showFunctions);
        m_fnList->setShown(showFunctions);

        bool showVariables = config.readBoolEntry("ShowVariables", true);
        action("toggleVariableList")->setChecked(showVariables);
        m_varList->setShown(showVariables);

        bool compactMode = config.readBoolEntry("InCompactMode", false);
        compactMode = compactMode || !showHistory;
        action("toggleCompactMode")->setChecked(compactMode);

        if(compactMode)
            QTimer::singleShot(0, this, SLOT(slotToggleCompactMode()));
    }

    {
        KConfigGroup config(KGlobal::config(), "Functions");

        QStringList fnList = config.readListEntry("FunctionList");
        foreach(QString fn, fnList) {
            // Run the function definitions through the parser
            parseString(fn.toLocal8Bit().constData());
        }
    }

    populateListViews();
}

void MainWindow::saveConfig()
{
    {
        KConfigGroup config(KGlobal::config(), "Settings");

        config.writeEntry("Trigonometric mode",
             trigMode() == Abakus::Degrees
             ? "Degrees"
             : "Radians");

        config.writeEntry("Use RPN Mode", inRPNMode());
        config.writeEntry("Decimal Precision", Abakus::m_prec);
    }

    {
        KConfigGroup config(KGlobal::config(), "Variables");

        QStringList list;
        QStringList values = ValueManager::instance()->valueNames();
        QStringList::ConstIterator it = values.begin();

        // Set precision to max for most accuracy
        Abakus::m_prec = 75;

        for(; it != values.end(); ++it) {
            if(ValueManager::instance()->isValueReadOnly(*it))
                continue;

            list += QString("%1=%2")
                        .arg(*it)
                        .arg(ValueManager::instance()->value(*it).toString());
        }

        config.writeEntry("Saved Variables", list);
    }

    {
        KConfigGroup config(KGlobal::config(), "GUI");
        bool inCompactMode = action("toggleCompactMode")->isChecked();

        config.writeEntry("InCompactMode", inCompactMode);

        if(!inCompactMode) {
            config.writeEntry("ShowHistory", m_history->isShown());
            config.writeEntry("ShowFunctions", m_fnList->isShown());
            config.writeEntry("ShowVariables", m_varList->isShown());
        }
        else {
            config.writeEntry("ShowHistory", m_wasHistoryShown);
            config.writeEntry("ShowFunctions", m_wasFnShown);
            config.writeEntry("ShowVariables", m_wasVarShown);
        }
    }

    {
        KConfigGroup config(KGlobal::config(), "Functions");

        FunctionManager *manager = FunctionManager::instance();

        QStringList userFunctions = manager->functionList(FunctionManager::UserDefined);
        QStringList saveList;

        for(QStringList::ConstIterator it = userFunctions.begin(); it != userFunctions.end(); ++it) {
            UnaryFunction *fn = dynamic_cast<UnaryFunction *>(manager->function(*it)->userFn->fn);
            QString var = manager->function(*it)->userFn->varName;
            QString expr = fn->operand()->infixString();

            saveList += QString("set %1(%2) = %3").arg(*it).arg(var).arg(expr);
        }

        config.writeEntry("FunctionList", saveList);
    }
}

void MainWindow::setupLayout()
{
    KActionCollection *ac = actionCollection();

    KStdAction::quit(kapp, SLOT(quit()), ac);
    KStdAction::showMenubar(this, SLOT(slotToggleMenuBar()), ac);

    QActionGroup *trigActions = new QActionGroup(this);

    KAction *ta = new KAction(i18n("Degrees"), ac, "setDegreesMode");
    ta->setChecked(trigMode() == Abakus::Degrees);
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_D);

    trigActions->addAction(ta);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotDegrees()));

    ta = new KAction(i18n("Radians"), ac, "setRadiansMode");
    ta->setChecked(trigMode() == Abakus::Radians);
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_R);

    trigActions->addAction(ta);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotRadians()));

    ta = new KAction(i18n("Show History List"), ac, "toggleHistoryList");
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_H);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotToggleHistoryList()));
    ta->setChecked(true);

    ta = new KAction(i18n("Show Variables"), ac, "toggleVariableList");
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_V);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotToggleVariableList()));
    ta->setChecked(true);

    ta = new KAction(i18n("Show Functions"), ac, "toggleFunctionList");
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_F);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotToggleFunctionList()));
    ta->setChecked(true);

    ta = new KAction(i18n("Activate Compact Mode"), ac, "toggleCompactMode");
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_C);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotToggleCompactMode()));
    ta->setChecked(false);

    ta = new KAction(i18n("Use RPN Mode"), ac, "toggleExpressionMode");
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_P);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotToggleExpressionMode()));
    ta->setChecked(false);

    // Precision actions.
    QActionGroup *precActions = new QActionGroup(this);

    ta = new KAction(i18n("Automatic Precision"), ac, "precisionAuto");
    ta->setShortcut(0);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotPrecisionAuto()));
    ta->setChecked(true);
    precActions->addAction(ta);

    ta = new KAction(i18n("3 Decimal Digits"), ac, "precision3");
    ta->setShortcut(0);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotPrecision3()));
    ta->setChecked(false);
    precActions->addAction(ta);

    ta = new KAction(i18n("8 Decimal Digits"), ac, "precision8");
    ta->setShortcut(0);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotPrecision8()));
    ta->setChecked(false);
    precActions->addAction(ta);

    ta = new KAction(i18n("15 Decimal Digits"), ac, "precision15");
    ta->setShortcut(0);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotPrecision15()));
    ta->setChecked(false);
    precActions->addAction(ta);

    ta = new KAction(i18n("50 Decimal Digits"), ac, "precision50");
    ta->setShortcut(0);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotPrecision50()));
    ta->setChecked(false);
    precActions->addAction(ta);

    ta = new KAction(i18n("Custom Precision..."), ac, "precisionCustom");
    ta->setShortcut(0);
    connect(ta, SIGNAL(triggered(bool)), SLOT(slotPrecisionCustom()));
    ta->setChecked(false);
    precActions->addAction(ta);

    ta = new KAction(KIcon("editclear"), i18n("Clear History"), ac, "clearHistory");
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_L);
    connect(ta, SIGNAL(triggered(bool)), m_result, SLOT(clear()));

    ta = new KAction(KIcon("goto"), i18n("Select Editor"), ac, "select_edit");
    ta->setShortcut(Qt::Key_F6);
    connect(ta, SIGNAL(triggered(bool)), m_edit, SLOT(setFocus()));
}

void MainWindow::populateListViews()
{
    QStringList values = ValueManager::instance()->valueNames();

    Abakus::number_t value = ValueManager::instance()->value("pi");
    new ValueListViewItem(m_varList, "pi", value);

    value = ValueManager::instance()->value("e");
    new ValueListViewItem(m_varList, "e", value);
}

KAction *MainWindow::action(const char *key) const
{
    return actionCollection()->action(key);
}

void MainWindow::slotEntrySelected(const QString &text)
{
    m_edit->setText(text);
    m_edit->moveCursor(Q3TextEdit::MoveEnd, false);
}

void MainWindow::slotResultSelected(const QString &text)
{
    m_edit->insert(text);
}

void MainWindow::slotToggleMenuBar()
{
    menuBar()->setShown(!menuBar()->isShown());
}

void MainWindow::slotToggleFunctionList()
{
    bool show = action("toggleFunctionList")->isChecked();
    m_fnList->setShown(show);

    if(!m_history->isShown()) {
        m_history->setShown(true);
        action("toggleHistoryList")->setChecked(true);
        slotToggleHistoryList();
    }

    action("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotToggleVariableList()
{
    bool show = action("toggleVariableList")->isChecked();
    m_varList->setShown(show);

    if(!m_history->isShown()) {
        m_history->setShown(true);
        action("toggleHistoryList")->setChecked(true);
        slotToggleHistoryList();
    }

    action("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotToggleHistoryList()
{
    bool show = action("toggleHistoryList")->isChecked();
    m_history->setShown(show);

    action("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotNewFunction(const QString &name)
{
    UserFunction *userFn = FunctionManager::instance()->function(name)->userFn;
    UnaryFunction *fn = dynamic_cast<UnaryFunction *>(userFn->fn);
    QString fnName = QString("%1(%2)").arg(name, userFn->varName);
    QString expr = fn->operand()->infixString();

    new K3ListViewItem(m_fnList, fnName, expr);
}

void MainWindow::slotRemoveFunction(const QString &name)
{
    UserFunction *userFn = FunctionManager::instance()->function(name)->userFn;
    QString fnName = QString("%1(%2)").arg(name, userFn->varName);

    Q3ListViewItem *item = 0;
    while((item = m_fnList->findItem(fnName, 0)) != 0)
        delete item;
}

void MainWindow::slotNewValue(const QString &name, Abakus::number_t value)
{
    new ValueListViewItem(m_varList, name, value);
}

void MainWindow::slotChangeValue(const QString &name, Abakus::number_t value)
{
    ValueListViewItem *item = static_cast<ValueListViewItem *>(m_varList->findItem(name, 0));

    if(item)
        item->valueChanged(value);
}

void MainWindow::slotRemoveValue(const QString &name)
{
    delete m_varList->findItem(name, 0);
}

void MainWindow::slotToggleCompactMode()
{
    if(action("toggleCompactMode")->isChecked()) {
        m_wasFnShown = m_fnList->isShown();
        m_wasVarShown = m_varList->isShown();
        m_wasHistoryShown = m_history->isShown();

        m_fnList->setShown(false);
        m_varList->setShown(false);
        m_history->setShown(false);

        action("toggleFunctionList")->setChecked(false);
        action("toggleVariableList")->setChecked(false);
        action("toggleHistoryList")->setChecked(false);

        m_oldSize = size();
        m_newSize = QSize(0, 0);
        QTimer::singleShot(0, this, SLOT(slotUpdateSize()));
    }
    else {
        m_fnList->setShown(m_wasFnShown);
        m_varList->setShown(m_wasVarShown);
        m_history->setShown(m_wasHistoryShown);

        action("toggleFunctionList")->setChecked(m_wasFnShown);
        action("toggleVariableList")->setChecked(m_wasVarShown);
        action("toggleHistoryList")->setChecked(m_wasHistoryShown);

        m_newSize = m_oldSize;
        QTimer::singleShot(0, this, SLOT(slotUpdateSize()));
    }
}

void MainWindow::slotToggleExpressionMode()
{
}

QString MainWindow::interpolateExpression(const QString &text, ResultListViewText *after)
{
    QString str(text);
    QRegExp stackRE("\\$\\d+");
    int pos;

    while((pos = stackRE.search(str)) != -1) {
        QString stackStr = stackRE.cap();
        Abakus::number_t value;
        unsigned numPos = stackStr.mid(1).toUInt();

        if(!m_result->getStackValue(numPos, value)) {
            new ResultListViewText(m_result, text, i18n("Marker %1 isn't set").arg(stackStr), after, true);
            return QString::null;
        }

        str.replace(pos, stackStr.length(), value.toString());
    }

    return str;
}

void MainWindow::slotPrecisionAuto()
{
    Abakus::m_prec = -1;
    redrawResults();
}

void MainWindow::slotPrecision3()
{
    Abakus::m_prec = 3;
    redrawResults();
}

void MainWindow::slotPrecision8()
{
    Abakus::m_prec = 8;
    redrawResults();
}

void MainWindow::slotPrecision15()
{
    Abakus::m_prec = 15;
    redrawResults();
}

void MainWindow::slotPrecision50()
{
    Abakus::m_prec = 50;
    redrawResults();
}

void MainWindow::slotPrecisionCustom()
{
    bool ok = false;
    int precision = KInputDialog::getInteger(i18n("Select number of decimal digits to display"),
            i18n("Decimal precision:"), Abakus::m_prec, 0, 75, 1, &ok, this);

    if(ok) {
        Abakus::m_prec = precision;
        redrawResults();
    }

    selectCorrectPrecisionAction();
}

void MainWindow::redrawResults()
{
    Q3ListViewItemIterator it(m_result);

    while(it.current()) {
        static_cast<ResultListViewText *>(it.current())->precisionChanged();
        ++it;
    }

    it = Q3ListViewItemIterator (m_varList);

    while(it.current()) {
        static_cast<ValueListViewItem *>(it.current())->valueChanged();
        ++it;
    }

    // Because of the way we implemented the menu, it is possible to deselect
    // every possibility, so make sure we have at least one selected.
    selectCorrectPrecisionAction();
}

// This function selects the correct precision action based on the value of
// Abakus::m_prec.  Useful for loading settings, or for custom precision
// selection.
void MainWindow::selectCorrectPrecisionAction()
{
    switch(Abakus::m_prec) {
        case 3:
            action("precision3")->setChecked(true);
        break;

        case 8:
            action("precision8")->setChecked(true);
        break;

        case 15:
            action("precision15")->setChecked(true);
        break;

        case 50:
            action("precision50")->setChecked(true);
        break;

        case -1:
            action("precisionAuto")->setChecked(true);
        break;

        default:
            action("precisionCustom")->setChecked(true);
    }
}

#include "mainwindow.moc"

// vim: set et ts=8 sw=4:
