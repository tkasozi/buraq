//
// Created by talik on 8/30/2025.
//

#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QString>
#include <QSize>
#include <QPoint>

#include "Filters/ThemeManager/ThemeManager.h"

// A simple struct to hold all application preferences
struct UserSettings {
    AppTheme theme = Dark;
    QSize windowSize = QSize(2100, 720);
    QPoint windowPosition = QPoint(100, 100);
    bool wordWrapEnabled = true;
    int editorFontSize = 11;
};

#endif // USERSETTINGS_H
