//
// Created by Talik Kasozi on 2/3/2024.
//

#include "ui/Filters/ThemeManager/ThemeManager.h"
#include "app_ui/AppUi.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // Set these before creating any QSettings objects
    QCoreApplication::setOrganizationName("BizAura.app Inc");
    QCoreApplication::setApplicationName("Buraq Editor");

    AppUi appUi{};

    return QApplication::exec();
}
