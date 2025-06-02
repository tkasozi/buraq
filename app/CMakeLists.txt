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


find_package(CURL REQUIRED)

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
		ui/editor/Editor.cpp
		ui/CustomDrawer.cpp
		ui/output_display/OutputDisplay.cpp
		ui/CustomLabel.cpp
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
		ui/editor/Editor.h
		ui/EditorMargin.h
		ui/CustomDrawer.h
		ui/FilePathLabel.h
		ui/output_display/OutputDisplay.h
		ui/CustomLabel.h
		ui/CommonWidget.h
		utils/Minion.h
		ui/editor/CodeRunner.cpp
		ui/editor/CodeRunner.h
		ui/EditorMargin.cpp
		ui/editor/LineNumberAreaWidget.cpp
		ui/editor/LineNumberAreaWidget.h
		ui/frameless_window/FramelessWindow.cpp
		ui/frameless_window/FramelessWindow.h
		../include/version.h
		app_version.cpp
		app_version.h
		../include/network.cpp
		../include/network.h
		client/VersionRepository.cpp
		client/VersionRepository.h
		ui/dialog/VersionUpdateDialog.cpp
		ui/dialog/VersionUpdateDialog.h
)

if (CMAKE_BUILD_TYPE STREQUAL "Release" AND WIN32)
	add_executable(ITools WIN32 ${ITOOLS_ALL_SOURCES} ${ITOOLS_HEADERS} res/ITools.rc)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
	add_executable(ITools ${ITOOLS_ALL_SOURCES} ${ITOOLS_HEADERS} res/ITools.rc)
endif ()

target_include_directories(ITools PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/ui" # For ui headers
		"${CMAKE_CURRENT_SOURCE_DIR}/utils" # For utils headers
		"${CMAKE_CURRENT_SOURCE_DIR}/../include" # For PluginInterface.h, IToolsAPI.h
		"${CMAKE_CURRENT_SOURCE_DIR}" # For headers in the current source dir
)

target_link_libraries(ITools
		Qt::Core
		Qt::Gui
		Qt::Widgets
		Qt::Sql
		Qt::Xml
		CURL::libcurl
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
	target_link_options(ITools PRIVATE -static-libgcc -static-libstdc++ -lpthread)

	set(QT_THIRD_PARTY_DEPENDENCY_DLLS
			"libb2-1.dll"
			"libdouble-conversion.dll"
			"libfreetype-6.dll"
			"libwinpthread-1.dll"
			"libharfbuzz-0.dll"
			"libmd4c.dll"
			"libpng16-16.dll"
			"libstdc++-6.dll"
			"libzlib1.dll"
			"libbrotlidec.dll"
			"libicuuc77.dll"
			"libicuin77.dll"
			"libicudt77.dll"
			"libpcre2-16-0.dll"
			"libpcre2-8-0.dll"
			"libbz2-1.dll"
			"libgraphite2.dll"
			"libzstd.dll"
			"zlib1.dll"
			"libgcc_s_seh-1.dll"
			"libglib-2.0-0.dll"
			"libbrotlicommon.dll"
			"libsqlite3-0.dll"
			"libintl-8.dll"
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
					COMMENT "Manually copying Qt third-party dependency: ${PTHREAD_DLL_SOURCE_PATH}"
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
