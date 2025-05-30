//
// Created by Talik Kasozi on 2/3/2024.
//
#include <QApplication>
#include <QStyleFactory>
#include "ui/AppUi.h"
#include "db_connection.h"
#include "app_version.h"
#include <QtSql>

int main(int argc, char *argv[])
{
	main_version_logic();

	QApplication app(argc, argv);

	if (!db_conn())
	{
		db_log("db_conn() EXIT_FAILURE..");
		// failed to connect to the database
		return EXIT_FAILURE;
	}

	// Initialize the database:
	QSqlError err = init_db();
	if (err.type() != QSqlError::NoError)
	{
		db_log("Error executing initializing db: " + QString(err.text()));

		return EXIT_FAILURE;
	}

	// configure default css
	QFile styleFile(":/styles.qss"); // Assuming the file is a resource
	styleFile.open(QIODevice::ReadOnly);
	app.setStyleSheet(styleFile.readAll());

	// user's home dir should be the default location when the app starts.
	// In the later release, save user's last dir/path
	std::filesystem::current_path(ItoolsNS::get_user_home_directory());

	std::unique_ptr<AppUi> ui = std::make_unique<AppUi>(nullptr);
	ui->show();


	return QApplication::exec();
}
