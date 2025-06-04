// Updater.exe - main.cpp
#include <windows.h>
#include <string>
#include <iostream>
//#include <tlhelp32.h> // For process checking (optional, better to use Handle)
#include <filesystem> // C++17
// You'll need a ZIP extraction library, e.g., Minizip or a system call to a utility

void extractZip(const std::string& zipPath, const std::string& extractToPath) {
    // Implementation for extracting a ZIP file.
    // This can be complex. Consider using a library like minizip (from zlib contrib)
    // or shelling out to a command like `tar -xf` on Linux/macOS or `Expand-Archive` via PowerShell on Windows.
    // For Windows, you could use Shell API (IShellDispatch::Namespace::CopyHere) but it's COM based.
    std::cout << "Extracting " << zipPath << " to " << extractToPath << std::endl;

    // Simplified example: Create a dummy file to simulate extraction
    // std::filesystem::create_directories(extractToPath);
    // std::ofstream dummyFile(std::filesystem::path(extractToPath) / "MyApp.exe");
    // dummyFile << "This is the new version." << std::endl;
    // dummyFile.close();

    // Example using PowerShell's Expand-Archive (Windows 10+ or if PowerShell Core is installed)
    // This is a system call, which has its own pros and cons.
    std::string command = "powershell -command \"Expand-Archive -Path '";
    command += zipPath;
    command += "' -DestinationPath '";
    command += extractToPath;
    command += "' -Force\""; // -Force to overwrite

    std::cout << "Executing: " << command << std::endl;
    int result = system(command.c_str());
    if (result == 0) {
        std::cout << "Extraction successful." << std::endl;
    } else {
        std::cerr << "Extraction failed. Exit code: " << result << std::endl;
        // Potentially try another method or report error
    }
}

void relaunchMainApp(const std::string& installPath) {
	// Example:
	// Updater.exe "C:\Users\talik\AppData\Local\Temp\ITools\updates" "C:\Program Files\ITools" ParentPID
    std::filesystem::path mainAppPath = std::filesystem::path(installPath) / "ITools.exe"; // Assuming your app is MyApp.exe
    std::cout << "Relaunching " << mainAppPath.string() << std::endl;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(mainAppPath.string().c_str(), NULL, NULL, NULL, FALSE, 0, NULL, installPath.c_str(), &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "Main application relaunched." << std::endl;
    } else {
        std::cerr << "Failed to relaunch main application. Error: " << GetLastError() << std::endl;
    }
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: Updater.exe <packagePath> <installPath> <parentPID>" << std::endl;
        return 1;
    }

    std::string packagePath = argv[1];
    std::string installPath = argv[2];
    DWORD parentPID = std::stoul(argv[3]);

    std::cout << "Updater started." << std::endl;
    std::cout << "Package: " << packagePath << std::endl;
    std::cout << "Install Dir: " << installPath << std::endl;
    std::cout << "Parent PID: " << parentPID << std::endl;

    // Wait for the main application to close
    HANDLE hParentProcess = OpenProcess(SYNCHRONIZE, FALSE, parentPID);
    if (hParentProcess == NULL) {
        std::cerr << "Could not open parent process. It might have already exited. Error: " << GetLastError() << std::endl;
        // Proceed if GetLastError() is ERROR_INVALID_PARAMETER (process already exited)
        if (GetLastError() != ERROR_INVALID_PARAMETER) {
             // If it's another error, maybe wait a bit or proceed cautiously
        }
    } else {
        std::cout << "Waiting for main application (PID: " << parentPID << ") to exit..." << std::endl;
        WaitForSingleObject(hParentProcess, INFINITE);
        CloseHandle(hParentProcess);
        std::cout << "Main application has exited." << std::endl;
    }

    // A small delay to ensure files are unlocked
    Sleep(1000); // 1 second, adjust as needed

    // c. Extract and Replace Files
    std::cout << "Starting update process..." << std::endl;
    try {
        // Ensure installPath exists, if not, this might be an initial install via updater
        if (!std::filesystem::exists(installPath)) {
            std::filesystem::create_directories(installPath);
        }

        // Before extracting, you might want to delete old files from installPath
        // Be careful with this! Only delete files you know are part of your app.
        // Or, the extraction can overwrite them if using -Force or similar.

        extractZip(packagePath, installPath); // You need a robust ZIP extraction function here!

        // d. Cleanup
        std::cout << "Cleaning up downloaded package..." << std::endl;
        if (std::filesystem::exists(packagePath)) {
            std::filesystem::remove(packagePath);
        }

        // e. Relaunch Main Application
        relaunchMainApp(installPath);

    } catch (const std::exception& e) {
        std::cerr << "Update failed: " << e.what() << std::endl;
        // Log error, maybe try to restore a backup if you implemented that
        // For now, try to relaunch the old app if possible or just exit
        relaunchMainApp(installPath); // Attempt to launch whatever is there
        return 1;
    }

    std::cout << "Updater finished. Exiting." << std::endl;
    return 0;
}
