//
// Created by talik on 6/7/2025.
//

#ifndef UPDATE_WORKER_H
#define UPDATE_WORKER_H


#include <QObject>
#include <QString>

#ifdef _WIN32

#include <windows.h>

#endif

class UpdateWorker final : public QObject
{
    Q_OBJECT

public:
    explicit UpdateWorker(QObject* parent = nullptr);

public slots:
    // The main function to run in the background thread
    void doUpdate(const std::filesystem::path& installerPath, const std::filesystem::path& installationPath,
                  unsigned long parentPID);

signals:
    // Signals to update the GUI
    void statusTextChanged(const QString& text);
    void logMessage(const QString& message);
    void progressChanged(int percentage);
    void finished(bool success, const QString& finalMessage);
    void restart();

private:
    void waitForMainAppToClose(unsigned long parentPID);
#ifdef _WIN32

    bool launchApp(const std::filesystem::path& appPath, HANDLE* outProcessHandle, const QString& args, bool createNewConsole = false);
    bool waitForAppWindow(HANDLE hProcess, DWORD timeout_ms);

#endif
    bool installNewVersion(const std::filesystem::path& installerPath, const std::filesystem::path& installationPath);
};

#endif //UPDATE_WORKER_H
