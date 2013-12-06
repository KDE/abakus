/*
    settings.cpp - access the settings through QML
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


#include "settings.h"

Settings::Settings(QQuickItem* parent) : QQuickItem(parent), m_settingscore(SettingsCore::instance())
{
    connect(m_settingscore, SIGNAL(precisionChanged()), this, SIGNAL(precisionChanged()));
    connect(m_settingscore, SIGNAL(trigModeChanged()), this, SIGNAL(trigModeChanged()));
    connect(m_settingscore, SIGNAL(compactModeChanged()), this, SIGNAL(compactModeChanged()));
    connect(m_settingscore, SIGNAL(mathematicalSidebarActiveViewChanged()), this, SIGNAL(mathematicalSidebarActiveViewChanged()));
    connect(m_settingscore, SIGNAL(mathematicalSidebarWidthChanged()), this, SIGNAL(mathematicalSidebarWidthChanged()));
    connect(m_settingscore, SIGNAL(mathematicalSidebarVisibleChanged()), this, SIGNAL(mathematicalSidebarVisibleChanged()));
}

Settings::~Settings()
{
}

int Settings::precision()
{
    return m_settingscore->precision();
}

void Settings::setPrecision(int precision)
{
    m_settingscore->setPrecision(precision);
}

Settings::TrigMode Settings::trigMode()
{
    return static_cast <Settings::TrigMode> (m_settingscore->trigMode());
}

void Settings::setTrigMode(Settings::TrigMode trigMode)
{
    m_settingscore->setTrigMode(static_cast <Abakus::TrigMode> (trigMode));
}

bool Settings::compactMode()
{
    return m_settingscore->compactMode();
}

void Settings::setCompactMode(bool compactMode)
{
    m_settingscore->setCompactMode(compactMode);
}

QString Settings::mathematicalSidebarActiveView()
{
    return m_settingscore->mathematicalSidebarActiveView();
}

void Settings::setMathematicalSidebarActiveView(const QString& activeView)
{
    m_settingscore->setMathematicalSidebarActiveView(activeView);
}

int Settings::mathematicalSidebarWidth()
{
    return m_settingscore->mathematicalSidebarWidth();
}

void Settings::setMathematicalSidebarWidth(int width)
{
    m_settingscore->setMathematicalSidebarWidth(width);
}

bool Settings::mathematicalSidebarVisible()
{
    return m_settingscore->mathematicalSidebarVisible();
}

void Settings::setMathematicalSidebarVisible(bool visible)
{
    m_settingscore->setMathematicalSidebarVisible(visible);
}
