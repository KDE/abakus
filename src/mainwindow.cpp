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

#include "functionmodel.h"
#include "numeralmodel.h"
#include "result.h"
#include "resultmodel.h"
#include "rpnmuncher.h"

#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QToolTip>

#include <KAction>
#include <KActionCollection>
#include <KConfig>
#include <KCmdLineArgs>
#include <KDebug>
#include <kdeclarative.h>
#include <KHelpMenu>
#include <KMenu>
#include <KShortcutsDialog>
#include <KStandardDirs>

MainWindow::MainWindow() :
    m_helpMenu(0),
    m_actionCollection(new KActionCollection(this)),
    m_resultItemModel (new ResultModel(this)),
    m_size(QSize(600, 220)),
    m_mathematicalSidebarActiveTab("numerals"),
    m_mathematicalSidebarWidth(200),
    m_mathematicalSidebarVisible(true),
    m_wasMathematicalSidebarShown(true),
    m_compactMode(false),
    m_rpnMode(false),
    m_historyLimit(500),
    m_insert(false)
{
    setObjectName("abakusMainWindow");
    
    loadConfig();
    setAutoSaveSettings();

    m_visibleHistoryItemIndices.clear();

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
    // show the window background
    m_declarativeView->setStyleSheet("background:transparent;");
    m_declarativeView->setAttribute(Qt::WA_TranslucentBackground);
    m_declarativeView->setWindowFlags(Qt::FramelessWindowHint);

    // initialize kdeclarative
    KDeclarative kDeclarative;
    kDeclarative.setDeclarativeEngine(m_declarativeView->engine());
    kDeclarative.initialize();
    kDeclarative.setupBindings();
    
    QString filePath = KStandardDirs::locate("appdata", "qml/main.qml");
    m_declarativeView->setSource(QUrl::fromLocalFile(filePath));
    
    setCentralWidget(m_declarativeView);
    
    m_declarativeView->setFocus();
    
    setupLayout();

//    m_dcopInterface = new AbakusIface();
}

bool MainWindow::inRPNMode() const
{
    return m_rpnMode;
}

bool MainWindow::queryExit()
{
    saveConfig();
    m_resultItemModel->clear();
    return KMainWindow::queryExit();
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
                m_resultItemModel->addMessage(str, resultVal);
            break;

            default:
                resultVal = Result::lastResult()->message();
                m_resultItemModel->addMessage(str, resultVal);
        }
    }
    
    // set result in expression text edit if in compact mode
    if(m_compactMode) {
        emit setEditorText(resultVal);
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
            emit setEditorText("ans " + str + " ");
        }
    }
}

QString MainWindow::getTag(const int &index)
{
    return m_resultItemModel->data(m_resultItemModel->index(index), ResultModel::TagRole).toString();
}

void MainWindow::showHelpMenu(int xPosition, int yPosition)
{
    m_helpMenu->popup(m_declarativeView->mapToGlobal(QPoint(xPosition, yPosition)));
}

void MainWindow::configureShortcuts()
{
    KShortcutsDialog::configure(m_actionCollection, KShortcutsEditor::LetterShortcutsDisallowed, this);
}

void MainWindow::showToolTip(const int xPosition, const int yPosition, const QString& toolTipText)
{
    QToolTip::showText(mapToGlobal(QPoint(xPosition, yPosition)), toolTipText);
}

void MainWindow::hideToolTip()
{
    QToolTip::hideText();
}

void MainWindow::setDegrees()
{
    setTrigMode(Abakus::Degrees);
    emit trigModeChanged((int)Abakus::Degrees);
}

void MainWindow::setRadians()
{
    setTrigMode(Abakus::Radians);
    emit trigModeChanged((int)Abakus::Radians);
}

void MainWindow::clearHistory()
{
    m_resultItemModel->clear();
}

void MainWindow::historyPrevious()
{
    emit setEditorText(m_resultItemModel->previousExpression());
}

void MainWindow::historyNext()
{
    emit setEditorText(m_resultItemModel->nextExpression());
}

void MainWindow::setMathematicalSidebarVisible(const bool& visible)
{
    m_mathematicalSidebarVisible = visible;
    emit mathematicalSidebarVisibleChanged(visible);
}

void MainWindow::removeNumeral(const QString& name)
{
    NumeralModel::instance()->removeValue(name);
}

void MainWindow::removeFunction(const QString& name)
{
    FunctionModel::instance()->removeFunction(name);
}

int MainWindow::getVisibleHistoryItemIndex(int listIndex)
{
    return (listIndex >= 0 && listIndex < m_visibleHistoryItemIndices.count()) ? m_visibleHistoryItemIndices[listIndex] : -1;
}

void MainWindow::addVisibleHistoryItemIndex(int itemIndex)
{
    m_visibleHistoryItemIndices.append(itemIndex);
}

void MainWindow::removeVisibleHistoryItemIndex(int itemIndex)
{
    m_visibleHistoryItemIndices.removeOne(itemIndex);
}

void MainWindow::applySettings()
{
    emit trigModeChanged((int)Abakus::m_trigMode);
    emit precisionChanged(Abakus::m_prec);
    emit setMathematicalActiveTab(m_mathematicalSidebarActiveTab);
    emit setMathematicalSidebarWidth(m_mathematicalSidebarWidth);
    
    if(m_compactMode)
    {
        slotToggleCompactMode(false);
    }
    else
    {
        emit mathematicalSidebarVisibleChanged(m_mathematicalSidebarVisible);
    }
}

void MainWindow::slotUpdateSize()
{
    if(m_compactMode)
    {
        resize(width(), minimumSize().height());
    }
    else
    {
        resize(m_size);
    }
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
    }
    else {
        setTrigMode(Abakus::Radians);
    }

    m_rpnMode = config.readEntry("Use RPN Mode", false);

    int precision = config.readEntry("Decimal Precision", -1);
    if(precision < -1 || precision > 75)
    {
        precision = -1;
    }

    Abakus::m_prec = precision;
    
    m_historyLimit = config.readEntry("History Limit", 500);
    

    config = KGlobal::config()->group("GUI");

    m_mathematicalSidebarVisible = config.readEntry("ShowMathematicalSidebar", true);
    m_mathematicalSidebarActiveTab = config.readEntry("MathematicalSidebarActiveTab", "numerals");
    m_mathematicalSidebarWidth = config.readEntry("MathematicalSidebarWidth", 200);

    m_compactMode = config.readEntry("InCompactMode", false);
    
    m_size = config.readEntry("Size", QSize(600, 220));
    slotUpdateSize();


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
    
    
    config = KGlobal::config()->group("Functions");

    QStringList fnList = config.readEntry("FunctionList", QStringList());
    foreach(QString str, fnList)
    {
        QByteArray strValue = str.toLatin1();
        parseString(strValue.data()); // Run the function definitions through the parser
    }
    
    
    config = KGlobal::config()->group("History");
    QStringList historyKeys = config.keyList();
    QStringList historyValues;
    ResultModelItem* resultModelItem;
    
    for(int i = historyKeys.count() - 1; i >= 0; --i)
    {
        historyValues = config.readEntry(historyKeys[i], QStringList());
        if(historyValues[2].toInt() == ResultModelItem::Result)
        {
            resultModelItem = new ResultModelItem(historyValues[0], Abakus::number_t(historyValues[1].toLatin1()));
        }
        else
        {
            resultModelItem = new ResultModelItem(historyValues[0], historyValues[1]);
        }
        m_resultItemModel->addResultModelItem(resultModelItem);
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
    config.writeEntry("History Limit", m_historyLimit);

    
    config = KGlobal::config()->group("GUI");

    config.writeEntry("InCompactMode", m_compactMode);

    if(!m_compactMode)
    {
        config.writeEntry("ShowMathematicalSidebar", m_mathematicalSidebarVisible);
        m_size = size();
    }
    else {
        config.writeEntry("ShowMathematicalSidebar", m_wasMathematicalSidebarShown);
    }
    
    config.writeEntry("MathematicalSidebarActiveTab", m_mathematicalSidebarActiveTab);
    config.writeEntry("MathematicalSidebarWidth", m_mathematicalSidebarWidth);
    
    config.writeEntry("Size", m_size);
    
    
    config = KGlobal::config()->group("Variables");
    
    QStringList saveList;
    QStringList values = NumeralModel::instance()->valueNames();
    QStringList::ConstIterator it = values.begin();
    
    // Set precision to max for most accuracy
    Abakus::m_prec = 75;
    
    for(; it != values.end(); ++it) {
        if(NumeralModel::instance()->isValueReadOnly(*it))
            continue;
        
        saveList += QString("%1=%2")
        .arg(*it)
        .arg(NumeralModel::instance()->value(*it).toString());
    }
    
    config.writeEntry("Saved Variables", saveList);


    config = KGlobal::config()->group("Functions");

    FunctionModel *manager = FunctionModel::instance();

    saveList.clear();
    QStringList userFunctions = manager->functionList(FunctionModel::UserDefined);

    foreach(QString functionName, userFunctions)
    {
        UnaryFunction *fn = dynamic_cast<UnaryFunction *>(manager->function(functionName)->userFn->fn);
        QString var = manager->function(functionName)->userFn->varName;
        QString expr = fn->operand()->infixString();

        saveList += QString("set %1(%2) = %3").arg(functionName).arg(var).arg(expr);
    }

    config.writeEntry("FunctionList", saveList);
    
    
    config = KGlobal::config()->group("History");
    config.deleteGroup();
    
    QList<ResultModelItem*> historyList = m_resultItemModel->resultList();
    int historyListLastIndex = historyList.count() - 1;
    int fieldWidth = QString("%1").arg(m_historyLimit).length();
    
    for(int i = historyList.count() - 1, j = 0; i >= 0 && j < m_historyLimit; --i, ++j)
    {
        saveList.clear();
        saveList << historyList[i]->expression();
        saveList << historyList[i]->result();
        saveList << QString("%1").arg(historyList[i]->type());
        config.writeEntry(QString("%1").arg(j, fieldWidth, 10, QLatin1Char('0')), saveList);
    }
    
    config.sync();
}

void MainWindow::setupLayout()
{
    m_actionCollection->addAssociatedWidget(this);

    KStandardAction::quit(this, SLOT(close()), m_actionCollection);
    KStandardAction::keyBindings(this, SLOT(configureShortcuts()), m_actionCollection);

    QAction *a = m_actionCollection->addAction("setDegreesMode", this, SLOT(setDegrees()));
    a->setText(i18n("&Degrees"));
    a->setShortcut(Qt::SHIFT | Qt::ALT | Qt::Key_D);
    a->setChecked(trigMode() == Abakus::Degrees);

    a = m_actionCollection->addAction("setRadiansMode", this, SLOT(setRadians()));
    a->setText(i18n("&Radians"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_R);
    a->setChecked(trigMode() == Abakus::Radians);

    a = m_actionCollection->addAction("toggleMathematicalSidebar", this, SLOT(slotToggleMathematicalSidebar()));
    a->setText(i18n("Show &Mathematical Sidebar"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_M);
    a->setChecked(true);

    a = m_actionCollection->addAction("toggleCompactMode", this, SLOT(slotToggleCompactMode()));
    a->setText(i18n("Activate &Compact Mode"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_C);
    a->setChecked(false);

    a = m_actionCollection->addAction("toggleExpressionMode", this, SLOT(slotToggleExpressionMode()));
    a->setText(i18n("Use R&PN Mode"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_P);
    a->setChecked(false);

    a = m_actionCollection->addAction("clearHistory", this, SLOT(clearHistory()));
    a->setText(i18n("Clear &History"));
    a->setIcon(KIcon("edit-clear-list"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_L);

    a = m_actionCollection->addAction("select_edit", m_declarativeView, SLOT(setFocus()));
    a->setText(i18n("Select Editor"));
    a->setShortcut(Qt::Key_F6);
    
    KHelpMenu* helpMenu = new KHelpMenu(this, KCmdLineArgs::aboutData(), true, m_actionCollection);
    m_helpMenu = helpMenu->menu();
}

void MainWindow::mathematicalSidebarActiveTabChanged(const QString& activeTab)
{
    m_mathematicalSidebarActiveTab = activeTab;
}

void MainWindow::mathematicalSidebarWidthChanged(const int newWidth)
{
    m_mathematicalSidebarWidth = newWidth;
}

void MainWindow::slotToggleMathematicalSidebar()
{
    bool show = !m_mathematicalSidebarVisible;
    setMathematicalSidebarVisible(show);

    if(m_compactMode) {
        m_wasMathematicalSidebarShown = m_mathematicalSidebarVisible;
        slotToggleCompactMode();
    }
}

void MainWindow::setCompactMode(bool newMode)
{
    if(m_compactMode != newMode)
    {
        slotToggleCompactMode();
    }
}

void MainWindow::slotToggleCompactMode(bool toggle)
{
    if(toggle) m_compactMode = !m_compactMode;

    if(m_compactMode)
    {
        m_wasMathematicalSidebarShown = m_mathematicalSidebarVisible;
        setMathematicalSidebarVisible(false);
        
        if(toggle) m_size = size();
    }
    else {
        setMathematicalSidebarVisible(m_wasMathematicalSidebarShown);
    }
    
    slotUpdateSize();
    emit compactModeChanged(m_compactMode);
}

void MainWindow::slotToggleExpressionMode()
{
    //TODO
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
            m_resultItemModel->addMessage(str, i18n("Marker %1 is not set", stackStr));
            return QString();
        }

        str.replace(pos, stackStr.length(), value.toString());
    }

    return str;
}

void MainWindow::setPrecision(int precision)
{
    Abakus::m_prec = precision;
    redrawResults();
    
    emit precisionChanged(Abakus::m_prec);
}

void MainWindow::redrawResults()
{
    m_resultItemModel->slotRedrawItems();
    NumeralModel::instance()->slotRedrawItems();
}

#include "mainwindow.moc"

// vim: set et ts=8 sw=4:
