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
#include <sstream>
#include <filesystem> // Requires C++17. For older C++, use platform-specific directory iteration.
#include <QCoreApplication>
#include <map>
#include <QtSql>

#include <windows.h>
#include <string>

#include "client/VersionRepository.h"
#include "dialog/VersionUpdateDialog.h"

AppUi::AppUi(QWidget *parent) : QMainWindow(parent)
{

	// Other UI
	setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

	// This can be updated dynamically.
	QIcon::setThemeName("dark");

	// overall bgColor
	setStyleSheet("background-color: #232323;");

	// setting up default window size
	const auto windowConfig = ItoolsNS::main_config.getWindow();
	resize(windowConfig->normalSize, windowConfig->minHeight);
	setMinimumSize(windowConfig->minWidth, windowConfig->minHeight);

	// Add Tool bar
	toolBar = std::make_unique<ToolBar>(this);
	setMenuWidget(toolBar.get());

	// Add Status bar
	statusBar = std::make_unique<QStatusBar>();
	statusBar->setStyleSheet(ItoolsNS::main_config.getMainStyles()->statusToolBar.styleSheet);
	setStatusBar(statusBar.get());

	// Setting window title and docking icon
	setWindowTitle(ItoolsNS::main_config.getTitle());
	setWindowIcon(ItoolsNS::main_config.getAppLogo());

	// Add CentralWidget
	auto *centralWidget = new QWidget;
	setCentralWidget(centralWidget);

	// Add layout to centralWidget
	centralWidgetLayout = std::make_unique<QGridLayout>();
	centralWidgetLayout->setSpacing(0);
	centralWidgetLayout->setContentsMargins(0, 0, 0, 0);
	centralWidget->setLayout(centralWidgetLayout.get());

	// Add control panel to the central widget.
	auto *centralWidgetLayout2 = new QGridLayout;
	centralWidgetLayout2->setSpacing(0);
	centralWidgetLayout2->setContentsMargins(0, 0, 0, 0);

	auto *centralWidgetControlPanel = new QWidget;
	centralWidgetControlPanel->setStyleSheet(ItoolsNS::main_config.getMainStyles()->controlToolBar.styleSheet);
	centralWidgetControlPanel->setFixedWidth(64);
	centralWidgetControlPanel->setLayout(centralWidgetLayout2);

	// panel A
	auto *layoutA = new QVBoxLayout;
	layoutA->setSpacing(1);
	layoutA->setContentsMargins(0, 0, 0, 0);

	auto *centralWidgetControlPanelA = new QWidget;
	centralWidgetControlPanelA->setStyleSheet("border: none;");
	centralWidgetControlPanelA->setLayout(layoutA);

	centralWidgetLayout2->addWidget(centralWidgetControlPanelA, 0, 0, 1, 12, Qt::AlignJustify);

	folderButton = std::make_unique<IconButton>(QIcon(ItoolsNS::main_config.getAppIcons()->folderIcon));

	layoutA->addWidget(folderButton.get());

	// Good, to be placed in panel B
	auto *layoutB = new QVBoxLayout;
	layoutB->setSpacing(8);

	auto *centralWidgetControlPanelB = new QWidget;
	centralWidgetControlPanelB->setLayout(layoutB);
	centralWidgetControlPanelB->setContentsMargins(16, 0, 0, 8);

	centralWidgetLayout2->addWidget(centralWidgetControlPanelB, 11, 0, 12, 12, Qt::AlignBottom);

	auto outputButton = new IconButton(QIcon(ItoolsNS::main_config.getAppIcons()->terminalIcon));
	connect(outputButton, &IconButton::clicked, this, &AppUi::onShowOutputButtonClicked);
	layoutB->addWidget(outputButton);

	auto *settingButton = new IconButton(QIcon(ItoolsNS::main_config.getAppIcons()->settingsIcon));
	layoutB->addWidget(settingButton);

	// add to central widget
	centralWidgetLayout->addWidget(centralWidgetControlPanel, 0, 0, 12, 1);

	// Component
	connect(folderButton.get(), &IconButton::clicked, this, &AppUi::onClicked);

	// layout c
	layoutB->setSpacing(0);
	layoutB->setContentsMargins(0, 0, 0, 0);

	// Editor helper component.
	editorMargin = std::make_unique<EditorMargin>(this);
	// The text or script editor.
	itoolsEditor = std::make_unique<Editor>(this);
	// Handles file nav
	drawer = std::make_unique<CustomDrawer>(itoolsEditor.get());
	// This where the output_display generated after executing the script will be displayed
	outPutArea = std::make_unique<OutputDisplay>(this);

	placeHolderLayout = std::make_unique<QGridLayout>();
	placeHolderLayout->setSpacing(0);
	placeHolderLayout->setContentsMargins(0, 0, 0, 0);

	placeHolderLayout->addWidget(drawer.get(), 0, 1, 12, 1, Qt::AlignmentFlag::AlignTop);
	placeHolderLayout->addWidget(editorMargin.get(), 0, 2, 12, 1, Qt::AlignmentFlag::AlignTop);
	placeHolderLayout->addWidget(itoolsEditor.get(), 0, 3, 12, 1);

	// add main content area
	auto editorAndDrawerAreaPanel = new QWidget;
	editorAndDrawerAreaPanel->setLayout(placeHolderLayout.get());
	centralWidgetLayout->addWidget(editorAndDrawerAreaPanel, 0, 2, 12, 12);
	centralWidgetLayout->addWidget(outPutArea.get(), 6, 2, 6, 12);

	configureAppContext();

	// Schedule onWindowFullyLoaded to run after current event processing is done
	QTimer::singleShot(0, this, &AppUi::onWindowFullyLoaded);

	// Add a temporary message that disappears after 5 seconds
	processStatusSlot("Ready.", 2000);
}

void AppUi::onWindowFullyLoaded()
{
	// Perform your actions here
	// For example:
	// - Load data from a database/network without blocking the UI initially

	// MyCustomDialog customDlg(this); // 'this' would be the parent widget
	VersionUpdateDialog versionUpdater(this);
	VersionRepository repo(api_context.get());

	UpdateInfo info = repo.main_version_logic();

	if (!info.latestVersion.empty())
	{
		versionUpdater.setWindowTitle("A new version " + QString::fromStdString(info.latestVersion) + " is available!");
		versionUpdater.setContent(QString::fromStdString(info.releaseNotes));

		if (versionUpdater.exec() == QDialog::Accepted)
		{
			std::filesystem::path zipFile = repo.downloadNewVersion();

			launchUpdaterAndExit(
				api_context->searchPath / "updater.exe",
				zipFile,
				api_context->searchPath.parent_path());
		}
		else
		{
			// New version was rejected or modal was closed.
			// do nothing
		}
	}
}

void AppUi::onClicked()
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

void AppUi::onShowOutputButtonClicked()
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

Editor *AppUi::getEditor()
{
	return itoolsEditor.get();
}

PluginManager *AppUi::getLangPluginManager()
{
	return pluginManager.get();
}

void AppUi::processStatusSlot(const QString &message, const int timeout)
{
	statusBar->showMessage(message, timeout);
}

void AppUi::processResultSlot(int exitCode, const QString &output, const QString &error)
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

void AppUi::configureAppContext()
{
	std::filesystem::path userDataPath = std::filesystem::temp_directory_path() / "ITools";

	// app search_path for plugins
	std::filesystem::path searchPath(QCoreApplication::applicationDirPath().toStdString());
	// Add required plugins
	std::map<std::string, std::string> plugins_{
		{"power_shell", searchPath.string() + "/plugins/ext/libPowershellExt.dll"}};

	api_context = std::make_unique<IToolsApi>();
	api_context->searchPath = searchPath;
	api_context->plugins = plugins_;
	api_context->userDataPath = userDataPath / ".data";
	api_context->userPath = userDataPath;

	pluginManager = std::make_unique<PluginManager>(api_context.get());

	// Load plugins from the specified directory
	pluginManager->loadPlugin("power_shell");
}

EditorMargin *AppUi::getEditorMargin()
{
	return editorMargin.get();
}

void AppUi::launchUpdaterAndExit(const std::filesystem::path &updaterPath, const std::filesystem::path &userPath,
								 const std::filesystem::path &installPath)
{
	// TODO incomplete package path, should be set to the actual package path.
	const std::filesystem::path packagePath = userPath / "";
	std::string commandLine = "\"" + updaterPath.string() + "\"";
	commandLine += " \"" + packagePath.string() + "\"";
	commandLine += " \"" + installPath.string() + "\"";
	commandLine += " " + std::to_string(GetCurrentProcessId()); // Pass current process ID

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (CreateProcessA(NULL,					   // No module name (use command line)
					   (LPSTR)commandLine.c_str(), // Command line
					   NULL,					   // Process handle not inheritable
					   NULL,					   // Thread handle not inheritable
					   FALSE,					   // Set handle inheritance to FALSE
					   CREATE_NEW_CONSOLE,		   // Give the updater its own console. Prevents from closing after the main app is closed.
					   NULL,					   // Use parent's environment block
					   NULL,					   // Use parent's starting directory
					   &si,						   // Pointer to STARTUPINFO structure
					   &pi)						   // Pointer to PROCESS_INFORMATION structure
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
