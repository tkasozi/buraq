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

#ifndef SCROLL_BAR_STYLES
#define SCROLL_BAR_STYLES R"(
        QPlainTextEdit {
            /* Add padding so text doesn't touch the scrollbar directly if scrollbar is inside */
            padding-right: 5px;
            padding-bottom: 5px;
			font-family: Arial;
			font-size: 14px;
			color: #ffffff;
			height: 100vh;
			width: 100%;
			background-color: #232323;
        }

        /* Style for BOTH horizontal and vertical scrollbars */
        QScrollBar:vertical {
            background: #232323; /* Scrollbar track color */
            width: 15px;         /* Width of the vertical scr  ollbar */
            margin: 0px 0px 0px 0px; /* Top, Right, Bottom, Left margins */
        }
        QScrollBar::handle:vertical {
            background: #606060; /* Handle color */
            min-height: 20px;    /* Minimum height of the handle */
            border-radius: 7px;  /* Rounded corners for the handle */
        }
        QScrollBar::handle:vertical:hover {
            background: #707070; /* Handle color on hover */
        }
        QScrollBar::add-line:vertical { /* Up arrow button */
            background: #232323;
            height: 14px;
            subcontrol-position: bottom;
            subcontrol-origin: margin;
        }
        QScrollBar::sub-line:vertical { /* Down arrow button */
            background: #232323;
            height: 14px;
            subcontrol-position: top;
            subcontrol-origin: margin;
        }
        /* You can use an image for the arrows: */
        /*
        QScrollBar::up-arrow:vertical {
            image: url(:/icons/up_arrow.png);
        }
        QScrollBar::down-arrow:vertical {
            image: url(:/icons/down_arrow.png);
        }
        */
        QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
            background: none; /* Area between handle and arrows */
        }
    )"
#endif

namespace ItoolsNS {
	[[maybe_unused]] extern Config main_config;

// Helper function to get the user's home directory

	static std::filesystem::path get_user_home_directory() {
		const char *home_dir_env = nullptr;

#ifdef _WIN32 // Check if compiling on Windows
		home_dir_env = std::getenv("USERPROFILE");
#else // For POSIX systems (Linux, macOS, etc.)
		home_dir_env = std::getenv("HOME");
#endif

		if (home_dir_env == nullptr) {
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
