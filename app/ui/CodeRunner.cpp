
//
// Created by talik on 5/28/2025.
//

#include <QIcon>

#include "CodeRunner.h"

CodeRunner::CodeRunner(QWidget *appUi) :
		IconButton(nullptr),
		appUi(appUi) {

	setIcon(QIcon(ItoolsNS::main_config.getAppIcons()->executeIcon));
	setFixedSize(32, 32);
	setStyleSheet("border: 0px;");

	// set tooltip for the run buttons
	setToolTip(
			"Run code."
			" & "
			"Highlighted code."
	);
}
