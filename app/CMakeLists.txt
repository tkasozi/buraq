project(ITools)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
		Core
		Gui
		Widgets
		Sql
		Xml
		REQUIRED)

set(ITOOLS_MAIN_SOURCES
		main.cpp
		PluginManager.cpp
		PluginManager.h
		FileObject.cpp
		ui/CommonWidget.h
)

set(ITOOLS_UI_SOURCES
		ui/AppUi.cpp
		ui/ToolBarEventFilter.cpp
		ui/ToolBar.cpp
		ui/Editor.cpp
		ui/CustomDrawer.cpp
		ui/OutputDisplay.cpp
		ui/CustomLabel.cpp
		ui/EditorActionArea.cpp
)

set(ITOOLS_UTILS_SOURCES
		utils/Minion.cpp
		utils/Config.cpp
		utils/Utils.cpp
)

set(ITOOLS_RESOURCES resources.qrc)

set(ITOOLS_ALL_SOURCES
		${ITOOLS_MAIN_SOURCES}
		${ITOOLS_UI_SOURCES}
		${ITOOLS_UTILS_SOURCES}
		${ITOOLS_RESOURCES}
)

set(ITOOLS_HEADERS
		PluginManager.h
		FileObject.h
		ui/AppUi.h
		ui/ToolBarEventFilter.h
		ui/ToolButton.h
		ui/ToolBar.h
		ui/IconButton.h
		ui/Editor.h
		ui/LineNumberArea.h
		ui/CustomDrawer.h
		ui/FilePathLabel.h
		ui/OutputDisplay.h
		ui/CustomLabel.h
		ui/EditorActionArea.h
		ui/CommonWidget.h
		utils/Minion.h
)

if (WIN32)
	add_executable(ITools WIN32 ${ITOOLS_ALL_SOURCES} ${ITOOLS_HEADERS} res/ITools.rc)
endif ()

target_include_directories(ITools PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/ui"      # For ui headers
		"${CMAKE_CURRENT_SOURCE_DIR}/utils"   # For utils headers
		"${CMAKE_CURRENT_SOURCE_DIR}/../include" # For PluginInterface.h, IToolsAPI.h
		"${CMAKE_CURRENT_SOURCE_DIR}"         # For headers in the current source dir
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

set(PLUGIN_BASE_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")

# Conditionally add static linking flags for MinGW/GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
	# Get the directory of the C++ compiler. In an MSYS2 MinGW setup,
	# this is typically where the required runtime DLLs are also located.
	get_filename_component(MINGW_RUNTIME_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

	message(STATUS "GCC detected, adding static linking flags for libgcc and libstdc++")
	target_link_options(ITools PRIVATE -static-libgcc -static-libstdc++)

	set(QT_THIRD_PARTY_DEPENDENCY_DLLS
			"libb2-1.dll"
			"libdouble-conversion.dll"
			"libfreetype-6.dll"
			"libwinpthread-1.dll"
			"libharfbuzz-0.dll"
			"libmd4c.dll"
			"libpng16-16.dll"
	)

	# --- Bundle i.e. libwinpthread-1.dll ---
	foreach (DLL_FILENAME ${QT_THIRD_PARTY_DEPENDENCY_DLLS})
		set(PTHREAD_DLL_SOURCE_PATH "${MINGW_COMPILER_BIN_DIR}\\${DLL_FILENAME}")

		if (EXISTS "${PTHREAD_DLL_SOURCE_PATH}")
			message(STATUS "Found ${DLL_FILENAME} at ${PTHREAD_DLL_SOURCE_PATH}")

			# Add a custom command to copy the DLL after 'iTools' is built
			add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"${PTHREAD_DLL_SOURCE_PATH}"
					"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
					COMMENT "Manually copying Qt third-party dependency: ${PTHREAD_DLL_FILENAME}"
			)
		else ()
			message(WARNING "${PTHREAD_DLL_SOURCE_PATH} not found. You might need to bundle it manually or adjust the path.")
		endif ()
	endforeach ()

else ()
	message(STATUS "Compiler is not GCC, skipping static linking flags for libgcc and libstdc++ (${CMAKE_CXX_COMPILER_ID})")
endif ()

if (WIN32)
	foreach (QT_LIB Core Gui Widgets Sql Xml)
		get_target_property(QT_BINARY_DIR Qt6::${QT_LIB} IMPORTED_LOCATION_RELEASE)
		get_filename_component(QT_BIN_DIR ${QT_BINARY_DIR} DIRECTORY)

		message(STATUS, " ${QT_BINARY_DIR}")

		if (EXISTS "${QT_BINARY_DIR}")
			add_custom_command(TARGET ${CMAKE_ROOT_PROJECT_POSTFIX} POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy
					"${QT_BINARY_DIR}"
					"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
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
endif ()
