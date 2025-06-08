project(updater)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    REQUIRED)

find_package(unofficial-minizip CONFIG REQUIRED)

set(UPDATER_RESOURCES ../../app/resources.qrc)
set(UPDATER_SOURCES Updater.cpp ${UPDATER_RESOURCES}
    UpdaterProgressDialog.cpp
    UpdaterProgressDialog.h
    UpdateWorker.cpp
    UpdateWorker.h)
set(UPDATER_HEADERS ../../include/IToolsAPI.h)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND WIN32)
    add_executable(updater WIN32 ${UPDATER_SOURCES} ../../app/res/ITools.rc)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    add_executable(updater ${UPDATER_SOURCES} ../../app/res/ITools.rc)
endif()

target_link_libraries(updater PRIVATE
    unofficial::minizip::minizip
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
