cmake_minimum_required(VERSION 3.27)

project(PowershellExt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
#set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

#find_package(Qt6 COMPONENTS
#		Core
#		Gui
#		Widgets
#		Xml
#		REQUIRED)

# --- Define the shared library target ---
# The first argument is the target name. CMake will automatically name the file
# appropriately for the platform (e.g., my_cool_plugin.dll, libmy_cool_plugin.so, libmy_cool_plugin.dylib).
add_library(PowershellExt SHARED
		PowershellExt.cpp PowershellExt.h
)

# CMAKE_SOURCE_DIR refers to the top-level source directory (MyCppApplication/)
# when this CMakeLists.txt is included by add_subdirectory() from the top-level CMakeLists.txt.
set(CMAKE_DOTNET_TARGET_FRAMEWORK "C:\\Program Files\\dotnet\\packs\\Microsoft.NETCore.App.Host.win-x64\\9.0.5\\runtimes\\win-x64\\native")

# --- Include Directories ---
# Tell CMake where to find header files.
message("***** Should set dynamically ***** ${CMAKE_DOTNET_TARGET_FRAMEWORK}")

target_include_directories(PowershellExt PUBLIC
		"${CMAKE_DOTNET_TARGET_FRAMEWORK}"
		"${CMAKE_SOURCE_DIR}/include"
)

#    CMake can usually handle the .lib extension directly.
#    Provide the full path to nethost.lib.
set(NETHOST_LIB "${CMAKE_DOTNET_TARGET_FRAMEWORK}/nethost.dll")

# Check if the library file exists
if (EXISTS "${NETHOST_LIB}")
	# Link the nethost library to your PowershellExt target
	target_link_libraries(PowershellExt PRIVATE "${NETHOST_LIB}")

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${NETHOST_LIB}"
			"${CMAKE_BINARY_DIR}/plugins/ext")
else ()
	message(FATAL_ERROR "nethost.dll not found at ${NETHOST_LIB}. Please check CMAKE_DOTNET_TARGET_FRAMEWORK and your .NET SDK installation.")
endif ()

# On Windows, this ensures that the functions are exported from the DLL.
# On other platforms (Linux, macOS), __attribute__((visibility("default"))) is typically used.
if (WIN32)
	target_compile_definitions(PowershellExt PRIVATE POWERSHELL_PLUGIN_EXPORTS)
endif ()


#target_link_libraries(PowershellExt PRIVATE
#		Qt6::Core
#		Qt6::Gui
#		Qt6::Widgets
#		Qt6::Sql
#		Qt6::Xml
#)

# --- Custom commands to copy ManagedLibrary.dll etc. ---
set(CSHARP_PROJECT_OUTPUT_DIR "C:\\Users\\talik\\workspace\\ITools\\ManagedLibrary\\bin\\Debug\\net9.0") # Adjust as needed
set(MANAGED_FILES
		"${CSHARP_PROJECT_OUTPUT_DIR}\\ManagedLibrary.dll"
		"${CSHARP_PROJECT_OUTPUT_DIR}\\ManagedLibrary.deps.json"
)

foreach (MANAGED_FILE ${MANAGED_FILES})
	if (EXISTS "${MANAGED_FILE}")
		add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
				"${MANAGED_FILE}"
				"${CMAKE_BINARY_DIR}/plugins/ext")
	else ()
		message(WARNING "Managed file not found for copying: ${MANAGED_FILE}")
	endif ()
endforeach ()


# On Windows, also place the PDB (Program Database for debugging) file alongside the DLL.
if (MSVC)
	set_target_properties(PowershellExt PROPERTIES
			PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/ext"
	)
endif ()

# --- Set Output Directories ---
# This ensures the compiled plugin (.dll, .so, .dylib) is placed in a 'plugins'
# subdirectory within the main build output directory (CMAKE_BINARY_DIR refers to the top-level build dir).
# Your main application will then look for plugins in <executable_location>/plugins/.
set_target_properties(PowershellExt PROPERTIES
		LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/ext" # For .so, .dylib, and DLL's .lib import library
		RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/ext" # For .dll files on Windows (which are runtime artifacts)
		ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/ext" # For static libs, or .lib on MSVC for DLLs
)
