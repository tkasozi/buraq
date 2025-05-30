//
// Created by talik on 5/29/2025.
//

#ifndef APP_VERSION_H
#define APP_VERSION_H

#include <string>
#include <iostream>

struct UpdateInfo {
	std::string latestVersion;
	std::string downloadUrl;
	std::string releaseNotes;
};

std::string getCurrentAppVersion();
UpdateInfo checkForUpdates();
int main_version_logic();

#endif //APP_VERSION_H
