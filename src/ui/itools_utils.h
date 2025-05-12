//
// Created by talik on 2/6/2024.
//

#ifndef ITOOLS_NAMESPACE
#define ITOOLS_NAMESPACE

namespace IToolsNamespace {
	// Set the executable path to powershell.exe
	constexpr const char *POWERSHELL_PATH = "C:/Windows/System32/WindowsPowerShell/v1.0/powershell.exe";

	namespace Styles {

		const QString commonToolBarStyles =
				"color: #C2C2C2;"
				"background-color: none;"
				"padding: 0px;"
				"border: none;";

		const QString centralControlToolBar = commonToolBarStyles +
											  "border-right: 1px solid #383838;";

		const QString toolBarBackgroundColor = commonToolBarStyles +
											   "height: 32px;"
											   "border-bottom: 1px solid #383838;";

		const QString toolBarHoverColor =
				"border: 0px;"
				"background-color: #383838;";

		const QString statusToolBar = commonToolBarStyles +
									  "border-top: 1px solid #383838;"
									  "height: 24px;";
	}

	namespace AppIcons {
		const QString minimize = "minimize_icon";
		const QString maximize = "maximize_icon";
		const QString restore = "restore_icon";
		const QString close = "close_icon";
		const QString play = "play_filled_icon";
		const QString playSelectedScript = "play_filled_icon2";
		const QString playWholeScript = "play_filled_icon3";
		const QString createNew = "create_new_icon";
	}
}

#endif // ITOOLS_NAMESPACE
