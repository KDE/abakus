/*
    settingscore.cpp - access the settings through QML
    Copyright (C) 2013  Mathias Kraus <mathias@m-hias.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "settingscore.h"

#include "function.h"
#include "functionmodel.h"
#include "numeralmodel.h"
#include "node.h"
#include "resultmodel.h"

#include <KConfig>
#include <KConfigGroup>
#include <KGlobal>

SettingsCore* SettingsCore::m_instance = 0;

SettingsCore* SettingsCore::instance()
{
    if(!m_instance)
    {
        m_instance = new SettingsCore();
    }
    
    return m_instance;
}

SettingsCore::SettingsCore() :
    m_windowSize(QSize(600, 220)),
    m_mathematicalSidebarActiveView("numerals"),
    m_mathematicalSidebarWidth(200),
    m_mathematicalSidebarVisible(true),
    m_compactMode(false),
    m_historyLimit(500)
{
}

SettingsCore::~SettingsCore()
{
}

void SettingsCore::loadSettings()
{
    KConfigGroup config = KGlobal::config()->group("Settings");
    
    QString mode = config.readEntry("Trigonometric mode", "Degrees");
    if(mode == "Degrees") {
        setTrigMode(Abakus::Degrees);
    }
    else {
        setTrigMode(Abakus::Radians);
    }
    
    int precision = config.readEntry("Decimal Precision", -1);
    if(precision < -1 || precision > 75)
    {
        precision = -1;
    }
    
    Abakus::m_prec = precision;
    
    m_historyLimit = config.readEntry("History Limit", 500);
    
    
    config = KGlobal::config()->group("GUI");
    
    m_mathematicalSidebarVisible = config.readEntry("ShowMathematicalSidebar", true);
    m_mathematicalSidebarActiveView = config.readEntry("MathematicalSidebarActiveTab", "numerals");
    m_mathematicalSidebarWidth = config.readEntry("MathematicalSidebarWidth", 200);
    
    m_compactMode = config.readEntry("InCompactMode", false);
    
    m_windowSize = config.readEntry("Size", QSize(600, 220));
    
    
    config = KGlobal::config()->group("Variables");
    QStringList variableKeys = config.keyList();
    QStringList variableValues;
    Abakus::Number number;
    
    for(int i = 0; i < variableKeys.count(); ++i)
    {
        variableValues = config.readEntry(variableKeys[i], QStringList());
        if(variableValues.count() < 3) continue;
        
        number = Abakus::Number(variableValues[1].toLatin1());
        number.setNumeralSystem((Abakus::NumeralSystem) variableValues[2].toInt());
        
        NumeralModel::instance()->setValue(variableValues[0], number);
    }
    
    
    config = KGlobal::config()->group("Functions");
    QStringList functionKeys = config.keyList();
    QString functionValue;
    
    for(int i = 0; i < functionKeys.count(); ++i)
    {
        functionValue = config.readEntry(functionKeys[i], QString());
        if(functionValue.isEmpty()) continue;
                                      
        QByteArray strValue = "set " + functionValue.toLatin1();
        parseString(strValue.data()); // Run the function definitions through the parser
    }
    
    
    config = KGlobal::config()->group("History");
    QStringList historyKeys = config.keyList();
    QStringList historyValues;
    ResultModel* resultModel = ResultModel::instance();
    ResultModelItem* resultModelItem;
    
    for(int i = historyKeys.count() - 1; i >= 0; --i)
    {
        historyValues = config.readEntry(historyKeys[i], QStringList());
        if(historyValues.count() < 1) continue;
        
        if(historyValues[0].toInt() == ResultModelItem::Result)
        {
            if(historyValues.count() < 4) continue;
            number = Abakus::Number(historyValues[2].toLatin1());
            number.setNumeralSystem((Abakus::NumeralSystem) historyValues[3].toInt());
            resultModelItem = new ResultModelItem(historyValues[1], number);
        }
        else
        {
            if(historyValues.count() < 3) continue;
            resultModelItem = new ResultModelItem(historyValues[1], historyValues[2]);
        }
        resultModel->addResultModelItem(resultModelItem);
    }
}

void SettingsCore::saveSettings()
{
    KConfigGroup config = KGlobal::config()->group("Settings");
    
    config.writeEntry("Trigonometric mode",
                      trigMode() == Abakus::Degrees
                      ? "Degrees"
                      : "Radians");
    
    config.writeEntry("Decimal Precision", Abakus::m_prec);
    config.writeEntry("History Limit", m_historyLimit);
    
    
    config = KGlobal::config()->group("GUI");
    
    config.writeEntry("InCompactMode", m_compactMode);
    
    config.writeEntry("ShowMathematicalSidebar", m_mathematicalSidebarVisible);
    
    config.writeEntry("MathematicalSidebarActiveTab", m_mathematicalSidebarActiveView);
    config.writeEntry("MathematicalSidebarWidth", m_mathematicalSidebarWidth);
    
    config.writeEntry("Size", m_windowSize);
    
    
    config = KGlobal::config()->group("Variables");
    config.deleteGroup();
    
    QStringList saveList;
    QStringList::ConstIterator it;
    int fieldWidth;
    int numberOfEntries;
    
    // Set precision to max for most accuracy
    Abakus::m_prec = 75;
    
    
    QStringList values = NumeralModel::instance()->valueNames();
    for(it = values.begin(), numberOfEntries = 0; it != values.end(); ++it) {
        if(NumeralModel::instance()->isValueReadOnly(*it))
            continue;
        
        ++numberOfEntries;
    }
    fieldWidth = QString("%1").arg(numberOfEntries).length();
    int j = 0;
    for(it = values.begin(); it != values.end(); ++it) {
        if(NumeralModel::instance()->isValueReadOnly(*it))
            continue;
        
        saveList.clear();
        saveList << *it;
        saveList << NumeralModel::instance()->value(*it).toString(Abakus::DEC);
        saveList << QString("%1").arg(NumeralModel::instance()->value(*it).numeralSystem());
        config.writeEntry(QString("%1").arg(j, fieldWidth, 10, QLatin1Char('0')), saveList);
        ++j;
    }
    
    
    config = KGlobal::config()->group("Functions");
    config.deleteGroup();
    
    FunctionModel *manager = FunctionModel::instance();
    UnaryFunction *fn;
    QString variable;
    QString expression;
    QString saveString;
    QStringList userFunctions = manager->functionList(FunctionModel::UserDefined);
    fieldWidth = QString("%1").arg(userFunctions.count()).length();
    j = 0;
    foreach(QString functionName, userFunctions)
    {
        fn = dynamic_cast<UnaryFunction *>(manager->function(functionName)->userFn->fn);
        variable = manager->function(functionName)->userFn->varName;
        expression = fn->operand()->infixString();
        
        saveString = QString("%1(%2) = %3").arg(functionName).arg(variable).arg(expression);
        config.writeEntry(QString("%1").arg(j, fieldWidth, 10, QLatin1Char('0')), saveString);
        ++j;
    }
    
    
    config = KGlobal::config()->group("History");
    config.deleteGroup();
    
    QList<ResultModelItem*> historyList = ResultModel::instance()->resultList();
    int historyListLastIndex = historyList.count() - 1;
    fieldWidth = QString("%1").arg(m_historyLimit).length();
    ResultModelItem::ResultItemType resultType;
    
    for(int i = historyList.count() - 1, j = 0; i >= 0 && j < m_historyLimit; --i, ++j)
    {
        saveList.clear();
        resultType = historyList[i]->type();
        saveList << QString("%1").arg(resultType);
        switch(resultType)
        {
            case ResultModelItem::Result:
                saveList << historyList[i]->expression();
                saveList << historyList[i]->resultValue().toString(Abakus::DEC);
                saveList << QString("%1").arg(historyList[i]->resultValue().numeralSystem());
                break;
            case ResultModelItem::Message:
            default:
                saveList << historyList[i]->expression();
                saveList << historyList[i]->result();
        }
        config.writeEntry(QString("%1").arg(j, fieldWidth, 10, QLatin1Char('0')), saveList);
    }
    
    config.sync();
}

int SettingsCore::precision()
{
    return Abakus::m_prec;
}

void SettingsCore::setPrecision(int precision)
{
    if(precision != Abakus::m_prec)
    {
        Abakus::m_prec = precision;
        emit precisionChanged();
    }
}

Abakus::TrigMode SettingsCore::trigMode()
{
    return Abakus::m_trigMode;
}

void SettingsCore::setTrigMode(Abakus::TrigMode trigMode)
{
    if(trigMode != Abakus::m_trigMode)
    {
        Abakus::m_trigMode = trigMode;
        emit trigModeChanged();
    }
}

bool SettingsCore::compactMode()
{
    return m_compactMode;
}

void SettingsCore::setCompactMode(bool compactMode)
{
    if(compactMode != m_compactMode)
    {
        m_compactMode = compactMode;
        emit compactModeChanged();
    }
}

QSize SettingsCore::windowSize()
{
    return m_windowSize;
}

void SettingsCore::setWindowSize(const QSize& windowSize)
{
    if(windowSize != m_windowSize)
    {
        m_windowSize = windowSize;
        emit windowSizeChanged();
    }
}

QString SettingsCore::mathematicalSidebarActiveView()
{
    return m_mathematicalSidebarActiveView;
}

void SettingsCore::setMathematicalSidebarActiveView(const QString& activeView)
{
    if(activeView != m_mathematicalSidebarActiveView)
    {
        m_mathematicalSidebarActiveView = activeView;
        emit mathematicalSidebarActiveViewChanged();
    }
}

int SettingsCore::mathematicalSidebarWidth()
{
    return m_mathematicalSidebarWidth;
}

void SettingsCore::setMathematicalSidebarWidth(int width)
{
    if(width != m_mathematicalSidebarWidth)
    {
        m_mathematicalSidebarWidth = width;
        emit mathematicalSidebarWidthChanged();
    }
}

bool SettingsCore::mathematicalSidebarVisible()
{
    return m_mathematicalSidebarVisible;
}

void SettingsCore::setMathematicalSidebarVisible(bool visible)
{
    if(visible != m_mathematicalSidebarVisible)
    {
        m_mathematicalSidebarVisible = visible;
        emit mathematicalSidebarVisibleChanged();
    }
}
