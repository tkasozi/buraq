project(ITools)

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

add_executable(ITools main.cpp
		resources.qrc
		ui/AppUi.cpp
		ui/AppUi.h
		ui/ToolBarEventFilter.cpp
		ui/ToolBarEventFilter.h
		ui/ToolButton.h
		ui/ToolBar.cpp
		ui/ToolBar.h
		ui/IconButton.h
		ui/Editor.cpp
		ui/Editor.h
		ui/LineNumberArea.h
		ui/CustomDrawer.cpp
		ui/CustomDrawer.h
		ui/FilePathLabel.h
		ui/OutputDisplay.h
		ui/CustomLabel.cpp
		ui/CustomLabel.h
		ui/EditorActionArea.cpp
		ui/EditorActionArea.h
		db_connection.h
		FileObject.h
		FileObject.cpp
		utils/Minion.cpp
		utils/Minion.h
		ui/OutputDisplay.cpp
		utils/Config.h
		utils/Config.cpp
		utils/Utils.cpp
		utils/Utils.h
		../include/PluginInterface.h
		PluginManager.cpp
		PluginManager.h
		../include/IToolsAPI.h
		ui/CommonWidget.h
)

target_link_libraries(ITools
		Qt::Core
		Qt::Gui
		Qt::Widgets
		Qt::Sql
		Qt::Xml
)

add_dependencies(ITools app_icon)
add_custom_target(app_icon ALL COMMAND
		${CMAKE_COMMAND} -E copy
		${PROJECT_SOURCE_DIR}/icons/workflow.ico
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

add_dependencies(ITools conf_file)
add_custom_target(conf_file ALL COMMAND
		${CMAKE_COMMAND} -E copy
		${PROJECT_SOURCE_DIR}/qt.conf
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)

# Embed resources into the executable
qt6_add_resources(ITools RESOURCES resources.qrc)

if (WIN32)
	foreach (QT_LIB Core Gui Widgets Sql Xml)
		get_target_property(QT_BINARY_DIR Qt6::${QT_LIB} IMPORTED_LOCATION)
		get_filename_component(QT_BIN_DIR ${QT_BINARY_DIR} DIRECTORY)
		#		message(STATUS, "*******************${QT_BIN_DIR}***************")

		message(STATUS, "${QT_BIN_DIR}/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll")
		if (EXISTS "${QT_BIN_DIR}/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll")
			add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy
					"${QT_BIN_DIR}/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
					"$<TARGET_FILE_DIR:${CMAKE_ROOT_PROJECT_POSTFIX}>")
		endif ()
	endforeach (QT_LIB)

	if (EXISTS "${QT_BIN_DIR}/../plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
		add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory
				"$<TARGET_FILE_DIR:${CMAKE_ROOT_PROJECT_POSTFIX}>/plugins/platforms/")
		add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy
				"${QT_BIN_DIR}/../plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
				"$<TARGET_FILE_DIR:${CMAKE_ROOT_PROJECT_POSTFIX}>/plugins/platforms/")
	endif ()
	if (EXISTS "${QT_BIN_DIR}/../plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll")
		add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory
				"$<TARGET_FILE_DIR:${CMAKE_ROOT_PROJECT_POSTFIX}>/plugins/sqldrivers/")
		add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy
				"${QT_BIN_DIR}/../plugins/sqldrivers/qsqlite${DEBUG_SUFFIX}.dll"
				"$<TARGET_FILE_DIR:${CMAKE_ROOT_PROJECT_POSTFIX}>/plugins/sqldrivers/")
	endif ()
	if (EXISTS "${QT_BIN_DIR}/../plugins/sqldrivers/qsqlpsql${DEBUG_SUFFIX}.dll")
		add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E make_directory
				"$<TARGET_FILE_DIR:${CMAKE_ROOT_PROJECT_POSTFIX}>/plugins/sqldrivers/")
		add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy
				"${QT_BIN_DIR}/../plugins/sqldrivers/qsqlpsql${DEBUG_SUFFIX}.dll"
				"$<TARGET_FILE_DIR:${CMAKE_ROOT_PROJECT_POSTFIX}>/plugins/sqldrivers/")
	endif ()
	# Add similar logic for other necessary plugin types (e.g., sqldrivers)
endif()
