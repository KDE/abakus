/*
 * mainwindow.cpp - part of abakus
 * Copyright (C) 2012 Mathias Kraus <k.hias@gmx.net>
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

#include <KStandardDirs>
#include <QDeclarativeView>
#include <QDeclarativeContext>

#include "editor.h"
#include "evaluator.h"
#include "functionmodel.h"
#include "numeralmodel.h"
#include "node.h"
#include "rpnmuncher.h"
//#include "dcopIface.h"
#include "result.h"
#include "resultmodel.h"

MainWindow::MainWindow() :
    KXmlGuiWindow(0),
    m_popup(0),
    m_resultItemModel (new ResultModel(this)),
    m_newSize(QSize(600, 300)), m_oldSize(QSize(600, 300)),
    m_historyVisible(true),
    m_numeralsVisible(true),
    m_functionsVisible(true),
    m_wasFnShown(true), m_wasVarShown(true), m_wasHistoryShown(true),
    m_compactMode(false),
    m_insert(false)
{
    setObjectName("abakusMainWindow");

    slotDegrees();

    m_declarativeView = new QDeclarativeView(this);
    m_declarativeContext = m_declarativeView->rootContext();
    m_declarativeContext->setContextProperty("mainWindow", this);
    m_declarativeContext->setContextProperty("resultModel", m_resultItemModel);
    m_declarativeContext->setContextProperty("numeralModel", NumeralModel::instance());
    m_declarativeContext->setContextProperty("functionModel", FunctionModel::instance());
    m_declarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    // Set view optimizations not already done for QDeclarativeView
    m_declarativeView->setAttribute(Qt::WA_OpaquePaintEvent);
    m_declarativeView->setAttribute(Qt::WA_NoSystemBackground);
    
    QString filePath = KStandardDirs::locate("appdata", "qml/main.qml");
    m_declarativeView->setSource(QUrl::fromLocalFile(filePath));
    
    setCentralWidget(m_declarativeView);
    
    m_declarativeView->setFocus();
    
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
    m_resultItemModel->clear();
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

void MainWindow::slotEvaluate(const QString &expression)
{
    QString text = expression;

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
            NumeralModel::instance()->setValue("ans", result);
            m_insert = true;
        }
        else {
            m_insert = false;
            resultVal = i18n("Error: %1", RPNParser::errorString());
        }

        m_resultItemModel->addResult(str, result);
    }
    else {

        // Check to see if it's just a function name, if so, add (ans).
        if(FunctionModel::instance()->isFunction(str))
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

                NumeralModel::instance()->setValue("ans", result);

                m_resultItemModel->addResult(str, result);
                m_insert = true;
            break;

            case Result::Null: // OK, no result to speak of
                resultVal = i18n("OK");
                m_resultItemModel->addMessage(resultVal);
            break;

            default:
                resultVal = Result::lastResult()->message();
                m_resultItemModel->addMessage(resultVal);
        }
    }
    
    // set result in expression text edit if in compact mode
    if(m_compactMode) {
        emit setInputText(resultVal);
    }
}

void MainWindow::slotTextChanged(const QString &str)
{
    if(str.length() == 1 && m_insert) {
        m_insert = false;

        // Don't do anything if in RPN Mode.
        if(inRPNMode())
            return;

        if(QRegExp("^[-+*/^]").indexIn(str) != -1) {
            emit setInputText("ans " + str + " ");
        }
    }
}

QString MainWindow::getTag(const int &index)
{
    return m_resultItemModel->data(m_resultItemModel->index(index), ResultModel::TagRole).toString();
}

void MainWindow::setDegrees()
{
    slotDegrees();
}

void MainWindow::setRadians()
{
    slotRadians();
}

void MainWindow::setHistoryVisible(const bool& visible)
{
    m_historyVisible = visible;
    emit historyVisibleChanged(visible);
}

void MainWindow::setNumeralsVisible(const bool& visible)
{
    m_numeralsVisible = visible;
    emit numeralsVisibleChanged(visible);
}

void MainWindow::setFunctionsVisible(const bool& visible)
{
    m_functionsVisible = visible;
    emit functionsVisibleChanged(visible);
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
    emit trigModeChanged((int)Abakus::Degrees);
    if(action("setDegreesMode"))
        action<KToggleAction>("setDegreesMode")->setChecked(true);
}

void MainWindow::slotRadians()
{
    setTrigMode(Abakus::Radians);
    emit trigModeChanged((int)Abakus::Radians);
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
    KConfigGroup config = KGlobal::config()->group("Settings");

    QString mode = config.readEntry("Trigonometric mode", "Degrees");
    if(mode == "Degrees") {
        setTrigMode(Abakus::Degrees);
        emit trigModeChanged((int)Abakus::Degrees);
    }
    else {
        setTrigMode(Abakus::Radians);
        emit trigModeChanged((int)Abakus::Radians);
    }

    bool useRPN = config.readEntry("Use RPN Mode", false);
    action<KToggleAction>("toggleExpressionMode")->setChecked(useRPN);

    int precision = config.readEntry("Decimal Precision", -1);
    if(precision < -1 || precision > 75)
        precision = -1;

    Abakus::m_prec = precision;
    selectCorrectPrecisionAction();


    config = KGlobal::config()->group("Variables");

    QStringList list = config.readEntry("Saved Variables", QStringList());
    foreach(QString str, list) {
        QStringList values = str.split('=');
        if(values.count() != 2) {
            kWarning() << "Your configuration file has somehow been corrupted!\n";
            continue;
        }

        QByteArray valueStr = values[1].toLatin1();
        NumeralModel::instance()->setValue(values[0], Abakus::number_t(valueStr.data()));
    }

    
    config = KGlobal::config()->group("GUI");

    bool showHistory = config.readEntry("ShowHistory", true);
    action<KToggleAction>("toggleHistoryList")->setChecked(showHistory);
    setHistoryVisible(showHistory);

    bool showFunctions = config.readEntry("ShowFunctions", true);
    action<KToggleAction>("toggleFunctionList")->setChecked(showFunctions);
    setFunctionsVisible(showFunctions);

    bool showVariables = config.readEntry("ShowVariables", true);
    action<KToggleAction>("toggleVariableList")->setChecked(showVariables);
    setNumeralsVisible(showVariables);

    bool compactMode = config.readEntry("InCompactMode", false);
    compactMode = compactMode || !showHistory;
    action<KToggleAction>("toggleCompactMode")->setChecked(compactMode);

    if(compactMode)
        QTimer::singleShot(0, this, SLOT(slotToggleCompactMode()));


    config = KGlobal::config()->group("Functions");

    QStringList fnList = config.readEntry("FunctionList", QStringList());
    foreach(QString str, fnList) {
        QByteArray strValue = str.toLatin1();
        parseString(strValue.data()); // Run the function definitions through the parser
    }
}

void MainWindow::saveConfig()
{
    KConfigGroup config = KGlobal::config()->group("Settings");

    config.writeEntry("Trigonometric mode",
            trigMode() == Abakus::Degrees
            ? "Degrees"
            : "Radians");

    config.writeEntry("Use RPN Mode", inRPNMode());
    config.writeEntry("Decimal Precision", Abakus::m_prec);

    
    config = KGlobal::config()->group("Variables");

    QStringList list;
    QStringList values = NumeralModel::instance()->valueNames();
    QStringList::ConstIterator it = values.begin();

    // Set precision to max for most accuracy
    Abakus::m_prec = 75;

    for(; it != values.end(); ++it) {
        if(NumeralModel::instance()->isValueReadOnly(*it))
            continue;

        list += QString("%1=%2")
                    .arg(*it)
                    .arg(NumeralModel::instance()->value(*it).toString());
    }

    config.writeEntry("Saved Variables", list);


    config = KGlobal::config()->group("GUI");
    
    bool inCompactMode = action<KToggleAction>("toggleCompactMode")->isChecked();

    config.writeEntry("InCompactMode", inCompactMode);

    if(!inCompactMode) {
        config.writeEntry("ShowHistory", m_historyVisible);
        config.writeEntry("ShowFunctions", m_functionsVisible);
        config.writeEntry("ShowVariables", m_numeralsVisible);
    }
    else {
        config.writeEntry("ShowHistory", m_wasHistoryShown);
        config.writeEntry("ShowFunctions", m_wasFnShown);
        config.writeEntry("ShowVariables", m_wasVarShown);
    }


    config = KGlobal::config()->group("Functions");

    FunctionModel *manager = FunctionModel::instance();

    QStringList userFunctions = manager->functionList(FunctionModel::UserDefined);
    QStringList saveList;

    foreach(QString functionName, userFunctions)
    {
        UnaryFunction *fn = dynamic_cast<UnaryFunction *>(manager->function(functionName)->userFn->fn);
        QString var = manager->function(functionName)->userFn->varName;
        QString expr = fn->operand()->infixString();

        saveList += QString("set %1(%2) = %3").arg(functionName).arg(var).arg(expr);
    }

    config.writeEntry("FunctionList", saveList);

    config.sync();
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

    a = ac->addAction("select_edit", m_declarativeView, SLOT(setFocus()));
    a->setIcon(KIcon("goto"));
    a->setText(i18n("Select Editor"));
    a->setShortcut(Qt::Key_F6);
}

QAction *MainWindow::action(const char *key) const
{
    return actionCollection()->action(key);
}

void MainWindow::slotToggleMenuBar()
{
    menuBar()->setShown(menuBar()->isHidden());
}

void MainWindow::slotToggleFunctionList()
{
    bool show = action<KToggleAction>("toggleFunctionList")->isChecked();
    setFunctionsVisible(show);

    if(m_compactMode) {
        action<KToggleAction>("toggleHistoryList")->setChecked(true);
        slotToggleHistoryList();
    }

    action<KToggleAction>("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotToggleVariableList()
{
    bool show = action<KToggleAction>("toggleVariableList")->isChecked();
    setNumeralsVisible(show);

    if(m_compactMode) {
        action<KToggleAction>("toggleHistoryList")->setChecked(true);
        slotToggleHistoryList();
    }

    action<KToggleAction>("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotToggleHistoryList()
{
    bool show = action<KToggleAction>("toggleHistoryList")->isChecked();
    setHistoryVisible(show);

    action<KToggleAction>("toggleCompactMode")->setChecked(false);
}

void MainWindow::slotToggleCompactMode()
{
    if(action<KToggleAction>("toggleCompactMode")->isChecked()) {
        m_wasFnShown = m_functionsVisible;
        m_wasVarShown = m_numeralsVisible;
        m_wasHistoryShown = m_historyVisible;
        m_compactMode = true;

        setFunctionsVisible(false);
        setNumeralsVisible(false);
        setHistoryVisible(false);

        action<KToggleAction>("toggleFunctionList")->setChecked(false);
        action<KToggleAction>("toggleVariableList")->setChecked(false);
        action<KToggleAction>("toggleHistoryList")->setChecked(false);

        m_oldSize = size();
        m_newSize = QSize(0, 0);
        QTimer::singleShot(0, this, SLOT(slotUpdateSize()));
    }
    else {
        setFunctionsVisible(m_wasFnShown);
        setNumeralsVisible(m_wasVarShown);
        setHistoryVisible(m_wasHistoryShown);
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
            m_resultItemModel->addMessage(i18n("Marker %1 is not set", stackStr));
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
    NumeralModel::instance()->slotRedrawItems();

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

#include "mainwindow.moc"

// vim: set et ts=8 sw=4:
