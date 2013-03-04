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

#include <QObject>
#include <QSize>
#include <QString>

class SettingsCore : public QObject
{
    Q_OBJECT
    
public:
    static SettingsCore* instance();
    
    void loadSettings();
    void saveSettings();
    
    int precision();
    void setPrecision(int precision);
    
    bool compactMode();
    void setCompactMode(bool compactMode);
    
    QSize windowSize();
    void setWindowSize(const QSize& windowSize);
    
    QString mathematicalSidebarActiveView();
    void setMathematicalSidebarActiveView(const QString& activeView);
    
    int mathematicalSidebarWidth();
    void setMathematicalSidebarWidth(int width);
    
    bool mathematicalSidebarVisible();
    void setMathematicalSidebarVisible(bool visible);
    
signals:
    void precisionChanged();
    void compactModeChanged();
    void windowSizeChanged();
    void mathematicalSidebarActiveViewChanged();
    void mathematicalSidebarWidthChanged();
    void mathematicalSidebarVisibleChanged();
    
private:
    SettingsCore();
    ~SettingsCore();
    
    static SettingsCore* m_instance;
    
    QSize m_windowSize;
    QString m_mathematicalSidebarActiveView;
    int m_mathematicalSidebarWidth;
    bool m_mathematicalSidebarVisible;
    bool m_compactMode;
    int m_historyLimit;
};

#endif // SETTINGSCORE_H
