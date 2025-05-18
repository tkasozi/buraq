cmake_minimum_required(VERSION 3.27)

project(ITools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
		Core
		Gui
		Widgets
		Sql
		Xml
		REQUIRED)

add_executable(ITools app/main.cpp
		resources.qrc
		app/ui/AppUi.cpp
		app/ui/AppUi.h
		app/ui/ToolBarEventFilter.cpp
		app/ui/ToolBarEventFilter.h
		app/ui/ToolButton.h
		app/ui/ToolBar.cpp
		app/ui/ToolBar.h
		app/ui/IconButton.h
		app/ui/Editor.cpp
		app/ui/Editor.h
		app/ui/LineNumberArea.h
		app/ui/CustomDrawer.cpp
		app/ui/CustomDrawer.h
		app/ui/FilePathLabel.h
		app/ui/OutputDisplay.h
		app/ui/CustomLabel.cpp
		app/ui/CustomLabel.h
		app/ui/EditorMargin.cpp
		app/ui/EditorMargin.h
		app/db_connection.h
		app/FileObject.h
		app/FileObject.cpp
		app/ProcessThread.cpp
		app/ProcessThread.h
		app/ui/OutputDisplay.cpp
		app/utils/Config.h
		app/utils/Config.cpp
		app/utils/Utils.cpp
		app/utils/Utils.h
		include/PluginInterface.h
		app/PluginManager.cpp
		app/PluginManager.h
		app/IToolsAPI.h
)
target_link_libraries(ITools
		Qt::Core
		Qt::Gui
		Qt::Widgets
		Qt::Sql
		Qt::Xml
)

add_subdirectory(exts/powershell)

add_dependencies(ITools theme_icons_32x32)
add_custom_target(theme_icons_32x32 ALL COMMAND
		${CMAKE_COMMAND} -E copy_directory
		${PROJECT_SOURCE_DIR}/icons/dark/svg
		Release/resources/themes/32x32/actions
)

add_dependencies(ITools theme_icons)
add_custom_target(theme_icons ALL COMMAND
		${CMAKE_COMMAND} -E copy_directory
		${PROJECT_SOURCE_DIR}/icons
		Release/resources
)

# Add resources
qt6_add_resources(ITools RESOURCES resources.qrc)

if (CMAKE_BUILD_TYPE MATCHES "Release" AND WIN32)
	foreach (QT_LIB Core Gui Widgets Sql Xml)
		get_target_property(QT_BINARY_DIR Qt6::${QT_LIB} IMPORTED_LOCATION)
		get_filename_component(QT_BIN_DIR ${QT_BINARY_DIR} DIRECTORY)
#		message(STATUS, "*******************${QT_BIN_DIR}***************")

		message(STATUS, "${QT_BIN_DIR}/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll")
		if (EXISTS "${QT_BIN_DIR}/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll")
			add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy
					"${QT_BIN_DIR}/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
					"$<TARGET_FILE_DIR:${PROJECT_NAME}>")
		endif ()
	endforeach (QT_LIB)

	if (EXISTS "${QT_BIN_DIR}/../plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory
				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy
				"${QT_BIN_DIR}/../plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
	endif ()
	if (EXISTS "${QT_BIN_DIR}/../plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory
				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy
				"${QT_BIN_DIR}/../plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
	endif ()
	if (EXISTS "${QT_BIN_DIR}/../plugins/sqldrivers/qsqlpsql${DEBUG_SUFFIX}.dll")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory
				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy
				"${QT_BIN_DIR}/../plugins/sqldrivers/qsqlpsql${DEBUG_SUFFIX}.dll"
				"$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/sqldrivers/")
	endif ()
	# Add similar logic for other necessary plugin types (e.g., sqldrivers)
endif()
