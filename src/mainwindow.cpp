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
#include "settings.h"

#include <qdeclarative.h>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QToolTip>

#include <KAction>
#include <KActionCollection>
#include <KCmdLineArgs>
#include <kdeclarative.h>
#include <KHelpMenu>
#include <KMenu>
#include <KShortcutsDialog>
#include <KStandardDirs>

MainWindow::MainWindow() :
    m_helpMenu(0),
    m_actionCollection(new KActionCollection(this)),
    m_resultItemModel (ResultModel::instance()),
    m_settingscore(SettingsCore::instance()),
    m_insert(false)
{
    setObjectName("abakusMainWindow");
    
    m_settingscore->loadSettings();
    slotUpdateSize();
    setAutoSaveSettings();

    m_visibleHistoryItemIndices.clear();
    
    connect(m_settingscore->instance(), SIGNAL(precisionChanged()), this, SLOT(slotRedrawResults()));
    connect(m_settingscore->instance(), SIGNAL(compactModeChanged()), this, SLOT(slotUpdateSize()));
    
    qmlRegisterType<Settings>("abakus", 1, 0, "Settings");

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
    
    setupShortcuts();
    
    KHelpMenu* helpMenu = new KHelpMenu(this, KCmdLineArgs::aboutData(), true, m_actionCollection);
    m_helpMenu = helpMenu->menu();

//    m_dcopInterface = new AbakusIface();
}

bool MainWindow::queryExit()
{
    QSize windowSize = size();
    if(m_settingscore->compactMode())
    {
        windowSize.setHeight(m_settingscore->windowSize().height());
    }
    m_settingscore->setWindowSize(windowSize);
    m_settingscore->saveSettings();
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

    // Check to see if it's just a function name, if so, add (ans).
    if(FunctionModel::instance()->isFunction(str))
        str += " ans";

    // Add right parentheses as needed to balance out the expression.
    int parenLevel = getParenthesesLevel(str);
    for(int i = 0; i < parenLevel; ++i)
        str += ')';

    QByteArray cStr = str.toLatin1();
    Abakus::Number result = parseString(cStr.data());

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
    
    // set result in expression text edit if in compact mode
    if(m_settingscore->compactMode()) {
        emit setEditorText(resultVal);
    }
}

void MainWindow::slotTextChanged(const QString &str)
{
    if(str.length() == 1 && m_insert) {
        m_insert = false;

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

void MainWindow::slotSetDegrees()
{
    m_settingscore->setTrigMode(Abakus::Degrees);
}

void MainWindow::slotSetRadians()
{
    m_settingscore->setTrigMode(Abakus::Radians);
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

void MainWindow::slotUpdateSize()
{
    if(m_settingscore->compactMode())
    {
        resize(width(), minimumSize().height());
    }
    else
    {
        resize(m_settingscore->windowSize());
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

void MainWindow::setupShortcuts()
{
    m_actionCollection->addAssociatedWidget(this);

    KStandardAction::quit(this, SLOT(close()), m_actionCollection);
    KStandardAction::keyBindings(this, SLOT(configureShortcuts()), m_actionCollection);

    QAction *a = m_actionCollection->addAction("setDegreesMode", this, SLOT(slotSetDegrees()));
    a->setText(i18n("&Degrees"));
    a->setShortcut(Qt::SHIFT | Qt::ALT | Qt::Key_D);

    a = m_actionCollection->addAction("setRadiansMode", this, SLOT(slotSetRadians()));
    a->setText(i18n("&Radians"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_R);

    a = m_actionCollection->addAction("toggleMathematicalSidebar", this, SLOT(slotToggleMathematicalSidebar()));
    a->setText(i18n("Show &Mathematical Sidebar"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_M);

    a = m_actionCollection->addAction("toggleCompactMode", this, SLOT(slotToggleCompactMode()));
    a->setText(i18n("Activate &Compact Mode"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_C);

    a = m_actionCollection->addAction("clearHistory", this, SLOT(clearHistory()));
    a->setText(i18n("Clear &History"));
    a->setIcon(KIcon("edit-clear-list"));
    a->setShortcut(Qt::SHIFT + Qt::ALT + Qt::Key_L);

    a = m_actionCollection->addAction("select_edit", this, SIGNAL(setFocusToEditor()));
    a->setText(i18n("Select Editor"));
    a->setShortcut(Qt::Key_F6);
}

void MainWindow::slotToggleMathematicalSidebar()
{
    m_settingscore->setMathematicalSidebarVisible(!m_settingscore->mathematicalSidebarVisible());
}

void MainWindow::slotToggleCompactMode()
{
    m_settingscore->setCompactMode(!m_settingscore->compactMode());
}

QString MainWindow::interpolateExpression(const QString &text)
{
    QString str(text);
    QRegExp stackRE("\\$\\d+");
    int pos;

    while((pos = stackRE.indexIn(str)) != -1) {
        QString stackStr = stackRE.cap();
        Abakus::Number value;
        unsigned numPos = stackStr.mid(1).toUInt();

        if(!m_resultItemModel->stackValue(numPos, value)) {
            m_resultItemModel->addMessage(str, i18n("Marker %1 is not set", stackStr));
            return QString();
        }

        str.replace(pos, stackStr.length(), value.toString());
    }

    return str;
}

void MainWindow::slotRedrawResults()
{
    m_resultItemModel->slotRedrawItems();
    NumeralModel::instance()->slotRedrawItems();
}

#include "mainwindow.moc"

// vim: set et ts=8 sw=4:
