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

#ifndef ITOOLS_PLUGIN_MANAGER_H
#define ITOOLS_PLUGIN_MANAGER_H

#include "../include/PluginInterface.h"
#include <string>
#include <vector>
#include <memory> // For std::unique_ptr or managing plugin instances

// Platform-specific includes for dynamic library loading
#ifdef _WIN32
#include <windows.h>
#else // POSIX (Linux, macOS)
#include <dlfcn.h>
#endif

class PluginManager {
public:
	PluginManager(void* app_context); // Application context to pass to plugins
	~PluginManager();

	// Loads a plugin from the given file path.
	bool loadPlugin(const std::string& plugin_path);

	// Unloads all loaded plugins.
	void unloadAllPlugins();

	// Scans a directory for plugins and attempts to load them.
	void loadPluginsFromDirectory(const std::string& directory_path = ".\\..\\Release\\plugins\\ext");

	// Example function to interact with all loaded plugins.
	void callPerformActionOnAll();
	void callGetNameOnAll();


private:
	// Structure to hold information about a loaded plugin
	struct LoadedPlugin {
#ifdef _WIN32
		HMODULE handle; // Library handle on Windows
#else
		void* handle;   // Library handle on POSIX
#endif
		IPlugin* instance;
		DestroyPluginFunc destroy_func; // Store the destroy function

		LoadedPlugin(decltype(handle) h, IPlugin* inst, DestroyPluginFunc df)
				: handle(h), instance(inst), destroy_func(df) {}
	};

	std::vector<LoadedPlugin> plugins_;
	void* application_context_; // Store the application context

#ifdef _WIN32
	void logWindowsError(const std::string& action);
#endif
};


#endif //ITOOLS_PLUGIN_MANAGER_H
