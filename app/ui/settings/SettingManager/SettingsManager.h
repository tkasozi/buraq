//
// Created by talik on 8/30/2025.
//

#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <optional> // Required for std::optional

#include "../UserSettings.h" // Settings struct

class SettingsManager
{
public:
    // Saves the given settings object to persistent storage
    static void saveSettings(const UserSettings& settings);

    // Loads the settings object from persistent storage
    static UserSettings loadSettings();

private:
    static std::optional<AppTheme> intToTheme(int value);
};

#endif // SETTINGSMANAGER_H
