//
// Created by talik on 5/29/2025.
//

#include "../include/version.h"
#include <boost/property_tree/json_parser.hpp>

#include "app_version.h"


std::string getCurrentAppVersion() {
	return std::to_string(APP_VERSION_MAJOR)
		   + "." + std::to_string(APP_VERSION_MINOR)
		   + "." + std::to_string(APP_VERSION_PATCH);
};

UpdateInfo checkForUpdates() {
	namespace pt = boost::property_tree;
	pt::ptree loadPtreeRoot;

	// std::string manifestContent = httpGet("https://yourserver.com/updates/version.json");
	pt::read_json("version.json", loadPtreeRoot);
	std::vector<std::tuple<std::string, std::string, std::string>> version;

	if (loadPtreeRoot.empty()) {
		std::cerr << "Failed to download update manifest." << std::endl;
		return {};
	}

	UpdateInfo info;

	auto latest_version_node = loadPtreeRoot.get_child("latestVersion");
	if(!latest_version_node.empty()) {
		info.latestVersion = latest_version_node.get_value<std::string>();
	}

	auto download_url_node = loadPtreeRoot.get_child("downloadUrl");
	if(!download_url_node.empty()) {
		info.downloadUrl = download_url_node.get_value<std::string>();
	}

	auto release_notes_node = loadPtreeRoot.get_child("releaseNotes");
	if(!release_notes_node.empty()) {
		info.releaseNotes = release_notes_node.get_value<std::string>();
	}

	return info;
}

int main_version_logic() { // Renamed to avoid conflict with a real main
	std::string currentVersion = getCurrentAppVersion();
	UpdateInfo updateInfo = checkForUpdates();

	if (!updateInfo.latestVersion.empty() && updateInfo.latestVersion > currentVersion) {
		std::cout << "A new version " << updateInfo.latestVersion << " is available!" << std::endl;
		std::cout << "Release notes: " << updateInfo.releaseNotes << std::endl;
		// Ask user if they want to update
		// If yes, proceed to download
	} else {
		std::cout << "You have the latest version." << std::endl;
	}
	return 0;
}
