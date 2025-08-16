//
// Created by talik on 8/15/2025.
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip> // For std::put_time
#include <ctime> // For std::localtime, std::time_t

#include "./buraq_api.h"

namespace file_utils
{
    std::string getFilename(const std::string& filePath)
    {
        if (!filePath.empty())
        {
            return std::filesystem::path(filePath).filename().string();
        }

        return nullptr;
    }

    void file_log(const std::string& message)
    {
        // Optionally, log to a file in a writable location:
        std::filesystem::path logFilePath = std::filesystem::temp_directory_path() / "Buraq" / ".data" / "log.txt";
        try
        {
            // Open the file in append mode.
            // std::ios::app ensures output is always written to the end.
            // std::ios::out is the default output mode, often implied by app, but explicit is fine.
            std::ofstream outputFile(logFilePath, std::ios::out | std::ios::app);
            outputFile.exceptions(std::ifstream::failbit);

            // Always check if the file was opened successfully!
            if (!outputFile.is_open())
            {
                std::cerr << "Error: Could not open file " << logFilePath.string() << " for appending." << std::endl;
                return;
            }

            // 1. Get the current time point from the system_clock
            std::chrono::system_clock::time_point now_tp = std::chrono::system_clock::now();

            // 2. Convert the time_point to a time_t (needed for some C-style functions like std::localtime)
            std::time_t now_c = std::chrono::system_clock::to_time_t(now_tp);

            // 3. Convert to a struct tm for more detailed fields (local time)
            // For other compilers, you might need localtime_r or be aware of std::localtime's behavior
            std::tm* temp_tm = std::localtime(&now_c);
            std::tm local_tm_struct{}; // Use a local variable for the struct tm
            if (temp_tm)
            {
                local_tm_struct = *temp_tm; // Copy the contents
            }
            else
            {
                std::cerr << "Failed to get local time." << std::endl;
                return;
            }

            // 4. Create a std::ostringstream object
            std::ostringstream oss;

            // 5. Use std::put_time to format the time and send it to the ostringstream
            oss << std::put_time(&local_tm_struct, "%Y-%m-%d %H:%M:%S");

            // Print the date and time using std::put_time (C++11)
            outputFile << oss.str() << ": " << message << std::endl;
            outputFile.close();
        }
        catch (const std::ios_base::failure& failure)
        {
            std::cerr << "Fails " << failure.what() << " code: " << failure.code();
        }
    }
}
