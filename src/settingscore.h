/*
    settingscore.h - access the settings through QML
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


#ifndef SETTINGSCORE_H
#define SETTINGSCORE_H

#include <QSize>
#include <QString>

class SettingsCore
{
    
public:
    static SettingsCore* instance();
    
    void loadSettings();
    void saveSettings();
    
    bool compactMode();
    void setCompactMode(bool compactMode);
    
    QSize windowSize();
    void setWindowSize(QSize windowSize);
    
    QString mathematicalSidebarActiveView();
    void setMathematicalSidebarActiveView(QString activeView);
    
    int mathematicalSidebarWidth();
    void setMathematicalSidebarWidth(int width);
    
    bool mathematicalSidebarVisible();
    void setMathematicalSidebarVisible(bool visible);
    
    bool wasMathematicalSidebarShown();
    void setWasMathematicalSidebarShown(bool wasShown);
    
private:
    SettingsCore();
    ~SettingsCore();
    
    static SettingsCore* m_instance;
    
    QSize m_windowSize;
    QString m_mathematicalSidebarActiveView;
    int m_mathematicalSidebarWidth;
    bool m_mathematicalSidebarVisible;
    bool m_wasMathematicalSidebarShown;
    bool m_compactMode;
    int m_historyLimit;
};

#endif // SETTINGSCORE_H
