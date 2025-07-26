//
// Created by talik on 5/30/2025.
//

#ifndef REPO_H
#define REPO_H

#include <string>
#include "network.h"
#include "app_version.h"
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.
#include "client/VersionRepository.h"
#include "IToolsAPI.h"

class VersionRepository {

public:
	explicit VersionRepository(IToolsApi *api_context);

	~VersionRepository() = default;

	/**
	 * Determines if there is a new version
	 * @return Returns the new version object or an empty version object otherwise.
	 */
	[[nodiscard]] UpdateInfo main_version_logic();
	[[nodiscard]] std::filesystem::path downloadNewVersion() const;


private:
	IToolsApi *api_context;
	std::string endpoint;
	UpdateInfo versionInfo;
	Network &network;

	static std::string getCurrentAppVersion();
	static std::vector<std::string> split_version(const std::string &str);

	/**
 * Fetches the application manifestJson file.
 * @return File path to where the manifestJson was downloaded.
 */
	static void get_manifest_json(const std::string& endpoint, UpdateInfo& info);
};



#endif //REPO_H
