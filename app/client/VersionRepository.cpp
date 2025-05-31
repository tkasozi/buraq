//
// Created by talik on 5/30/2025.
//

#include "VersionRepository.h"
#include <thread>
#include <mutex>
#include <vector>
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.

// Global mutex to protect access to the Network singleton's methods
std::mutex network_mutex;

VersionRepository::VersionRepository() : endpoint("file:///C:/Users/talik/workspace/ITools/app/version.json"),
										 network(Network::singleton()) {}

void VersionRepository::get_manifest_json(const std::string &endpoint, UpdateInfo &info) {
	std::string response;
	std::filesystem::path manifest_json = std::filesystem::temp_directory_path() / "ITools" / "manifest.json";

	{ // Create a scope for the lock_guard
		std::lock_guard<std::mutex> lock(network_mutex); // Lock before accessing network methods

		Network &net = Network::singleton(); // Get the singleton instance
		response = net.http_get(endpoint);         // Call the method
		std::ofstream outputFile(manifest_json, std::ios::out);
		outputFile << response;
		outputFile.close();
		// Mutex is released when 'lock' goes out of scope
	} // network_mutex is unlocked here

	namespace pt = boost::property_tree;
	pt::ptree loadPtreeRoot;

	pt::read_json(manifest_json.string(), loadPtreeRoot);
	std::vector<std::tuple<std::string, std::string, std::string>> version;

	if (loadPtreeRoot.empty()) {
		throw std::runtime_error("Failed to download update manifest.");
	}

	auto latest_version_node = loadPtreeRoot.get_child("latestVersion");
	info.latestVersion = latest_version_node.get_value<std::string>();

	auto download_url_node = loadPtreeRoot.get_child("downloadUrl");
	info.downloadUrl = download_url_node.get_value<std::string>();

	auto release_notes_node = loadPtreeRoot.get_child("releaseNotes");
	info.releaseNotes = release_notes_node.get_value<std::string>();
}

UpdateInfo VersionRepository::main_version_logic() {

	std::vector<std::thread> threads;
	const int num_threads = 1;

	// create a separate thread..
	for (int i = 0; i < num_threads; ++i) {
		threads.emplace_back(get_manifest_json, endpoint, std::ref(versionInfo));
	}

	for (std::thread &t: threads) {
		if (t.joinable()) {
			t.join();
		}
	}

	try {
		std::string currentVersion = getCurrentAppVersion();

		if (versionInfo.latestVersion.empty()) {
			std::cerr << "No version" << std::endl;

			// no version
			return {};
		}

		std::vector<std::string> ver = split_version(versionInfo.latestVersion);

		if (ver.size() != 3) {
			std::cerr << "Bad version format" << std::endl;

			// invalid version
			return {};
		}

		if (std::stoi(ver[0]) > APP_VERSION_MAJOR
			|| std::stoi(ver[1]) > APP_VERSION_MINOR
			|| std::stoi(ver[2]) > APP_VERSION_PATCH) {
			std::cout << "A new version " << versionInfo.latestVersion << " is available!" << std::endl;
			std::cout << "Release notes: " << versionInfo.releaseNotes << std::endl;
			return versionInfo;
		} else {
			std::cout << "You have the latest version. " << currentVersion << std::endl;
			return {};
		}
	} catch (...) {
		std::cerr << "Exception while comparing versions" << std::endl;
		// exceptions
		return {};
	}
}


std::string VersionRepository::getCurrentAppVersion() {
	return std::to_string(APP_VERSION_MAJOR)
		   + "." + std::to_string(APP_VERSION_MINOR)
		   + "." + std::to_string(APP_VERSION_PATCH);
};


std::vector<std::string> VersionRepository::split_version(const std::string &str) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(46); // 46 => '.'
	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(46, start);
	}
	tokens.push_back(str.substr(start));
	return tokens;
}
