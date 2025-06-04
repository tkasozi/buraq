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

#ifndef ITOOLS_API_H
#define ITOOLS_API_H

#include <filesystem>
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <fstream>
#include <chrono>
#include <iomanip> // For std::put_time
#include <ctime> // For std::localtime, std::time_t

    // Example of an application context you might pass to plugins
    struct IToolsApi
{
    std::filesystem::path searchPath;
    std::filesystem::path userPath;
    std::filesystem::path userDataPath;
    std::map<std::string, std::string> plugins;
};

struct EditorState
{

    bool hasText;
    bool isBlockValid;
    bool isSelected;
    int blockCount;
    int cursorBlockNumber;
    int blockNumber;
    int lineHeight;
    int currentLineHeight;
    std::set<int> selectedBlockNumbers;

    // For the updateEditorState check if states are different
    bool operator!=(const EditorState &other) const
    {
        return blockCount != other.blockCount ||
               blockNumber != other.blockNumber ||
               cursorBlockNumber != other.cursorBlockNumber;
    }

    bool operator==(const EditorState &other) const
    {
        return !(*this != other);
    }
};

// Function to log messages (example)
static void db_log(const std::string &message)
{
    // Optionally, log to a file in a writable location:
    std::filesystem::path logFilePath = std::filesystem::temp_directory_path() / "ITools" / ".data" / "log.txt";
    try
    {
        std::ofstream outputFile(logFilePath, std::ios::out);
        outputFile.exceptions(std::ifstream::failbit);

        // 1. Get the current time point from the system_clock
        std::chrono::system_clock::time_point now_tp = std::chrono::system_clock::now();

        // 2. Convert the time_point to a time_t (needed for some C-style functions like std::localtime)
        std::time_t now_c = std::chrono::system_clock::to_time_t(now_tp);

        // 3. Convert to a struct tm for more detailed fields (local time)
		// For other compilers, you might need localtime_r or be aware of std::localtime's behavior
		std::tm* temp_tm = std::localtime(&now_c);
		std::tm local_tm_struct; // Use a local variable for the struct tm
		if (temp_tm) {
			local_tm_struct = *temp_tm; // Copy the contents
		} else {
			std::cerr << "Failed to get local time." << std::endl;
			return;
		}

		// 4. Create an std::ostringstream object
		std::ostringstream oss;

		// 5. Use std::put_time to format the time and send it to the ostringstream
		oss << std::put_time(&local_tm_struct, "%Y-%m-%d %H:%M:%S");

        // Print the date and time using std::put_time (C++11)
        outputFile << oss.str() << ": " << message << std::endl;
        outputFile.close();
    }
    catch (const std::ios_base::failure &failure)
    {
		std::cerr << "Fails "<< failure.what() << " code: " << failure.code();
    }
}

#endif // ITOOLS_API_H
