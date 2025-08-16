//
// Created by talik on 3/2/2024.
//

#ifndef IT_TOOLS_EDITOR_H2
#define IT_TOOLS_EDITOR_H2

#include <QPlainTextEdit>
#include <QWidget>
#include <QFile>
#include <QTimer>
#include <QRegularExpression>
#include <QStack>

#include "EditorMargin.h"
#include "buraq_api.h"

class Editor final : public QWidget {

Q_OBJECT

protected:

	// void focusInEvent(QFocusEvent *e) override;

	void keyPressEvent(QKeyEvent *e) override;

	void keyReleaseEvent(QKeyEvent *e) override;

	void mousePressEvent(QMouseEvent *e) override;

signals:
	void statusUpdate(QString status, int timeout = 10000);

	void readyToSaveEvent();

	void syntaxtHighlightingEvent();

	void inlineSyntaxtHighlightingEvent();

	void lineNumberAreaPaintEventSignal(const EditorState &state);


public:
	explicit Editor(QWidget *appUi = nullptr);

	~Editor() override  = default;

	void openAndParseFile(const QString &filePath, QFile::OpenModeFlag modeFlag = QFile::OpenModeFlag::ReadOnly);
	// Add forwarding methods if external code calls QPlainTextEdit methods on Editor
	[[nodiscard]] QString toPlainText() const { return m_plainTextEdit->toPlainText(); }
	[[nodiscard]] QString selectedText() const { return m_plainTextEdit->textCursor().selectedText(); }
	void setPlainText(const QString &text) const { m_plainTextEdit->setPlainText(text); }

private slots:

	void highlightCurrentLine();

	void documentSyntaxHighlighting();

	void inlineSyntaxHighlighting();

	void autoSave();

private:

	std::unique_ptr<QPlainTextEdit> m_plainTextEdit; // FIX: Internal QPlainTextEdit
	std::unique_ptr<EditorMargin> editorMargin;      // Your margin widget
	QWidget *appUi;
	QStack<QString> history;
	QString currentFile;
	QString previousText;
	QTimer autoSaveTimer;
	EditorState state;

	static QString convertTextToHtml(QString &);
	static QString convertRhsTextToHtml(const QString &);

	void setupSignals();
};

#endif //IT_TOOLS_EDITOR_H2
