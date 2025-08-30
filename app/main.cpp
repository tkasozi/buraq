//
// Created by Talik Kasozi on 2/3/2024.
//
#include <QApplication>
#include <QDir>

#include "network.h"
#include "database/db_conn.h"
#include "theme_manager.h"
#include "Utils.h"
#include "clients/VersionClient/VersionRepository.h"
#include "app_ui/AppUi.h"
#include "utils/Config.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Initialize the ThemeManager instance
    ThemeManager& themeManager = ThemeManager::instance();
    app.installEventFilter(&themeManager);

    // Set the initial theme. This ensures the correct stylesheet is loaded
    // at startup, even before any system palette change events might occur.
    themeManager.setAppTheme(themeManager.getThemeFromPalette(app.palette()));
    // Initialize based on current system palette

    // Load configuration settings
    Config::singleton();

    // Ensure the singleton (and curl_global_init) is created before threads,
    // though Meyers singleton handles this.
    Network::singleton(); // Initialize network singleton if not already.

    using file_utils::file_log;
    if (!database::db_conn())
    {
        file_log("db_conn() EXIT_FAILURE..");
        // failed to connect to the database
        return EXIT_FAILURE;
    }

    // Initialize the database:
    if (const QSqlError err = database::init_db(); err.type() != QSqlError::NoError)
    {
        file_log("Error executing initializing db:" + err.text().toStdString());
        return EXIT_FAILURE;
    }

    // user's home dir should be the default location when the app starts.
    // In the later release, save user's last dir/path
    std::filesystem::current_path(ItoolsNS::get_user_home_directory());

    const auto ui = std::make_unique<AppUi>(nullptr);
    ui->show();

    return QApplication::exec();
}
