cmake_minimum_required(VERSION 3.27)
project(ITools)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
		Core
		Gui
		Widgets
		Sql
		REQUIRED)

add_executable(ITools src/main.cpp
		resources.qrc
		src/ui/IToolsUi.cpp
		src/ui/IToolsUi.h
		src/ui/itools_utils.h
		src/ui/ToolBarEventFilter.cpp
		src/ui/ToolBarEventFilter.h
		src/ui/ToolButton.h
		src/ui/ToolBar.cpp
		src/ui/ToolBar.h
		src/ui/IconButton.h
		src/ui/Editor.cpp
		src/ui/Editor.h
		src/ui/LineNumberArea.h
		src/ui/CustomDrawer.cpp
		src/ui/CustomDrawer.h
		src/ui/FilePathLabel.h
		src/ui/OutputDisplay.h
		src/ui/CustomLabel.cpp
		src/ui/CustomLabel.h
		src/ui/EditorMargin.cpp
		src/ui/EditorMargin.h
		src/db_conn.h
		src/FileObject.h
		src/FileObject.cpp
		src/ProcessThread.cpp
		src/ProcessThread.h
		src/ui/OutputDisplay.cpp
)
target_link_libraries(ITools
		Qt::Core
		Qt::Gui
		Qt::Widgets
		Qt::Sql
)

add_dependencies(ITools theme_icons_32x32)
add_custom_target(theme_icons_32x32 ALL COMMAND
		${CMAKE_COMMAND} -E copy_directory
		${PROJECT_SOURCE_DIR}/icons/dark/svg
		${CMAKE_BINARY_DIR}/resources/themes/32x32/actions
)

add_dependencies(ITools theme_icons)
add_custom_target(theme_icons ALL COMMAND
		${CMAKE_COMMAND} -E copy_directory
		${PROJECT_SOURCE_DIR}/icons
		${CMAKE_BINARY_DIR}/resources
)

# create a file name temp.db in the binary dir
file(WRITE ${CMAKE_BINARY_DIR}/0a97fd39-aac6-463c-9b91-a3f8a7649ef0.db "")

# Add resources
qt6_add_resources(ITools RESOURCES resources.qrc)

#if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
#	set(DEBUG_SUFFIX)
#	if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
#		set(DEBUG_SUFFIX "d")
#	endif ()
#
#	set(QT_INSTALL_PATH "${CMAKE_INSTALL_PREFIX}")
#	if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
#		set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
#		if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
#			set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
#		endif ()
#	endif ()
#	if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
#		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#				COMMAND ${CMAKE_COMMAND} -E make_directory
#				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
#		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#				COMMAND ${CMAKE_COMMAND} -E copy
#				"${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
#				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
#	endif ()
#	if (EXISTS "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll")
#		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#				COMMAND ${CMAKE_COMMAND} -E make_directory
#				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
#		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#				COMMAND ${CMAKE_COMMAND} -E copy
#				"${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
#				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
#	endif ()
#	if (EXISTS "${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlpsql${DEBUG_SUFFIX}.dll")
#		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#				COMMAND ${CMAKE_COMMAND} -E make_directory
#				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
#		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#				COMMAND ${CMAKE_COMMAND} -E copy
#				"${QT_INSTALL_PATH}/plugins/sqldrivers/qsqlpsql${DEBUG_SUFFIX}.dll"
#				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
#	endif ()
#	foreach (QT_LIB Core Gui Widgets Sql)
#		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#				COMMAND ${CMAKE_COMMAND} -E copy
#				"${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
#				"$<TARGET_FILE_DIR:${PROJECT_NAME}>")
#	endforeach (QT_LIB)
#endif ()
