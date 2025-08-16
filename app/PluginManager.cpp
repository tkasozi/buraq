// MIT License
//
// Copyright (c)  "2025" Talik A. Kasozi
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// Created by talik on 5/16/2025.
//

#include "PluginManager.h"
#include <iostream>
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.

#ifdef _WIN32

void PluginManager::logWindowsError(const std::string &action) {
	DWORD error_id = GetLastError();
	if (error_id == 0) {
		std::cerr << "Error in " << action << ": No error code." << std::endl;
		return;
	}
	LPSTR message_buffer = nullptr;
	size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR) &message_buffer, 0, NULL);

	std::cerr << "Error in " << action << " (Code " << error_id << "): " << message_buffer << std::endl;
	LocalFree(message_buffer);
}

#endif

PluginManager::PluginManager(buraq::buraq_api *app_context) : application_context_(app_context) {}

PluginManager::~PluginManager() {
//	std::cout << "~PluginManager()" << std::endl;
	unloadAllPlugins();
}

bool PluginManager::loadPlugin(const std::string &plugin_name) {
	std::string plugin_path = application_context_->plugins.at(plugin_name);

#ifdef _WIN32
	HMODULE plugin_handle = LoadLibraryA(plugin_path.c_str());
	if (!plugin_handle) {
		logWindowsError("LoadLibraryA for " + plugin_path);
		return false;
	}
#else
	// RTLD_NOW: Resolve all symbols immediately.
		// RTLD_LAZY: Resolve symbols only when they are referenced.
		// RTLD_GLOBAL: Make symbols from this library available for subsequently loaded libraries.
		void* plugin_handle = dlopen(plugin_path.c_str(), RTLD_NOW | RTLD_GLOBAL);
		if (!plugin_handle) {
			std::cerr << "Failed to load plugin " << plugin_path << ". Error: " << dlerror() << std::endl;
			return false;
		}
#endif

	// Get pointers to the factory functions
#ifdef _WIN32
	CreatePluginFunc create_func = (CreatePluginFunc) GetProcAddress(plugin_handle, "create_plugin");
	DestroyPluginFunc destroy_func = (DestroyPluginFunc) GetProcAddress(plugin_handle, "destroy_plugin");
#else
	// Clear any existing error
		dlerror();
		CreatePluginFunc create_func = (CreatePluginFunc)dlsym(plugin_handle, "create_plugin");
		const char* dlsym_error = dlerror();
		if (dlsym_error) {
#ifdef _WIN32
				FreeLibrary(plugin_handle);
#else
				dlclose(plugin_handle);
#endif
			return false;
		}

		destroy_func = (DestroyPluginFunc)dlsym(plugin_handle, "destroy_plugin");
		dlsym_error = dlerror();
		if (dlsym_error) {
#ifdef _WIN32
				FreeLibrary(plugin_handle);
#else
				dlclose(plugin_handle);
#endif
			return false;
		}
#endif

	if (!create_func || !destroy_func) {
#ifdef _WIN32
		if (!create_func) logWindowsError("GetProcAddress for create_plugin");
		if (!destroy_func) logWindowsError("GetProcAddress for destroy_plugin");
		FreeLibrary(plugin_handle);
#else
		dlclose(plugin_handle);
#endif
		return false;
	}

	IPlugin *plugin_instance = create_func(application_context_);
	if (!plugin_instance) {
#ifdef _WIN32
		FreeLibrary(plugin_handle);
#else
		dlclose(plugin_handle);
#endif
		return false;
	}

	// Initialize the plugin
	if (!plugin_instance->initialize(application_context_)) {
		std::cerr << "Plugin initialization failed for: " << plugin_instance->getName() << " from " << plugin_path
				  << std::endl;
		destroy_func(plugin_instance); // Clean up the partially created plugin
#ifdef _WIN32
		FreeLibrary(plugin_handle);
#else
		dlclose(plugin_handle);
#endif
		return false;
	}

	plugins_.emplace_back(plugin_handle, plugin_instance, destroy_func);

	return true;
}

void PluginManager::unloadAllPlugins() {
	// Unload in reverse order of loading (optional, but sometimes good practice)
	for (auto it = plugins_.rbegin(); it != plugins_.rend(); ++it) {
		if (it->instance) {
			it->instance->shutdown(); // Call plugin's shutdown method
			it->destroy_func(it->instance); // Call plugin's destroy function
			it->instance = nullptr;
		}
		if (it->handle) {
#ifdef _WIN32
			FreeLibrary(it->handle);
#else
			dlclose(it->handle);
#endif
			it->handle = nullptr;
		}
	}
	plugins_.clear();
}

void PluginManager::loadPluginsFromDirectory(const std::string &directory_path) {
	std::cout << "Scanning for plugins in directory: " << directory_path << std::endl;

	// ----------------------------------------------------------------------------------
	const char *pluginPath = directory_path.c_str();
	char fullPath[MAX_PATH];
	if (GetFullPathNameA(pluginPath, MAX_PATH, fullPath, NULL) != 0) {
		std::cout <<"Attempting to load plugin from: %s\n", fullPath;
	} else {
		std::cout <<"Could not resolve full path for: %s\n", pluginPath;
	}
	HMODULE hPlugin = LoadLibraryA(pluginPath);
	if (hPlugin == NULL) {
		std::cout <<"Error in LoadLibraryA for %s (Code %lu)\n" << pluginPath << "" << GetLastError();
	}
	// ----------------------------------------------------------------------------------
	namespace fs = std::filesystem;

	if (!fs::exists(directory_path) || !fs::is_directory(directory_path)) {
		std::cerr << "Plugin directory does not exist or is not a directory: " << directory_path << std::endl;
		return;
	}

	for (const auto &entry: fs::directory_iterator(directory_path)) {
		if (entry.is_regular_file()) {
			fs::path path = entry.path();
			std::string extension = path.extension().string();
			bool loaded = false;
#ifdef _WIN32
			if (extension == ".dll") {
				loaded = loadPlugin(path.string());
			}
#elif __APPLE__ // macOS uses .dylib
			if (extension == ".dylib") {
					loaded = loadPlugin(path.string());
				}
#else // Linux and other POSIX use .so
				if (extension == ".so") {
					loaded = loadPlugin(path.string());
				}
#endif
			if (loaded) {
				// Optional: do something immediately after a successful load
			}
		}
	}
}

ProcessedData PluginManager::callPerformAction(void *cmd) {
	if (plugins_.empty()) {
		return {};
	}

	if (const auto &p_info = plugins_.front(); p_info.instance) {
		return p_info.instance->performAction(cmd);
	}
	return {};
}
