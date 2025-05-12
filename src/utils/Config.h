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

struct WindowConfig {
	int minWidth;
	int minHeight;
	int normalSize;
	QIcon minimizeIcon;
	QIcon maximizeIcon;
	QIcon restoreIcon;
	QIcon closeIcon;
};
struct AppIcons {
	QIcon settingsIcon;
	QIcon folderIcon;
	QIcon terminalIcon;
	QIcon playCode;
	QIcon executeIcon;
	QIcon executeSelectedIcon;
	QIcon addFileIcon;
};

class Config {

public:
	explicit Config() ;

	virtual ~Config() {
		delete windowConfig;
		windowConfig = nullptr;
		delete appIcons;
		appIcons = nullptr;
	};

	[[nodiscard]] const QString &getTitle() const {
		return title;
	}

	[[nodiscard]] const QString &getVersion() const {
		return version;
	}

	[[nodiscard]] const QIcon &getAppLogo() const {
		return appLogo;
	}

	[[nodiscard]] const WindowConfig &getWindow() const {
		return *windowConfig;
	}

	[[nodiscard]] const AppIcons &getAppIcons() const {
		return *appIcons;
	}

private:
	QString title;
	QString version;
	QIcon appLogo;
	WindowConfig *windowConfig;
	AppIcons *appIcons;

	void processWindowAttr(const QDomElement&);

	void processAppIconsAttr(const QDomElement &element);
};

#endif //ITOOLS_CONFIG_H
