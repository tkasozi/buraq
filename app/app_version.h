//
// Created by talik on 5/29/2025.
//

#ifndef APP_VERSION_H
#define APP_VERSION_H

#include <string>

struct github_manifest_asset
{
	std::string name;
	std::string downloadUrl;
	std::string size;
	std::string contentType;
	std::string sha;
};

struct UpdateInfo {
	std::string latestVersion;
	std::string releaseNotes;
	github_manifest_asset asset;
};

std::string getCurrentAppVersion();
UpdateInfo checkForUpdates();
int main_version_logic();

#endif //APP_VERSION_H
