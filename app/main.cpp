//
// Created by Talik Kasozi on 2/3/2024.
//
#include <QApplication>
#include <QStyleFactory>

#include "network.h"
#include "db_connection.h"
#include "client/VersionRepository.h"
#include "dialog/VersionUpdateDialog.h"
#include "app_version.h"
#include "app_ui/AppUi.h"
#include "utils/Config.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	// Create a custom palette
	QPalette darkPalette;
	darkPalette.setColor(QPalette::Window, QColor(53, 53, 53)); // Background of windows
	darkPalette.setColor(QPalette::WindowText, Qt::white);     // Text color on window background
	darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));   // Background for input fields, text views etc.
	darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53)); // Alternate background for items in lists/tables
	darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
	darkPalette.setColor(QPalette::ToolTipText, Qt::white);
	darkPalette.setColor(QPalette::Text, Qt::white);
	darkPalette.setColor(QPalette::Button, QColor(53, 53, 53)); // Button background
	darkPalette.setColor(QPalette::ButtonText, Qt::white);     // Button text
	darkPalette.setColor(QPalette::BrightText, Qt::red);       // Bright text color

	darkPalette.setColor(QPalette::Highlight, QColor(142, 45, 197)); // Selection highlight
	darkPalette.setColor(QPalette::HighlightedText, Qt::black);   // Text on selection highlight

	// Apply the custom palette to the application
	QApplication::setPalette(darkPalette);

	// It's generally cross-platform and modern-looking and better dark theme compatibility.
	QApplication::setStyle("Fusion");

	// Ensure the singleton (and curl_global_init) is created before threads,
	// though Meyers singleton handles this.
	Network::singleton(); // Initialize network singleton if not already.

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

	Config::singleton(); // Load configuration settings

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
