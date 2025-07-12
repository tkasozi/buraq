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

#ifndef ITOOLS_CONFIG_H
#define ITOOLS_CONFIG_H

#include <QString>
#include <QIcon>
#include <QDomElement>

struct WindowConfig
{
	int minWidth;
	int minHeight;
	int normalSize;
	QIcon minimizeIcon;
	QIcon maximizeIcon;
	QIcon restoreIcon;
	QIcon closeIcon;
};
struct AppIcons
{
	QIcon settingsIcon;
	QIcon folderIcon;
	QIcon terminalIcon;
	QIcon playCode;
	QIcon executeIcon;
	QIcon executeSelectedIcon;
	QIcon addFileIcon;
};
struct StyleSheetStruct
{
	QString color = QString("");
	QString backgroundColor = QString("");
	QString padding = QString("");
	QString border = QString("");
	QString height = QString("");
	QString borderColor = QString("");
	QString borderBottom = QString();
	/**
	 * String combining all the styleSheet object's properties
	 *
	 */
	QString styleSheet;
};
struct MainStyles
{
	StyleSheetStruct commonStyle;
	StyleSheetStruct controlToolBar;
	StyleSheetStruct toolBar;
	StyleSheetStruct statusToolBar;
	StyleSheetStruct toolBarHover;
};

class Config
{

public:
	/**
	 * @brief Returns the singleton instance of the Config class.
	 *
	 * This static method provides access to the single, shared instance of the Config class,
	 * ensuring that only one instance exists throughout the application's lifetime.
	 *
	 * @return Config The singleton instance of the Config class.
	 */
	static Config &singleton();

	[[nodiscard]] const QString &getTitle() const
	{
		return title;
	}

	[[nodiscard]] QString getVersion() const
	{
		return version;
	}

	[[nodiscard]] const QString &getPowershellPath() const
	{
		return powershellPath;
	}

	[[nodiscard]] const QIcon &getAppLogo() const
	{
		return appLogo;
	}

	[[nodiscard]] const WindowConfig *getWindow() const
	{
		return windowConfig.get();
	}

	[[nodiscard]] const AppIcons *getAppIcons() const
	{
		return appIcons.get();
	}

	[[nodiscard]] const MainStyles *getMainStyles() const
	{
		return mainStyles.get();
	}
private:
	Config();
	// smart pointer tobe cleaned up
	~Config() = default;
	Config(const Config &) = delete;			// No copy constructor
	Config &operator=(const Config &) = delete; // No copy assignment

	bool isSetup = false;
	QString title;
	QString version;
	QString powershellPath;
	QIcon appLogo;
	std::unique_ptr<WindowConfig> windowConfig;
	std::unique_ptr<AppIcons> appIcons;
	std::unique_ptr<MainStyles> mainStyles;

	void processWindowAttr(const QDomElement &) const;

	void processAppIconsAttr(const QDomElement &element) const;

	void processStyles(const QDomElement &element);

	void processStyleBlock(QDomElement &element, StyleSheetStruct &aStruct) const;

	static void loadConfig(Config *_thi);
};

#endif // ITOOLS_CONFIG_H
