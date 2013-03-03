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


#ifndef SETTINGS_H
#define SETTINGS_H

#include "settingscore.h"

#include <QObject>

class Settings : public QObject
{

    Q_OBJECT
    Q_PROPERTY(int mathematicalSidebarWidth READ mathematicalSidebarWidth WRITE setMathematicalSidebarWidth NOTIFY mathematicalSidebarWidthChanged)
    
public:
    Settings(QObject* parent = 0);
    virtual ~Settings();
    
    int mathematicalSidebarWidth();
    void setMathematicalSidebarWidth(int width);
    
signals:
    void mathematicalSidebarWidthChanged();
    
private:
    SettingsCore* m_settingscore;
};

#endif // SETTINGS_H
