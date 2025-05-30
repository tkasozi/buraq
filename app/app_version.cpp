//
// Created by talik on 5/29/2025.
//

#include <boost/property_tree/json_parser.hpp>
#include "app_version.h"

#include "../include/version.h"
#include "../include/network.h"

std::string getCurrentAppVersion() {
	return std::to_string(APP_VERSION_MAJOR)
		   + "." + std::to_string(APP_VERSION_MINOR)
		   + "." + std::to_string(APP_VERSION_PATCH);
};

UpdateInfo checkForUpdates() {
	namespace pt = boost::property_tree;
	pt::ptree loadPtreeRoot;

	// std::string manifestContent = httpGet("file:///C:/Users/talik/workspace/ITools/app/version.json");
	pt::read_json("version.json", loadPtreeRoot);
	std::vector<std::tuple<std::string, std::string, std::string>> version;

	if (loadPtreeRoot.empty()) {
		std::cerr << "Failed to download update manifest." << std::endl;
		return {};
	}

	UpdateInfo info;

	auto latest_version_node = loadPtreeRoot.get_child("latestVersion");
	info.latestVersion = latest_version_node.get_value<std::string>();

	auto download_url_node = loadPtreeRoot.get_child("downloadUrl");
	info.downloadUrl = download_url_node.get_value<std::string>();

	auto release_notes_node = loadPtreeRoot.get_child("releaseNotes");
	info.releaseNotes = release_notes_node.get_value<std::string>();

	return info;
}

std::vector<std::string> split(const std::string &str, char delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);
	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}
	tokens.push_back(str.substr(start));
	return tokens;
}

int main_version_logic() { // Renamed to avoid conflict with a real main

	try {
		std::string currentVersion = getCurrentAppVersion();
		UpdateInfo updateInfo = checkForUpdates();

		if (updateInfo.latestVersion.empty()) {
			std::cerr << "No version" << std::endl;

			// no version
			return -1;
		}

		std::vector<std::string> ver = split(updateInfo.latestVersion, '.');

		if (ver.size() != 3) {
			std::cerr << "Bad version format" << std::endl;

			// invalid version
			return -1;
		}

		if (std::stoi(ver[0]) > APP_VERSION_MAJOR
			|| std::stoi(ver[1]) > APP_VERSION_MINOR
			|| std::stoi(ver[2]) > APP_VERSION_PATCH) {
			std::cout << "A new version " << updateInfo.latestVersion << " is available!" << std::endl;
			std::cout << "Release notes: " << updateInfo.releaseNotes << std::endl;
		} else {
			std::cout << "You have the latest version. " << currentVersion << std::endl;
		}
	} catch (...) {
		std::cerr << "Exception while comparing versions" << std::endl;
		// exceptions
		return -1;
	}

	return 0;
}
