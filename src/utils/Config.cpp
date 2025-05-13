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
		mainStyles(new MainStyles),
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

	QDomNodeList powershellPathList = root.elementsByTagName("powershellPath");
	if (powershellPathList.count() > 0) {
		QDomElement powershellPathElement = powershellPathList.at(0).toElement();
		this->powershellPath = QString(powershellPathElement.text());
	} else {
		qDebug() << "Error: Configuration is missing a 'powershellPath'.";
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

	QDomNodeList mainStylesList = root.elementsByTagName("Styles");
	if (mainStylesList.count() > 0) {
		QDomElement mainStylesElement = mainStylesList.at(0).toElement();
		this->processStyles(mainStylesElement);
	} else {
		qDebug() << "Error: Configuration is missing a 'Styles'.";
		exit(-1);
	}

	file.close();
}

void Config::processWindowAttr(const QDomElement &element) {
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

void Config::processAppIconsAttr(const QDomElement &element) {
	auto appIconsNodes = element.childNodes();
	for (int i = 0; i < appIconsNodes.size(); ++i) {
		QDomElement qDomElement = appIconsNodes.item(i).toElement();
		QString tagName = qDomElement.tagName();

		if (tagName == "settings") {
			appIcons->settingsIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (tagName == "folder") {
			appIcons->folderIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (tagName == "terminal") {
			appIcons->terminalIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (tagName == "playCode") {
			appIcons->playCode = QIcon::fromTheme(qDomElement.text());
		}
		if (tagName == "execute") {
			appIcons->executeIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (tagName == "executeSelected") {
			appIcons->executeSelectedIcon = QIcon::fromTheme(qDomElement.text());
		}
		if (tagName == "addFile") {
			appIcons->addFileIcon = QIcon::fromTheme(qDomElement.text());
		}
	}
}

void Config::processStyles(const QDomElement &element) {
	auto styleNodes = element.childNodes();
	QDomElement qDomCommonNode;
	for (int i = 0; i < styleNodes.size(); ++i) {
		auto temp = styleNodes.item(i).toElement();
		if (temp.tagName() == "commonStyle") {
			processStyleBlock(temp, mainStyles->commonStyle);
			break;
		}
	}

	for (int i = 0; i < styleNodes.size(); ++i) {
		auto temp = styleNodes.item(i).toElement();
		if (temp.tagName() != "commonStyle") {
			if (temp.tagName() == "controlToolBar") {
				processStyleBlock(temp, mainStyles->controlToolBar);
			}
			if (temp.tagName() == "toolBar") {
				processStyleBlock(temp, mainStyles->toolBar);
			}
			if (temp.tagName() == "statusToolBar") {
				processStyleBlock(temp, mainStyles->statusToolBar);
			}
			if (temp.tagName() == "toolBarHover") {
				processStyleBlock(temp, mainStyles->toolBarHover);
			}
		}
	}
}

void Config::processStyleBlock(QDomElement &element, StyleSheetStruct &aStruct) {
	// includes styles
	if (element.hasAttribute("inherits") && element.attributes().item(0).nodeValue() == "commonStyle") {
		aStruct.color = mainStyles->commonStyle.color;
		aStruct.backgroundColor = mainStyles->commonStyle.backgroundColor;
		aStruct.padding = mainStyles->commonStyle.padding;
		aStruct.border = mainStyles->commonStyle.border;
		aStruct.height = mainStyles->commonStyle.height;
	}

	auto styleNodes = element.childNodes();
	for (int i = 0; i < styleNodes.size(); ++i) {
		auto temp = styleNodes.item(i).toElement();
		if (temp.tagName() == "backgroundColor") {
			aStruct.backgroundColor = QString("background-color:" + temp.text() + ";");
		}
		if (temp.tagName() == "border") {
			aStruct.border = QString("border: " + temp.text() + ";");
		}
		if (temp.tagName() == "borderBottom") {
			aStruct.borderBottom = QString("border-bottom:" + temp.text() + ";");
		}
		if (temp.tagName() == "borderTop") {
			aStruct.borderBottom = QString("border-top:" + temp.text() + ";");
		}
		if (temp.tagName() == "borderColor") {
			aStruct.borderColor = QString("border-color:" + temp.text() + ";");
		}
		if (temp.tagName() == "padding") {
			aStruct.padding = QString("padding:" + temp.text() + ";");
		}
		if (temp.tagName() == "height") {
			aStruct.height = QString("height:" + temp.text() + ";");
		}

		aStruct.styleSheet = aStruct.backgroundColor + aStruct.padding
							 + aStruct.color
							 + aStruct.border
							 + aStruct.height
							 + aStruct.borderColor
							 + aStruct.borderBottom;
	}
}
