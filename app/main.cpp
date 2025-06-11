//
// Created by Talik Kasozi on 2/3/2024.
//
#include <QApplication>
#include <QStyleFactory>

#include "app_ui/AppUi.h"
#include "network.h"
#include "db_connection.h"
#include "client/VersionRepository.h"
#include "dialog/VersionUpdateDialog.h"
#include "app_version.h"

int main(int argc, char *argv[])
{
	// Ensure the singleton (and curl_global_init) is created before threads,
	// though Meyers singleton handles this.
	Network::singleton(); // Initialize network singleton if not already.

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
		db_log("Error executing initializing db:" + err.text().toStdString());
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
