project(IToolsEditor LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS
		Core
		Gui
		Widgets
		Sql
		Xml
		Svg
		REQUIRED)
find_package(Qt6SvgWidgets CONFIG REQUIRED)

qt_standard_project_setup() # Recommended for standard Qt setup

find_package(CURL REQUIRED)

find_package(Boost REQUIRED COMPONENTS property_tree)

set(ITOOLS_MAIN_SOURCES
		main.cpp
		PluginManager.cpp
		PluginManager.h
		FileObject.cpp
		ui/CommonWidget.h
)

set(ITOOLS_UI_SOURCES
		ui/app_ui/AppUi.cpp
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

set(ITOOLS_RESOURCES
		res/ITools.rc
		resources.qrc
)

set(ITOOLS_ALL_SOURCES
		${ITOOLS_MAIN_SOURCES}
		${ITOOLS_UI_SOURCES}
		${ITOOLS_UTILS_SOURCES}
		${ITOOLS_RESOURCES}
)

set(ITOOLS_HEADERS
		PluginManager.h
		FileObject.h
		ui/app_ui/AppUi.h
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
		${CMAKE_SOURCE_DIR}/include/version.h
		app_version.cpp
		app_version.h
		${CMAKE_SOURCE_DIR}/include/network.cpp
		${CMAKE_SOURCE_DIR}/include/network.h
		client/VersionRepository.cpp
		client/VersionRepository.h
		ui/dialog/VersionUpdateDialog.cpp
		ui/dialog/VersionUpdateDialog.h
)

if (CMAKE_BUILD_TYPE STREQUAL "Release" AND WIN32)
	add_executable(it-tools-editor WIN32 ${ITOOLS_ALL_SOURCES} ${ITOOLS_HEADERS})
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug" AND WIN32)
	add_executable(it-tools-editor ${ITOOLS_ALL_SOURCES} ${ITOOLS_HEADERS})
endif ()

# Adds Qt, Boost, and Standard Library headers that are used everywhere
#target_precompile_headers(it-tools-editor PRIVATE ${CMAKE_SOURCE_DIR}/include/pch.h)

# Embed resources into the executable
qt_add_resources(it-tools-editor RESOURCES resources.qrc)

# Set linker options based on the compiler being used
#if (MSVC)
#	# For the Microsoft toolchain (link.exe), the flag is /lld
#	target_link_options(it-tools-editor PRIVATE "/lld")
#elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#	# For Clang toolchains, the flag is -fuse-ld=lld
#	target_link_options(it-tools-editor PRIVATE "-fuse-ld=lld")
#endif ()


target_include_directories(it-tools-editor PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/ui" # For ui headers
		"${CMAKE_CURRENT_SOURCE_DIR}/utils" # For utils headers
		"${CMAKE_SOURCE_DIR}/include" # For PluginInterface.h, IToolsAPI.h
		"${CMAKE_CURRENT_SOURCE_DIR}" # For headers in the current source dir
)

target_link_libraries(it-tools-editor PRIVATE
		Qt6::Core
		Qt6::Gui
		Qt6::Widgets
		Qt6::Sql
		Qt6::Xml
		Qt6::Svg
		Qt6::SvgPrivate
		Qt6::SvgWidgets
		Qt6::SvgWidgetsPrivate
		CURL::libcurl
		Boost::property_tree
)

add_custom_target(app_icon ALL COMMAND
		${CMAKE_COMMAND} -E copy
		${PROJECT_SOURCE_DIR}/icons/workflow.ico
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
)
add_dependencies(it-tools-editor app_icon)

set(PLUGIN_BASE_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")

# Conditionally add static linking flags for MinGW/GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND WIN32)
	# Get the directory of the C++ compiler. In an MSYS2 MinGW setup,
	# this is typically where the required runtime DLLs are also located.
	get_filename_component(MINGW_RUNTIME_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

	message(STATUS "GCC detected, adding static linking flags for libgcc and libstdc++")
	target_link_options(it-tools-editor PRIVATE -static-libgcc -static-libstdc++ -lpthread)

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

			# Add a custom command to copy the DLL after 'it-tools-editor' is built
			add_custom_command(TARGET ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} POST_BUILD
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
	# --- DEPLOYMENT LOGIC ---
	# This command adds a new target to your project named "deploy"
	string(TOLOWER "${CMAKE_BUILD_TYPE}" BUILD_TYPE)
	add_custom_target(deploy_main #ALL
			COMMAND ${CMAKE_COMMAND} -E echo "Deploying for it-tools-editor $<CONFIG>..."
			COMMAND Qt6::windeployqt
			"--dir=plugins"
			# Use a generator expression to add the --release or --debug flag automatically
			# "--${BUILD_TYPE}"
#			"--no-translations"
#			"--no-opengl-sw"
#			"--svg"
			# Use a generator expression to get the path to your .exe file at build time
			"$<TARGET_FILE:it-tools-editor>"
			# This ensures the deployment command runs from the directory where the .exe is located
			WORKING_DIRECTORY "$<TARGET_FILE_DIR:it-tools-editor>"
			# This makes sure it-tools-editor.exe is built BEFORE this command tries to run
			DEPENDS it-tools-editor
			# This makes the output appear nicely in the terminal
			USES_TERMINAL
	)
endif ()
