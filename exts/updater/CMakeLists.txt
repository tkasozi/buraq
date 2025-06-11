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

set(UPDATER_RESOURCES ${CMAKE_SOURCE_DIR}/app/resources.qrc)
set(UPDATER_SOURCES Updater.cpp ${UPDATER_RESOURCES}
    UpdaterProgressDialog.cpp
    UpdaterProgressDialog.h
    UpdateWorker.cpp
    UpdateWorker.h)
set(UPDATER_HEADERS ${CMAKE_SOURCE_DIR}/include/IToolsAPI.h)

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND WIN32)
    add_executable(updater WIN32 ${UPDATER_SOURCES} ${CMAKE_SOURCE_DIR}/app/res/ITools.rc)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    add_executable(updater ${UPDATER_SOURCES} ${CMAKE_SOURCE_DIR}/app/res/ITools.rc)
endif()

# Adds Qt, Boost, and Standard Library headers that are used everywhere
target_precompile_headers(updater PRIVATE ${CMAKE_SOURCE_DIR}/include/pch.h)

# Add this for MSVC builds to use the faster lld linker
# In your CMakeLists.txt, after add_executable(updater ...)

# Set linker options based on the compiler being used
if(MSVC)
    # For the Microsoft toolchain (link.exe), the flag is /lld
    target_link_options(updater PRIVATE "/lld")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # For Clang toolchains, the flag is -fuse-ld=lld
    target_link_options(updater PRIVATE "-fuse-ld=lld")
endif()

target_link_libraries(updater PRIVATE
    unofficial::minizip::minizip
    Qt::Core
    Qt::Gui
    Qt::Widgets
)
