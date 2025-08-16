//
// Created by talik on 5/30/2025.
//

#include "VersionRepository.h"
#include <thread>
#include <mutex>
#include <vector>
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.
#include <qlogging.h>
#include <QDebug>
#include <list>
#include <ranges>
#include <boost/property_tree/json_parser.hpp>

#include "../include/version.h"
#include "../include/network.h"

// Global mutex to protect access to the Network singleton's methods
std::mutex network_mutex;


VersionRepository::VersionRepository(buraq_api* api_context) :
    endpoint("https://raw.githubusercontent.com/tkasozi/buraq/refs/heads/main/manifest.json"),
    network(Network::singleton()),
    api_context(api_context)
{
}

static std::string get(const boost::property_tree::basic_ptree<std::string, std::string>& a, const char* path)
{
    try
    {
        return a.get<std::string>(path);
    }
    catch (const std::exception& e)
    {
        qDebug() << "property_tree " << e.what();
        return "";
    }
}

void VersionRepository::get_manifest_json(const std::string& endpoint, UpdateInfo& info)
{
    namespace pt = boost::property_tree;

    std::filesystem::path manifest_json = std::filesystem::temp_directory_path() / "Buraq" / "manifest.json";

    try
    {
        pt::ptree loadPtreeRoot;
        {
            std::string response;
            // Create a scope for the lock_guard
            std::lock_guard<std::mutex> lock(network_mutex); // Lock before accessing network methods

            Network& net = Network::singleton(); // Get the singleton instance
            response = net.http_get(endpoint); // Call the method
            std::ofstream outputFile(manifest_json, std::ios::out);
            outputFile << response;
            outputFile.close();
            // Mutex is released when 'lock' goes out of scope
        } // network_mutex is unlocked here

        pt::read_json(manifest_json.string(), loadPtreeRoot);

        if (loadPtreeRoot.empty())
        {
            throw std::runtime_error("Failed to download update manifest.");
        }

        info.isConnFailure = false;

        auto latest_version_node = loadPtreeRoot.get_child("version");
        info.latestVersion = latest_version_node.get_value<std::string>();

        auto release_notes_node = loadPtreeRoot.get_child("notes");
        info.releaseNotes = release_notes_node.get_value<std::string>();

        for (auto& asset : loadPtreeRoot.get_child("assets") | std::views::values)
        {
            info.asset = {
                .name = asset.get<std::string>("name"),
                .downloadUrl = asset.get<std::string>("download_url"),
                .size = asset.get<std::string>("size"),
                // .sha = asset.get<std::string>("sha"),
                .sha = get(asset, "sha"),
            };
        }
    }
    catch (const pt::ptree_error& e)
    {
        qDebug() << "Related to the manifest.json file: " << e.what();
    }
    catch (const std::exception& e)
    {
        info.isConnFailure = true;
        qDebug() << "Other Error (ie. Network): " << e.what();
    }
}

UpdateInfo VersionRepository::main_version_logic()
{
    if (std::thread thread(get_manifest_json, endpoint, std::ref(versionInfo)); thread.joinable())
    {
        thread.join();
    }

    try
    {
        if (versionInfo.latestVersion.empty())
        {
            std::cerr << "No version" << std::endl;

            // no version
            return versionInfo;
        }

        const std::vector<std::string> ver = split_version(versionInfo.latestVersion);

        if (ver.size() != 3)
        {
            std::cerr << "Bad version format" << std::endl;

            // invalid version
            return {.isConnFailure = versionInfo.isConnFailure};
        }

        if (std::stoi(ver[0]) > APP_VERSION_MAJOR || std::stoi(ver[1]) > APP_VERSION_MINOR || std::stoi(ver[2]) >
            APP_VERSION_PATCH)
        {
            std::cout << "A new version " << versionInfo.latestVersion << " is available!" << std::endl;
            std::cout << "Release notes: " << versionInfo.releaseNotes << std::endl;
            return versionInfo;
        }
        else
        {
            std::cout << "You have the latest version. " << getCurrentAppVersion() << std::endl;
            return {};
        }
    }
    catch (...)
    {
        std::cerr << "Exception while comparing versions" << std::endl;
        // exceptions
        return {.isConnFailure = versionInfo.isConnFailure};
    }
}

std::string VersionRepository::getCurrentAppVersion()
{
    return std::to_string(APP_VERSION_MAJOR) + "." + std::to_string(APP_VERSION_MINOR) + "." + std::to_string(
        APP_VERSION_PATCH);
};

std::vector<std::string> VersionRepository::split_version(const std::string& str)
{
    if (str.empty()) return {};

    std::string str_version = str;
    if (str.starts_with('v'))
    {
        str_version = str.substr(1, str.length() - 1);
    }
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str_version.find(46); // 46 => '.'
    while (end != std::string::npos)
    {
        tokens.emplace_back(str_version.substr(start, end - start));
        start = end + 1;
        end = str_version.find(46, start);
    }
    tokens.emplace_back(str_version.substr(start));
    return tokens;
}

std::filesystem::path VersionRepository::downloadNewVersion() const
{
    if (versionInfo.latestVersion.empty())
    {
        return {}; // No version available, exit the application
    };

    std::filesystem::path latestRelease = std::filesystem::temp_directory_path() / "Buraq" / versionInfo.asset.name;

    bool has_errors = false;
    {
        // Create a scope for the lock_guard
        std::lock_guard<std::mutex> lock(network_mutex); // Lock before accessing network methods

        const Network& net = Network::singleton(); // Get the singleton instance
        if (const auto response = net.downloadFile(versionInfo.asset.downloadUrl, latestRelease.string().c_str());
            response != 0)
        {
            has_errors = true;
            std::cout << "Failed to download" << std::endl;
        }
        // Mutex is released when 'lock' goes out of scope
    } // network_mutex is unlocked here

    if (has_errors)
    {
        return std::filesystem::temp_directory_path() / "Buraq";
    }

    return latestRelease;
}
