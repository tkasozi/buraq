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
    add_executable(${PROJECT_NAME} WIN32 ${UPDATER_SOURCES} ${CMAKE_SOURCE_DIR}/app/res/ITools.rc)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
    add_executable(${PROJECT_NAME} ${UPDATER_SOURCES} ${CMAKE_SOURCE_DIR}/app/res/ITools.rc)
endif()

# Adds Qt, Boost, and Standard Library headers that are used everywhere
target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include/pch.h)

# Add this for MSVC builds to use the faster lld linker
# In your CMakeLists.txt, after add_executable(${PROJECT_NAME} ...)

# Set linker options based on the compiler being used
if(MSVC)
    # For the Microsoft toolchain (link.exe), the flag is /lld
    target_link_options(${PROJECT_NAME} PRIVATE "/lld")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # For Clang toolchains, the flag is -fuse-ld=lld
    target_link_options(${PROJECT_NAME} PRIVATE "-fuse-ld=lld")
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    unofficial::minizip::minizip
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

if(WIN32)
    # --- DEPLOYMENT LOGIC ---
    # This command adds a new target to your project named "deploy"
    string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
    add_custom_target(deploy_updater #ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Deploying for ${PROJECT_NAME} $<CONFIG>..."
        # The COMMAND runs windeployqt using the Qt6::windeployqt target we found earlier
        COMMAND Qt6::windeployqt
        "--dir=plugins"
        # Use a generator expression to add the --release or --debug flag automatically
        "--${BUILD_TYPE}"
        # Add other useful flags as needed (e.g., --qmldir)
        # "--sqldrivers"
        "--no-translations"
        "--no-opengl-sw"
        # "--no-svg"
        # Use a generator expression to get the path to your .exe file at build time
        "$<TARGET_FILE:${PROJECT_NAME}>"
        # This ensures the deployment command runs from the directory where the .exe is located
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        # This makes sure ${PROJECT_NAME}.exe is built BEFORE this command tries to run
        DEPENDS ${PROJECT_NAME}
        # This makes the output appear nicely in the terminal
        USES_TERMINAL
    )
endif()
