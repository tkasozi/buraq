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
// Created by talik on 5/12/2025.
//

#ifndef ITOOLS_UTILS_H
#define ITOOLS_UTILS_H

#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.

#include "Config.h"

namespace ItoolsNS {
static Config& getConfig() {
    // 'static' here means 'instance' is created only ONCE,
    // the very first time this function is called.
    static Config instance;
    Config::loadConfig(&instance);
    return instance;
}

// Helper function to get the user's home directory

static std::filesystem::path get_user_home_directory() {
    const char *home_dir_env = nullptr;

#ifdef _WIN32 // Check if compiling on Windows
    home_dir_env = std::getenv("USERPROFILE");
#else // For POSIX systems (Linux, macOS, etc.)
    home_dir_env = std::getenv("HOME");
#endif

    if(home_dir_env == nullptr) {
        // Environment variable not found, handle error or return a default/empty path
#ifdef _WIN32
//		std:: << "Warning: USERPROFILE environment variable not set." << std::endl;
#else
        std::cerr << "Warning: HOME environment variable not set." << std::endl;
#endif
        return {}; // Return an empty path
    }

    return std::filesystem::path(home_dir_env);
}
}

#endif //ITOOLS_UTILS_H
