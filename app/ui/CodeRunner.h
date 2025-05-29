//
// Created by talik on 5/28/2025.
//

#ifndef CODERUNNER_H
#define CODERUNNER_H

#include <QWidget>
#include "IconButton.h"

class CodeRunner : public IconButton {

public:
	 explicit CodeRunner(QWidget *appUi);
	~CodeRunner() override = default;

private:
	// should be managed elsewhere
	QWidget *appUi;
};


#endif //CODERUNNER_H
