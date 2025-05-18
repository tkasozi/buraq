//
// Created by Talik Kasozi on 2/3/2024.
//
#include <QApplication>
#include <QStyleFactory>
#include "ui/AppUi.h"
#include "db_connection.h"
#include "PluginManager.h"
#include "IToolsAPI.h"
#include <QtSql>

int main(int argc, char *argv[]) {
	QApplication app(argc, argv);

	if(!db_conn()) {
		// failed to connect to the database
		return EXIT_FAILURE;
	}

	// Initialize the database:
	QSqlError err = init_db();
	if (err.type() != QSqlError::NoError) {
		qDebug() << "Error executing initializing db:" << err.text();
		return EXIT_FAILURE;
	}

	QApplication::setStyle(QStyleFactory::create("Windows"));

	// configure default css
	QFile styleFile(":/styles.qss");  // Assuming the file is a resource
	styleFile.open(QIODevice::ReadOnly);
	app.setStyleSheet(styleFile.readAll());

	// Interact with loaded plugins

	AppUi ui(nullptr);
	ui.show();


	return QApplication::exec();
}
