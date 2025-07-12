//
// Created by Talik Kasozi on 2/3/2024.
//
#ifndef APP_UI_H
#define APP_UI_H

#include <QStatusBar>

#include "ToolBarEventFilter.h"
#include "CustomDrawer.h"
#include "IconButton.h"
#include "output_display/OutputDisplay.h"
#include "ToolBar.h"
#include "PluginManager.h"
#include "EditorMargin.h"


class AppUi : public QMainWindow {
Q_OBJECT

public slots:
	void processStatusSlot(const QString &, int timeout = 5000) const;
	void processResultSlot(int exitCode, const QString &output, const QString &error) const;

private slots:

	void onClicked() const;

	void onShowOutputButtonClicked() const;

	void onWindowFullyLoaded();

public:
	explicit AppUi(QWidget *parent = nullptr);

	// The std::unique_ptr members will automatically
	// delete their managed objects when AppUi is destroyed.
	// No manual delete, no manual nullptr.
	~AppUi() override = default;

	Editor *getEditor() const;
	EditorMargin *getEditorMargin() const;

	PluginManager *getLangPluginManager() const;

	IToolsApi *get_api_context() { return api_context.get(); };
private:
	std::unique_ptr<PluginManager> pluginManager;
	std::unique_ptr<CustomDrawer> drawer;
	std::unique_ptr<QGridLayout> centralWidgetLayout;
	std::unique_ptr<IconButton> folderButton;
	std::unique_ptr<OutputDisplay> outPutArea;
	std::unique_ptr<QGridLayout> placeHolderLayout;
	std::unique_ptr<Editor> itoolsEditor;
	std::unique_ptr<EditorMargin> editorMargin;
	std::unique_ptr<ToolBar> toolBar;
	std::unique_ptr<QStatusBar> statusBar;
	std::unique_ptr<IToolsApi> api_context;

	void configureAppContext();
	static void launchUpdaterAndExit(const  std::filesystem::path &updaterPath, const  std::filesystem::path &packagePath,
								 const  std::filesystem::path &installPath);
};


#endif // APP_UI_H
