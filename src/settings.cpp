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

#include <QDebug>

Settings::Settings(QObject* parent) : QObject(parent), m_settingscore(SettingsCore::instance())
{
    connect(m_settingscore, SIGNAL(mathematicalSidebarWidthChanged()), this, SIGNAL(mathematicalSidebarWidthChanged()));
}

Settings::~Settings()
{
}

int Settings::mathematicalSidebarWidth()
{
    return m_settingscore->mathematicalSidebarWidth();
}

void Settings::setMathematicalSidebarWidth(int width)
{
    m_settingscore->setMathematicalSidebarWidth(width);
}
