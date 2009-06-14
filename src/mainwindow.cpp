/*
 * mainwindow.cpp - part of abakus
 * Copyright (C) 2004, 2005, 2007, 2009 Michael Pyne <mpyne@kde.org>
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

#include <kmenubar.h>
#include <kstandardaction.h>
#include <kstandardshortcut.h>
#include <kshortcut.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kglobal.h>
#include <klocale.h>
#include <kapplication.h>
#include <kconfigbase.h>
#include <kactioncollection.h>
#include <kconfiggroup.h>
#include <kinputdialog.h>
#include <ktoggleaction.h>

#include <QtGui/QLineEdit>
#include <QtGui/QTreeView>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QTreeWidget>
#include <QtGui/QTreeWidgetItemIterator>
#include <QtGui/QStandardItem>
#include <QtGui/QStandardItemModel>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QHeaderView>
#include <QtCore/QTimer>

#include "editor.h"
#include "evaluator.h"
#include "function.h"
#include "valuemanager.h"
#include "node.h"
#include "rpnmuncher.h"
//#include "dcopIface.h"
#include "ui_mainwindow.h"
#include "abakuslistview.h"
#include "result.h"
#include "resultmodel.h"

MainWindow::MainWindow() :
    KXmlGuiWindow(0), m_ui(new Ui::MainWindow), m_popup(0), m_insert(false)
{
    setObjectName("abakusMainWindow");
    QWidget *w = new QWidget(this);
    setCentralWidget(w);

    m_ui->setupUi(w);

    slotDegrees();
    connect(m_ui->degreesButton, SIGNAL(clicked()), SLOT(slotDegrees()));
    connect(m_ui->radiansButton, SIGNAL(clicked()), SLOT(slotRadians()));

    m_resultItemModel = new ResultModel(this);
    m_ui->resultList->setModel(m_resultItemModel->model());
    m_ui->resultList->header()->setResizeMode(QHeaderView::ResizeToContents);
    connect(m_resultItemModel, SIGNAL(signalColumnChanged(int)),
            m_ui->resultList, SLOT(resizeColumnToContents(int)));
    connect(m_ui->resultList,  SIGNAL(clicked(const QModelIndex &)),
            this, SLOT(itemClicked(const QModelIndex &)));

    m_ui->expression->setFocus();

    connect(m_ui->evaluateButton, SIGNAL(clicked()), SLOT(slotEvaluate()));
    connect(m_ui->expression, SIGNAL(returnPressed()), SLOT(slotEvaluate()));
    connect(m_ui->expression, SIGNAL(textChanged(const QString &)),
            this,         SLOT(slotTextChanged(const QString &)));

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
    setupGUI(QSize(450, 400), Keys | StatusBar | Save | Create);
    setAutoSaveSettings();

    loadConfig();
//    m_dcopInterface = new AbakusIface();
}

bool MainWindow::inRPNMode() const
{
    return action<KToggleAction>("toggleExpressionMode")->isChecked();
}

bool MainWindow::queryExit()
{
    saveConfig();
    return KXmlGuiWindow::queryExit();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *e)
{
    static QMenu *popup = 0;

    if(!popup) {
        popup = new QMenu(this);
        popup->addAction(action("options_show_menubar"));
    }

    if(!action<KToggleAction>("options_show_menubar")->isChecked())
        popup->popup(e->globalPos());
}

void MainWindow::slotEvaluate()
{
    QString text = m_ui->expression->text();

    text.replace("\n", "");
    if(text.isEmpty())
        return;

    // TODO: Put this back in, used to be Editor.
    // m_ui->expression->appendHistory(text);

    // Expand $foo references.
    QString str = interpolateExpression(text);

    if(str.isEmpty())
        return; // Error already has been posted

    QString resultVal;

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
        if(m_compactMode) {
            m_ui->expression->setText(resultVal);
            QTimer::singleShot(0, m_ui->expression, SLOT(selectAll()));

            return;
        }

        m_resultItemModel->addResult(str, result);
    }
    else {

        // Check to see if it's just a function name, if so, add (ans).
        if(FunctionManager::instance()->isFunction(str))
            str += " ans";

        // Add right parentheses as needed to balance out the expression.
        int parenLevel = getParenthesesLevel(str);
        for(int i = 0; i < parenLevel; ++i)
            str += ')';

        QByteArray cStr = str.toLatin1();
        Abakus::number_t result = parseString(cStr.data());

        switch(Result::lastResult()->type()) {
            case Result::Value:
                resultVal = result.toString();

                ValueManager::instance()->setValue("ans", result);

                m_resultItemModel->addResult(str, result);
                m_insert = true;
            break;

            case Result::Null: // OK, no result to speak of
                resultVal = "OK";
                m_resultItemModel->addMessage(resultVal);
            break;

            default:
                resultVal = Result::lastResult()->message();
                m_resultItemModel->addMessage(resultVal);
        }

        // Skip creating list view items if in compact mode.
        if(m_compactMode) {
            m_ui->expression->setText(resultVal);
            QTimer::singleShot(0, m_ui->expression, SLOT(selectAll()));

            return;
        }
    }

    m_ui->expression->setText(text);

    QTimer::singleShot(0, m_ui->expression, SLOT(selectAll()));
}

void MainWindow::slotTextChanged(const QString &str)
{
    if(str.length() == 1 && m_insert) {
        m_insert = false;

        // Don't do anything if in RPN Mode.
        if(inRPNMode())
            return;

        if(QRegExp("^[-+*/^]").indexIn(str) != -1) {
            m_ui->expression->setText("ans " + str + " ");
            m_ui->expression->setCursorPosition(m_ui->expression->text().length());
        }
    }
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
    m_ui->degreesButton->setChecked(true);
    if(action("setDegreesMode"))
        action<KToggleAction>("setDegreesMode")->setChecked(true);
}

void MainWindow::slotRadians()
{
    setTrigMode(Abakus::Radians);
    m_ui->radiansButton->setChecked(true);
    if(action("setRadiansMode"))
        action<KToggleAction>("setRadiansMode")->setChecked(true);
}

int MainWindow::getParenthesesLevel(const QString &str)
{
    int level = 0;

    for(int i = 0; i < str.length(); ++i)
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
            m_ui->degreesButton->setChecked(true);
        }
        else {
            setTrigMode(Abakus::Radians);
            m_ui->radiansButton->setChecked(true);
        }

        bool useRPN = config.readEntry("Use RPN Mode", false);
        action<KToggleAction>("toggleExpressionMode")->setChecked(useRPN);

        int precision = config.readEntry("Decimal Precision", -1);
        if(precision < -1 || precision > 75)
            precision = -1;

        Abakus::m_prec = precision;
        selectCorrectPrecisionAction();
    }

    {
        KConfigGroup config(KGlobal::config(), "Variables");

        QStringList list = config.readEntry("Saved Variables", QStringList());
        foreach(QString str, list) {
            QStringList values = str.split('=');
            if(values.count() != 2) {
                kWarning() << "Your configuration file has somehow been corrupted!\n";
                continue;
            }

            QByteArray valueStr = values[1].toLatin1();
            ValueManager::instance()->setValue(values[0], Abakus::number_t(valueStr.data()));
        }
    }

    {
        KConfigGroup config(KGlobal::config(), "GUI");

        bool showHistory = config.readEntry("ShowHistory", true);
        action<KToggleAction>("toggleHistoryList")->setChecked(showHistory);
        m_ui->resultList->setShown(showHistory);

        bool showFunctions = config.readEntry("ShowFunctions", true);
        action<KToggleAction>("toggleFunctionList")->setChecked(showFunctions);
        m_ui->fnList->setShown(showFunctions);

        bool showVariables = config.readEntry("ShowVariables", true);
        action<KToggleAction>("toggleVariableList")->setChecked(showVariables);
        m_ui->varList->setShown(showVariables);

        bool compactMode = config.readEntry("InCompactMode", false);
        compactMode = compactMode || !showHistory;
        action<KToggleAction>("toggleCompactMode")->setChecked(compactMode);

        if(compactMode)
            QTimer::singleShot(0, this, SLOT(slotToggleCompactMode()));
    }

    {
        KConfigGroup config(KGlobal::config(), "Functions");

        QStringList fnList = config.readEntry("FunctionList", QStringList());
        foreach(QString str, fnList) {
            QByteArray strValue = str.toLatin1();
            parseString(strValue.data()); // Run the function definitions through the parser
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
        bool inCompactMode = action<KToggleAction>("toggleCompactMode")->isChecked();

        config.writeEntry("InCompactMode", inCompactMode);

        if(!inCompactMode) {
            config.writeEntry("ShowHistory", !m_ui->resultList->isHidden());
            config.writeEntry("ShowFunctions", !m_ui->fnList->isHidden());
            config.writeEntry("ShowVariables", !m_ui->varList->isHidden());
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

    KStandardAction::quit(kapp, SLOT(quit()), ac);
    KStandardAction::showMenubar(this, SLOT(slotToggleMenuBar()), ac);

    QActionGroup *trigModeGroup = new QActionGroup(ac);

    KToggleAction *ta = ac->add<KToggleAction>("setDegreesMode", this, SLOT(slotDegrees()));
    ta->setText(i18n("&Degrees"));
    ta->setShortcut(Qt::SHIFT | Qt::ALT | Qt::Key_D);
    ta->setActionGroup(trigModeGroup);
    ta->setChecked(trigMode() == Abakus::Degrees);

    ta = ac->add<KToggleAction>("setRadiansMode", this, SLOT(slotRadians()));
    ta->setText(i18n("&Radians"));
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_R);
    ta->setActionGroup(trigModeGroup);
    ta->setChecked(trigMode() == Abakus::Radians);

    ta = ac->add<KToggleAction>("toggleHistoryList", this, SLOT(slotToggleHistoryList()));
    ta->setText(i18n("Show &History List"));
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_H);
    ta->setChecked(true);

    ta = ac->add<KToggleAction>("toggleVariableList", this, SLOT(slotToggleVariableList()));
    ta->setText(i18n("Show &Variables"));
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_V);
    ta->setChecked(true);

    ta = ac->add<KToggleAction>("toggleFunctionList", this, SLOT(slotToggleFunctionList()));
    ta->setText(i18n("Show &Functions"));
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_F);
    ta->setChecked(true);

    ta = ac->add<KToggleAction>("toggleCompactMode", this, SLOT(slotToggleCompactMode()));
    ta->setText(i18n("Activate &Compact Mode"));
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_C);
    ta->setChecked(false);

    ta = ac->add<KToggleAction>("toggleExpressionMode", this, SLOT(slotToggleExpressionMode()));
    ta->setText(i18n("Use R&PN Mode"));
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_P);
    ta->setChecked(false);

    QActionGroup *precisionGroup = new QActionGroup(ac);

    // Precision actions.
    ta = ac->add<KToggleAction>("precisionAuto", this, SLOT(slotPrecisionAuto()));
    ta->setText(i18n("&Automatic Precision"));
    ta->setActionGroup(precisionGroup);
    ta->setChecked(true);

    ta = ac->add<KToggleAction>("precision3", this, SLOT(slotPrecision3()));
    ta->setText(i18n("&3 Decimal Digits"));
    ta->setActionGroup(precisionGroup);

    ta = ac->add<KToggleAction>("precision8", this, SLOT(slotPrecision8()));
    ta->setText(i18n("&8 Decimal Digits"));
    ta->setActionGroup(precisionGroup);

    ta = ac->add<KToggleAction>("precision15", this, SLOT(slotPrecision15()));
    ta->setText(i18n("&15 Decimal Digits"));
    ta->setActionGroup(precisionGroup);

    ta = ac->add<KToggleAction>("precision50", this, SLOT(slotPrecision50()));
    ta->setText(i18n("&50 Decimal Digits"));
    ta->setActionGroup(precisionGroup);

    ta = ac->add<KToggleAction>("precisionCustom", this, SLOT(slotPrecisionCustom()));
    ta->setText(i18n("C&ustom Precision..."));
    ta->setActionGroup(precisionGroup);

    QAction *a = ac->addAction("clearHistory", m_resultItemModel, SLOT(clear()));
    a->setText(i18n("Clear &History"));
    a->setIcon(KIcon("editclear"));
    ta->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_L);

    a = ac->addAction("select_edit", m_ui->expression, SLOT(setFocus()));
    a->setIcon(KIcon("goto"));
    a->setText(i18n("Select Editor"));
    a->setShortcut(Qt::Key_F6);
}

void MainWindow::populateListViews()
{
    QStringList values = ValueManager::instance()->valueNames();

    Abakus::number_t value = ValueManager::instance()->value("pi");
    new ValueTreeWidgetItem(m_ui->varList, "pi", value);

    value = ValueManager::instance()->value("e");
    new ValueTreeWidgetItem(m_ui->varList, "e", value);
}

QAction *MainWindow::action(const char *key) const
{
    return actionCollection()->action(key);
}

void MainWindow::slotEntrySelected(const QString &text)
{
    m_ui->expression->setText(text);
    m_ui->expression->setCursorPosition(m_ui->expression->text().length());
}

void MainWindow::slotResultSelected(const QString &text)
{
    m_ui->expression->insert(text);
}

void MainWindow::slotToggleMenuBar()
{
    menuBar()->setShown(menuBar()->isHidden());
}

void MainWindow::slotToggleFunctionList()
{
    bool show = action<KToggleAction>("toggleFunctionList")->isChecked();
    m_ui->fnList->setShown(show);

    if(m_compactMode) {
        action<KToggleAction>("toggleHistoryList")->setChecked(true);
        slotToggleHistoryList();
    }

    action<KToggleAction>("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotToggleVariableList()
{
    bool show = action<KToggleAction>("toggleVariableList")->isChecked();
    m_ui->varList->setShown(show);

    if(m_compactMode) {
        action<KToggleAction>("toggleHistoryList")->setChecked(true);
        slotToggleHistoryList();
    }

    action<KToggleAction>("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotToggleHistoryList()
{
    bool show = action<KToggleAction>("toggleHistoryList")->isChecked();
    m_ui->resultList->setShown(show);

    action<KToggleAction>("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotNewFunction(const QString &name)
{
    UserFunction *userFn = FunctionManager::instance()->function(name)->userFn;
    UnaryFunction *fn = dynamic_cast<UnaryFunction *>(userFn->fn);
    QString fnName = QString("%1(%2)").arg(name, userFn->varName);
    QString expr = fn->operand()->infixString();

    QTreeWidgetItem *item = new QTreeWidgetItem(m_ui->fnList);
    item->setText(0, fnName);
    item->setText(1, expr);
}

void MainWindow::slotRemoveFunction(const QString &name)
{
    UserFunction *userFn = FunctionManager::instance()->function(name)->userFn;
    QString fnName = QString("%1(%2)").arg(name, userFn->varName);

    QList<QTreeWidgetItem *> markedForDeath
        = m_ui->fnList->findItems(fnName, Qt::MatchFixedString, 0);

    foreach(QTreeWidgetItem *item, markedForDeath)
        delete item;
}

void MainWindow::slotNewValue(const QString &name, Abakus::number_t value)
{
    new ValueTreeWidgetItem(m_ui->varList, name, value);
}

void MainWindow::slotChangeValue(const QString &name, Abakus::number_t value)
{
    QList<QTreeWidgetItem *> items = m_ui->varList->findItems(name, Qt::MatchFixedString, 0);

    foreach(QTreeWidgetItem *item, items)
        static_cast<ValueTreeWidgetItem *>(item)->valueChanged(value);
}

void MainWindow::slotRemoveValue(const QString &name)
{
    QList<QTreeWidgetItem *> items = m_ui->varList->findItems(name, Qt::MatchFixedString, 0);

    foreach(QTreeWidgetItem *item, items)
        delete item;
}

void MainWindow::slotToggleCompactMode()
{
    if(action<KToggleAction>("toggleCompactMode")->isChecked()) {
        m_wasFnShown = !m_ui->fnList->isHidden();
        m_wasVarShown = !m_ui->varList->isHidden();
        m_wasHistoryShown = !m_ui->resultList->isHidden();
        m_compactMode = true;

        m_ui->fnList->setShown(false);
        m_ui->varList->setShown(false);
        m_ui->resultList->setShown(false);

        action<KToggleAction>("toggleFunctionList")->setChecked(false);
        action<KToggleAction>("toggleVariableList")->setChecked(false);
        action<KToggleAction>("toggleHistoryList")->setChecked(false);

        m_oldSize = size();
        m_newSize = QSize(0, 0);
        QTimer::singleShot(0, this, SLOT(slotUpdateSize()));
    }
    else {
        m_ui->fnList->setShown(m_wasFnShown);
        m_ui->varList->setShown(m_wasVarShown);
        m_ui->resultList->setShown(m_wasHistoryShown);
        m_compactMode = false;

        action<KToggleAction>("toggleFunctionList")->setChecked(m_wasFnShown);
        action<KToggleAction>("toggleVariableList")->setChecked(m_wasVarShown);
        action<KToggleAction>("toggleHistoryList")->setChecked(m_wasHistoryShown);

        m_newSize = m_oldSize;
        QTimer::singleShot(0, this, SLOT(slotUpdateSize()));
    }
}

void MainWindow::slotToggleExpressionMode()
{
}

QString MainWindow::interpolateExpression(const QString &text)
{
    QString str(text);
    QRegExp stackRE("\\$\\d+");
    int pos;

    while((pos = stackRE.indexIn(str)) != -1) {
        QString stackStr = stackRE.cap();
        Abakus::number_t value;
        unsigned numPos = stackStr.mid(1).toUInt();

        if(!m_resultItemModel->stackValue(numPos, value)) {
            m_resultItemModel->addMessage(i18n("Marker %1 isn't set", stackStr));
            return QString();
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
    m_resultItemModel->slotRedrawItems();
    m_ui->varList->redrawItems();

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
            action<KToggleAction>("precision3")->setChecked(true);
        break;

        case 8:
            action<KToggleAction>("precision8")->setChecked(true);
        break;

        case 15:
            action<KToggleAction>("precision15")->setChecked(true);
        break;

        case 50:
            action<KToggleAction>("precision50")->setChecked(true);
        break;

        case -1:
            action<KToggleAction>("precisionAuto")->setChecked(true);
        break;

        default:
            action<KToggleAction>("precisionCustom")->setChecked(true);
    }
}

void MainWindow::itemClicked(const QModelIndex &index)
{
    QStandardItem *item = m_resultItemModel->model()->itemFromIndex(index);
    if(!item)
        return;

    if(item->column() == 0)
        slotEntrySelected(item->text());
    else if(item->column() == 1)
        slotResultSelected(item->text());
}

#include "mainwindow.moc"

// vim: set et ts=8 sw=4:
