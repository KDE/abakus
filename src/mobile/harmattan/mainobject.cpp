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
#include "mainobject.h"

#include "functionmodel.h"
#include "numeralmodel.h"
#include "result.h"
#include "resultmodel.h"
#include "settings.h"

#include <qdeclarative.h>
#include <QDeclarativeContext>
#include <QDeclarativeView>
#include <QMenu>
#include <QToolTip>

#define i18n tr

MainObject::MainObject() :
    m_resultItemModel (ResultModel::instance()),
    m_declarativeContext(NULL),
    m_settingscore(SettingsCore::instance()),
    m_insert(false)
{
    m_settingscore->loadSettings();
    
    m_visibleHistoryItemIndices.clear();
    
    connect(m_settingscore->instance(), SIGNAL(precisionChanged()), this, SLOT(slotRedrawResults()));
    
    qmlRegisterType<Settings>("abakus", 1, 0, "Settings");
}

MainObject::~MainObject()
{
    m_settingscore->saveSettings();
    m_resultItemModel->clear();
}

void MainObject::setView(QDeclarativeView* declarativeView)
{
    m_declarativeContext = declarativeView->rootContext();
    m_declarativeContext->setContextProperty("mainWindow", this);
    m_declarativeContext->setContextProperty("resultModel", m_resultItemModel);
    m_declarativeContext->setContextProperty("numeralModel", NumeralModel::instance());
    m_declarativeContext->setContextProperty("functionModel", FunctionModel::instance());
}

void MainObject::slotEvaluate(const QString &expression)
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

void MainObject::slotTextChanged(const QString &str)
{
    if(str.length() == 1 && m_insert) {
        m_insert = false;

        if(QRegExp("^[-+*/^]").indexIn(str) != -1) {
            emit setEditorText("ans " + str + " ");
        }
    }
}

QString MainObject::getTag(const int &index)
{
    return m_resultItemModel->data(m_resultItemModel->index(index), ResultModel::TagRole).toString();
}

void MainObject::clearHistory()
{
    m_resultItemModel->clear();
}

void MainObject::historyPrevious()
{
    emit setEditorText(m_resultItemModel->previousExpression());
}

void MainObject::historyNext()
{
    emit setEditorText(m_resultItemModel->nextExpression());
}

void MainObject::removeNumeral(const QString& name)
{
    NumeralModel::instance()->removeValue(name);
}

void MainObject::removeFunction(const QString& name)
{
    FunctionModel::instance()->removeFunction(name);
}

int MainObject::getVisibleHistoryItemIndex(int listIndex)
{
    return (listIndex >= 0 && listIndex < m_visibleHistoryItemIndices.count()) ? m_visibleHistoryItemIndices[listIndex] : -1;
}

void MainObject::addVisibleHistoryItemIndex(int itemIndex)
{
    m_visibleHistoryItemIndices.append(itemIndex);
}

void MainObject::removeVisibleHistoryItemIndex(int itemIndex)
{
    m_visibleHistoryItemIndices.removeOne(itemIndex);
}

int MainObject::getParenthesesLevel(const QString &str)
{
    int level = 0;

    for(int i = 0; i < str.length(); ++i)
        if(str[i] == '(')
            ++level;
        else if(str[i] == ')')
            --level;

    return level;
}

QString MainObject::interpolateExpression(const QString &text)
{
    QString str(text);
    QRegExp stackRE("\\$\\d+");
    int pos;

    while((pos = stackRE.indexIn(str)) != -1) {
        QString stackStr = stackRE.cap();
        Abakus::Number value;
        unsigned numPos = stackStr.mid(1).toUInt();

        if(!m_resultItemModel->stackValue(numPos, value)) {
            m_resultItemModel->addMessage(str, i18n("Marker %1 is not set").arg(stackStr));
            return QString();
        }

        str.replace(pos, stackStr.length(), value.toString());
    }

    return str;
}

void MainObject::slotRedrawResults()
{
    m_resultItemModel->slotRedrawItems();
    NumeralModel::instance()->slotRedrawItems();
}
