project(PowershellExt LANGUAGES CXX)

set(MANAGED_LIBRARY_PUBLISH_DIR "${CMAKE_HOME_DIRECTORY}/ManagedLibrary/ManagedLibraryPublish") # Path from dotnet publish -o

# --- Define the shared library target ---
# The first argument is the target name. CMake will automatically name the file
# appropriately for the platform (e.g., my_cool_plugin.dll, libmy_cool_plugin.so, libmy_cool_plugin.dylib).
add_library(PowershellExt SHARED
		PowershellExt.cpp PowershellExt.h
)

# Assuming MainApp.exe is in CMAKE_BINARY_DIR for single-config MinGW:
set(PLUGIN_BASE_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/plugins")
set(PLUGIN_SUBDIR "ext")
set(PLUGIN_FULL_OUTPUT_DIR "${PLUGIN_BASE_OUTPUT_DIR}/${PLUGIN_SUBDIR}")

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
set(NET_HOST_DLL "${CMAKE_DOTNET_TARGET_FRAMEWORK}/nethost.dll")
set(NET_HOST_LIB "${CMAKE_DOTNET_TARGET_FRAMEWORK}/nethost.lib")

if (EXISTS "${NET_HOST_LIB}")
	target_link_libraries(PowershellExt PRIVATE "${NET_HOST_LIB}")
else ()
	message(FATAL_ERROR "${NET_HOST_DLL}not found. Please check your .NET SDK installation.")
endif ()

# Check if the library file exists
if (EXISTS "${NET_HOST_DLL}")
	# Link the nethost library to your PowershellExt target

	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory "${PLUGIN_FULL_OUTPUT_DIR}" # Ensure directory exists
			COMMAND ${CMAKE_COMMAND} -E copy
			"${NET_HOST_DLL}"
			"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
			COMMENT "Copying ${NET_HOST_DLL} to ${PLUGIN_SUBDIR}"
			VERBATIM)
else ()
	message(FATAL_ERROR "nethost.dll not found at ${NET_HOST_DLL}. Please check CMAKE_DOTNET_TARGET_FRAMEWORK and your .NET SDK installation.")
endif ()

# On Windows, this ensures that the functions are exported from the DLL.
# On other platforms (Linux, macOS), __attribute__((visibility("default"))) is typically used.
if (WIN32)
	target_compile_definitions(PowershellExt PRIVATE POWERSHELL_PLUGIN_EXPORTS)
endif ()

# Glob all files in the source assets directory
file(GLOB ASSET_FILES "${MANAGED_LIBRARY_PUBLISH_DIR}/*") # Gets files directly in the folder

# Ensure destination directory exists
file(MAKE_DIRECTORY "${PLUGIN_FULL_OUTPUT_DIR}/ManagedLibraryPublish")

# Loop through the globbed files and copy them
foreach(ASSET_FILE ${ASSET_FILES})
	file(COPY "${ASSET_FILE}" DESTINATION "${PLUGIN_FULL_OUTPUT_DIR}/ManagedLibraryPublish")
endforeach()

# On Windows, also place the PDB (Program Database for debugging) file alongside the DLL.
if (MSVC)
	set_target_properties(PowershellExt PROPERTIES
			PDB_OUTPUT_DIRECTORY "${PLUGIN_FULL_OUTPUT_DIR}/plugins/ext"
	)
endif ()
