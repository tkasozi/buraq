//
// Created by talik on 4/19/2024.
//

#ifndef IT_TOOLS_OUTPUTDISPLAY_H
#define IT_TOOLS_OUTPUTDISPLAY_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QPlainTextEdit>
#include <QScrollArea>

class OutputDisplay : public QWidget {
Q_OBJECT

public:
	~OutputDisplay() override = default;

	explicit OutputDisplay(QWidget *parent = nullptr);

	void toggle();

	void log(const QString &output, const QString &error);

private:
	// state is "error" or "default"
	static QLabel *createLabel(const QString &text, QString state = "default");
	QVBoxLayout *qResultLogsLayout;
	QScrollArea *scrollArea;
	QWidget *main;
};

#endif //IT_TOOLS_OUTPUTDISPLAY_H
