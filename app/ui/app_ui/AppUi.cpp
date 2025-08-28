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
// Created by Talik Kasozi on 2/3/2024.
//

#include "AppUi.h"
#include "ToolBar.h"
#include "IconButton.h"
#include "editor/Editor.h"
#include "CustomDrawer.h"
#include "EditorMargin.h"
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.
#include <QCoreApplication>
#include <map>

#include <string>

#include "../../clients/VersionClient/VersionRepository.h"
#include "dialog/VersionUpdateDialog.h"
#include "ManagedProcess/ManagedProcess.h"

#ifdef _WIN32

#include <windows.h>
#include <shlobj.h> // SHGetKnownFolderPath

#endif

AppUi::AppUi(QWidget* parent) : QMainWindow(parent)
{
    // Init application views
    initAppLayout();

    // init app's file system
    initAppContext();
}

AppUi::~AppUi()
{
    // When the MyApp object is destroyed, m_bridgeProcess's destructor
    // will be called automatically, terminating the child process.
    delete m_bridgeProcess;
}

void AppUi::onClicked() const
{
    drawer->toggle();
    if (drawer->isVisible())
    {
        placeHolderLayout->addWidget(drawer.get(), 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
    }
    else
    {
        placeHolderLayout->removeWidget(drawer.get());
    }
}

void AppUi::onShowOutputButtonClicked() const
{
    outPutArea->toggle();
    if (outPutArea->isVisible())
    {
        centralWidgetLayout->addWidget(outPutArea.get(), 6, 2, 6, 12);
    }
    else
    {
        centralWidgetLayout->removeWidget(outPutArea.get());
    }
}

Editor* AppUi::getEditor() const
{
    return itoolsEditor.get();
}

PluginManager* AppUi::getLangPluginManager() const
{
    return pluginManager.get();
}

void AppUi::processStatusSlot(const QString& message, const int timeout) const
{
    statusBar->showMessage(message, timeout);
}

void AppUi::processResultSlot(const int exitCode, const QString& output, const QString& error) const
{
    outPutArea->show();

    if (exitCode == 0)
    {
        outPutArea->log(output, error);

        processStatusSlot(error.isEmpty() ? "Completed!" : "Completed with errors.");
    }
    else
    {
        processStatusSlot("Process failed!");
        outPutArea->log("", error);
    }
}

void AppUi::initAppLayout()
{
     // setting up default window size
    const auto windowConfig = Config::singleton().getWindow();
    resize(windowConfig->normalSize, windowConfig->minHeight);
    setMinimumSize(windowConfig->minWidth, windowConfig->minHeight);

    // Add Tool bar
    toolBar = std::make_unique<ToolBar>(this);
    // Add the File menu first
    toolBar->addFileMenu();

    setMenuWidget(toolBar.get());

    // Add Status bar
    statusBar = std::make_unique<QStatusBar>(this);
    statusBar->setObjectName("appStatusBar");
    statusBar->setSizeGripEnabled(false);

    setStatusBar(statusBar.get());

    // Setting window title and docking icon
    setWindowTitle(UpdateInfo().currentVersion.data());
    setWindowIcon(Config::singleton().getAppLogo());

    // Add CentralWidget
    auto* centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    // Add layout to centralWidget
    centralWidgetLayout = std::make_unique<QGridLayout>();
    centralWidgetLayout->setSpacing(0);
    centralWidgetLayout->setContentsMargins(0, 0, 0, 0);
    centralWidget->setLayout(centralWidgetLayout.get());

    // Add control panel to the central widget.
    auto* centralWidgetLayout2 = new QGridLayout;
    centralWidgetLayout2->setSpacing(0);
    centralWidgetLayout2->setContentsMargins(0, 0, 0, 0);

    const auto centralWidgetControlPanel = new QWidget;
    centralWidgetControlPanel->setObjectName("ControlToolBar");

    centralWidgetControlPanel->setFixedWidth(64);
    centralWidgetControlPanel->setLayout(centralWidgetLayout2);

    // panel A
    auto* layoutA = new QVBoxLayout;
    layoutA->setSpacing(1);
    layoutA->setContentsMargins(0, 0, 0, 0);

    auto* centralWidgetControlPanelA = new QWidget;
    centralWidgetControlPanelA->setLayout(layoutA);

    centralWidgetLayout2->addWidget(centralWidgetControlPanelA, 0, 0, 1, 12, Qt::AlignJustify);

    const auto appIcons = Config::singleton().getAppIcons();
    folderButton = std::make_unique<IconButton>(appIcons->folderIcon);

    layoutA->addWidget(folderButton.get());

    // Good, to be placed in panel B
    auto* layoutB = new QVBoxLayout;
    layoutB->setSpacing(8);

    auto* centralWidgetControlPanelB = new QWidget;
    centralWidgetControlPanelB->setLayout(layoutB);
    centralWidgetControlPanelB->setContentsMargins(16, 0, 0, 8);

    centralWidgetLayout2->addWidget(centralWidgetControlPanelB, 11, 0, 12, 12, Qt::AlignBottom);

    const auto outputButton = new IconButton(appIcons->terminalIcon);
    connect(outputButton, &IconButton::clicked, this, &AppUi::onShowOutputButtonClicked);
    layoutB->addWidget(outputButton);

    const auto settingsButton = new IconButton(appIcons->settingsIcon);
    layoutB->addWidget(settingsButton);

    // add to central widget
    centralWidgetLayout->addWidget(centralWidgetControlPanel, 0, 0, 12, 1);

    // Component
    connect(folderButton.get(), &IconButton::clicked, this, &AppUi::onClicked);

    // layout c
    layoutB->setSpacing(0);
    layoutB->setContentsMargins(0, 0, 0, 0);

    // Editor helper component.
    itoolsEditor = std::make_unique<Editor>(this);
    // Handles file nav
    drawer = std::make_unique<CustomDrawer>(itoolsEditor.get());
    // This where the output_display generated after executing the script will be displayed
    outPutArea = std::make_unique<OutputDisplay>(this);

    placeHolderLayout = std::make_unique<QGridLayout>();
    placeHolderLayout->setSpacing(0);
    placeHolderLayout->setContentsMargins(0, 0, 0, 0);

    placeHolderLayout->addWidget(drawer.get(), 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
    // placeHolderLayout->addWidget(editorMargin.get(), 0, 2, 12, 1, Qt::AlignmentFlag::AlignTop);
    placeHolderLayout->addWidget(itoolsEditor.get(), 0, 3, 12, 1);

    // add main content area
    const auto editorAndDrawerAreaPanel = new QWidget;
    editorAndDrawerAreaPanel->setLayout(placeHolderLayout.get());
    centralWidgetLayout->addWidget(editorAndDrawerAreaPanel, 0, 2, 12, 12);
    centralWidgetLayout->addWidget(outPutArea.get(), 6, 2, 6, 12);
}

void AppUi::initAppContext()
{
    const std::filesystem::path userDataPath = std::filesystem::temp_directory_path() / "Buraq";

    // app search_path for plugins
    const std::filesystem::path searchPath(QCoreApplication::applicationDirPath().toStdString());
    // Add required plugins
    const std::map<std::string, std::string> plugins_{};

    api_context = std::make_unique<buraq::buraq_api>();
    api_context->searchPath = searchPath;
    api_context->plugins = plugins_;
    api_context->userDataPath = userDataPath / ".data";
    api_context->userPath = userDataPath;

    pluginManager = std::make_unique<PluginManager>(api_context.get());

    // TBD
    pluginManager->loadPluginsFromDirectory((searchPath / "plugins").string());

    // Schedule onWindowFullyLoaded to run after current event processing is done
    QTimer::singleShot(10000, this, &AppUi::onWindowFullyLoaded);
}

void AppUi::onWindowFullyLoaded()
{
    initPSLangSupport();

    verifyApplicationVersion();
}

void AppUi::initPSLangSupport()
{
    const std::filesystem::path psLangSupportPath = api_context->searchPath / "PS.Bridge/Buraq.Bridge.exe";

    qDebug() << "PSLang Support: " << psLangSupportPath.string();

    processStatusSlot("PSLang Support..", 5000);

    m_bridgeProcess = new ManagedProcess(psLangSupportPath);

    if (!m_bridgeProcess->isRunning()) {
        std::cerr << "Bridge process failed to start." << std::endl;
        processStatusSlot("PowerShell Support Failed.", 5000);
    }

    processStatusSlot("PSLang Support Ready", 30000);
}

void AppUi::verifyApplicationVersion()
{
    VersionRepository repo(api_context.get());

    if (const UpdateInfo update_info = repo.main_version_logic(); update_info.isConnFailure == false)
    {
        if (update_info.latestVersion.empty())
        {
            // No version was set
            qDebug() << "app version is empty or already upto date!";
            return;
        }

        VersionUpdateDialog versionUpdater(this);
        versionUpdater.setWindowTitle(
            "A new version " + QString::fromStdString(update_info.latestVersion) + " is available!");
        versionUpdater.setContent(QString::fromStdString(update_info.releaseNotes));

        if (versionUpdater.exec() == QDialog::Accepted)
        {
            const std::filesystem::path installerExe = repo.downloadNewVersion();

            qDebug() << "AppUi.cpp";
            qDebug() << installerExe.string();
            qDebug() << (api_context->searchPath / "updater.exe").string();
            qDebug() << api_context->searchPath.parent_path().string();
            qDebug() << "ENDs AppUi.cpp";
            // Get the path to the AppData\Local folder
            PWSTR pszPath = NULL;
            if (const HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, NULL, &pszPath); SUCCEEDED(hr))
            {
                // Convert the wide character string to a narrow character string (std::string)
                std::wstring wsPath(pszPath);
                std::string sPath(wsPath.begin(), wsPath.end());

                sPath += "\\Programs\\Buraq";

                // Print the resulting path
                std::cout << "The path is: " << sPath << std::endl;

                launchUpdaterAndExit(
                    api_context->searchPath / "updater.exe", installerExe, sPath
                );

                // Free the memory allocated by SHGetKnownFolderPath
                CoTaskMemFree(pszPath);
            }
            else
            {
                std::cerr << "Failed to get the path." << std::endl;
            }
            processStatusSlot("Ready.", 2000);
        }
        else
        {
            // New version was rejected or modal was closed.
            // do nothing
        }
    }
    else if (update_info.isConnFailure)
    {
        qDebug() << "Update failed to connect to github repository.";
        processStatusSlot("Failed to get updates. Check connection!", 10000);
    }
}

EditorMargin* AppUi::getEditorMargin() const
{
    return editorMargin.get();
}

void AppUi::launchUpdaterAndExit(
    const std::filesystem::path& updaterPath,
    const std::filesystem::path& packagePath,
    const std::filesystem::path& installationPath
) {
    std::string commandLine = "\"" + updaterPath.string() + "\"";
    commandLine += " \"" + packagePath.string() + "\"";
    commandLine += " \"" + installationPath.string() + "\"";
    commandLine += " " + std::to_string(GetCurrentProcessId()); // Pass current process ID

    qDebug() << "commandLine: " << commandLine;

    //updater.exe setup-x.x.x.dev.exe C:\Users\user\AppData\Local\Programs\Buraq\ 16572
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (CreateProcessA(NULL, // No module name (use command line)
                       (LPSTR)commandLine.c_str(), // Command line
                       NULL, // Process handle not inheritable
                       NULL, // Thread handle not inheritable
                       FALSE, // Set handle inheritance to FALSE
                       CREATE_NEW_CONSOLE,
                       // Give the updater its own console. Prevents from closing after the main app is closed.
                       NULL, // Use parent's environment block
                       NULL, // Use parent's starting directory
                       &si, // Pointer to STARTUPINFO structure
                       &pi) // Pointer to PROCESS_INFORMATION structure
    )
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        std::cout << "Updater launched. Exiting main application." << std::endl;
        exit(0);
    }
    else
    {
        std::cerr << "Failed to launch updater. Error: " << GetLastError() << std::endl;
    }
}
