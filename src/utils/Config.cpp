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

#include "Config.h"
#include <QFile>
#include <QDomDocument>
#include <QDebug>

Config::Config() :
		windowConfig(new WindowConfig),
		appIcons(new AppIcons) {

	QFile file(":/config/main_config.xml");
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Error opening file:" << file.errorString();
		exit(-1);
	}

	QDomDocument configDoc;
	if (!configDoc.setContent(&file)) {
		file.close();
		qDebug() << "Error parsing XML file.";
		exit(-1);
	}

	// Get the root element (configuration)
	QDomElement root = configDoc.documentElement();
	if (root.tagName() != "configuration") {
		qDebug() << "Error: Root element is not 'configuration'.";
		exit(-1);
	}

	QDomNodeList appTitleList = root.elementsByTagName("Title");
	if (appTitleList.count() > 0) {
		QDomElement appTitleElement = appTitleList.at(0).toElement();
		this->title = QString(appTitleElement.text());
	} else {
		qDebug() << "Error: Configuration is missing a 'Title'.";
		exit(-1);
	}

	QDomNodeList appVersionList = root.elementsByTagName("Version");
	if (appVersionList.count() > 0) {
		QDomElement appVersionElement = appVersionList.at(0).toElement();
		this->version = QString(appVersionElement.text());
	} else {
		qDebug() << "Error: Configuration is missing a 'Version'.";
		exit(-1);
	}

	QDomNodeList appLogoList = root.elementsByTagName("AppLogo");
	if (appLogoList.count() > 0) {
		QDomElement appLogoElement = appLogoList.at(0).toElement();
		this->appLogo = QIcon::fromTheme(appLogoElement.text());
	} else {
		qDebug() << "Error: Configuration is missing a 'AppLogo'.";
		exit(-1);
	}

	QDomNodeList windowList = root.elementsByTagName("window");
	if (windowList.count() > 0) {
		QDomElement windowElement = windowList.at(0).toElement();
		this->processWindowAttr(windowElement);
	} else {
		qDebug() << "Error: Configuration is missing a 'window'.";
		exit(-1);
	}

	QDomNodeList appIconsList = root.elementsByTagName("AppIcons");
	if (appIconsList.count() > 0) {
		QDomElement appIconsElement = appIconsList.at(0).toElement();
		this->processAppIconsAttr(appIconsElement);
	} else {
		qDebug() << "Error: Configuration is missing a 'AppIcons'.";
		exit(-1);
	}

	file.close();
}

void Config::processWindowAttr(const QDomElement& element) {
	//element.attributes().item(1).nodeValue()
	for (int i = 0; i < element.attributes().size(); ++i) {
		QString attrName = element.attributes().item(i).nodeName();
		QString attrValue = element.attributes().item(i).nodeValue();

		if (attrName == "minWidth") {
			windowConfig->minWidth = attrValue.toInt();
		}
		if (attrName == "minHeight") {
			windowConfig->minHeight = attrValue.toInt();
		}
		if (attrName == "normalSize") {
			windowConfig->normalSize = attrValue.toInt();
		}
		if (attrName == "minimizeIcon") {
			windowConfig->minimizeIcon = QIcon::fromTheme(attrValue);
		}
		if (attrName == "maximizeIcon") {
			windowConfig->maximizeIcon = QIcon::fromTheme(attrValue);
		}
		if (attrName == "restoreIcon") {
			windowConfig->restoreIcon = QIcon::fromTheme(attrValue);
		}
		if (attrName == "closeIcon") {
			windowConfig->closeIcon = QIcon::fromTheme(attrValue);
		}
	}
}

void Config::processAppIconsAttr(const QDomElement& element) {
	auto appIconsNodes = element.childNodes();
	for (int i = 0; i < appIconsNodes.size(); ++i) {
		QDomElement qDomElement = appIconsNodes.item(i).toElement();
		QString attrName = qDomElement.tagName();

		if (attrName == "settings") {
			appIcons->settingsIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (attrName == "folder") {
			appIcons->folderIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (attrName == "terminal") {
			appIcons->terminalIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (attrName == "playCode") {
			appIcons->playCode = QIcon::fromTheme(qDomElement.text());
		}
		if (attrName == "execute") {
			appIcons->executeIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (attrName == "executeSelected") {
			appIcons->executeSelectedIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (attrName == "addFile") {
			appIcons->addFileIcon = QIcon::fromTheme(qDomElement.text());
		}
	}
}
