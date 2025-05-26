//
// Created by talik on 3/2/2024.
//

#ifndef IT_TOOLS_EDITOR_H2
#define IT_TOOLS_EDITOR_H2

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QRect>
#include <QFile>
#include <QTimer>
#include <QRegularExpression>
#include <QStack>

#include "OutputDisplay.h"

class Editor : public QPlainTextEdit {

Q_OBJECT

protected:

	void focusInEvent(QFocusEvent *e) override;

	void keyPressEvent(QKeyEvent *e) override;

	void keyReleaseEvent(QKeyEvent *e) override;

	void mousePressEvent(QMouseEvent *e) override;

signals:
	void statusUpdate(QString status, int timeout = 10000);

	void readyToSaveEvent();

	void syntaxtHighlightingEvent();

	void inlineSyntaxtHighlightingEvent();

public:
	explicit Editor(QWidget *appUi = nullptr);

	~Editor() override  = default;

	void openAndParseFile(const QString &filePath, QFile::OpenModeFlag modeFlag = QFile::OpenModeFlag::ReadOnly);

	void lineNumberAreaPaintEvent(QPaintEvent *event);

	int lineNumberAreaWidth();

private slots:
	void setupSignals();

	void updateLineNumberAreaWidth(int newBlockCount);

	void highlightCurrentLine();

	void updateLineNumberArea(const QRect &rect, int dy);

	void documentSyntaxHighlighting();

	void inlineSyntaxHighlighting();

	void autoSave();

private:
	static QString convertTextToHtml(QString &);
	static QString convertRhsTextToHtml(const QString &);

	std::unique_ptr<QWidget> lineNumberArea;
	QWidget *appUi;
	QStack<QString> history;
	QIcon play;
	QString currentFile;
	QString previousText;
	QTimer autoSaveTimer;

};

#endif //IT_TOOLS_EDITOR_H2
