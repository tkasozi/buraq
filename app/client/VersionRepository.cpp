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


VersionRepository::VersionRepository(IToolsApi* api_context) :
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
    std::filesystem::path manifest_json = std::filesystem::temp_directory_path() / "ITools" / "manifest.json";

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

    namespace pt = boost::property_tree;
    try
    {
        pt::ptree loadPtreeRoot;

        pt::read_json(manifest_json.string(), loadPtreeRoot);
        std::vector<std::tuple<std::string, std::string, std::string>> version;

        if (loadPtreeRoot.empty())
        {
            throw std::runtime_error("Failed to download update manifest.");
        }

        auto latest_version_node = loadPtreeRoot.get_child("version");
        info.latestVersion = latest_version_node.get_value<std::string>();

        auto release_notes_node = loadPtreeRoot.get_child("notes");
        info.releaseNotes = release_notes_node.get_value<std::string>();

        std::list<std::string> asset_names;

        std::vector<std::string> item_list;
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
        qDebug() << e.what();
    }
}

UpdateInfo VersionRepository::main_version_logic()
{
    std::vector<std::thread> threads;
    constexpr int num_threads = 1;

    // create a separate thread..
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(get_manifest_json, endpoint, std::ref(versionInfo));
    }

    for (std::thread& t : threads)
    {
        if (t.joinable())
        {
            t.join();
        }
    }

    try
    {
        const std::string currentVersion = getCurrentAppVersion();

        if (versionInfo.latestVersion.empty())
        {
            std::cerr << "No version" << std::endl;

            // no version
            return {};
        }

        const std::vector<std::string> ver = split_version(versionInfo.latestVersion);

        if (ver.size() != 3)
        {
            std::cerr << "Bad version format" << std::endl;

            // invalid version
            return {};
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
            std::cout << "You have the latest version. " << currentVersion << std::endl;
            return {};
        }
    }
    catch (...)
    {
        std::cerr << "Exception while comparing versions" << std::endl;
        // exceptions
        return {};
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
        tokens.push_back(str_version.substr(start, end - start));
        start = end + 1;
        end = str_version.find(46, start);
    }
    tokens.push_back(str_version.substr(start));
    return tokens;
}

std::filesystem::path VersionRepository::downloadNewVersion() const
{
    if (versionInfo.latestVersion.empty())
    {
        return {}; // No version available, exit the application
    };

    std::filesystem::path latestRelease = std::filesystem::temp_directory_path() / "ITools" / versionInfo.asset.name;

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
        return std::filesystem::temp_directory_path() / "ITools";
    }

    return latestRelease;
}
