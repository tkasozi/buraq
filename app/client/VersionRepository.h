//
// Created by talik on 5/30/2025.
//

#ifndef REPO_H
#define REPO_H

#include <string>
#include "network.h"
#include "app_version.h"
#include <boost/property_tree/json_parser.hpp>

#include "../include/version.h"
#include "../include/network.h"
#include "client/VersionRepository.h"

class VersionRepository {

public:
	VersionRepository();

	~VersionRepository() = default;

	int main_version_logic();

private:
	std::string endpoint;
	UpdateInfo versionInfo;
	Network &network;

	std::string getCurrentAppVersion();
	std::vector<std::string> split_version(const std::string &str);

	/**
 * Fetches the application manifestJson file.
 * @return File path to where the manifestJson was downloaded.
 */
	static void get_manifest_json(const std::string& endpoint, UpdateInfo& info);
};



#endif //REPO_H
