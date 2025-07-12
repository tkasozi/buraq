project(powershell_plugin LANGUAGES CXX)

set(MANAGED_LIBRARY_PUBLISH_DIR "${CMAKE_SOURCE_DIR}/ManagedLibrary/publish") # Path from dotnet publish -o

# --- Define the shared library target ---
# The first argument is the target name. CMake will automatically name the file
# appropriately for the platform (e.g., my_cool_plugin.dll, libmy_cool_plugin.so, libmy_cool_plugin.dylib).
add_library(${PROJECT_NAME} SHARED PowershellExt.cpp PowershellExt.h)

# Assuming MainApp.exe is in CMAKE_BINARY_DIR for single-config MinGW:
set(PLUGIN_BASE_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set(PLUGIN_SUBDIR "ext")
set(PLUGIN_FULL_OUTPUT_DIR "${PLUGIN_BASE_OUTPUT_DIR}/${PLUGIN_SUBDIR}")

find_file(NETHOST_DLL NAMES nethost.dll REQUIRED)
get_filename_component(CMAKE_DOTNET_TARGET_FRAMEWORK "${NETHOST_DLL}" DIRECTORY)
message("CMAKE_DOTNET_TARGET_FRAMEWORK: ${CMAKE_DOTNET_TARGET_FRAMEWORK}")

# --- Include Directories ---
target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_DOTNET_TARGET_FRAMEWORK}"
        "${CMAKE_SOURCE_DIR}/include"
)

# Check if the library file exists
foreach (NET_LIB nethost.lib nethost.dll)
    # 1. Define a variable holding the path to the source DLL you want to copy.
    #    Using CMAKE_CURRENT_SOURCE_DIR makes the path relative to the current CMakeLists.txt
    set(MANAGED_DLL_SOURCE "${CMAKE_DOTNET_TARGET_FRAMEWORK}/${NET_LIB}")
    if (EXISTS ${MANAGED_DLL_SOURCE})
        # --- DEPLOY MANAGED DEPENDENCIES ---
        # 2. Define the destination directory.
        #    Here, we DO use a generator expression correctly to get the output directory
        #    of your main target, and then we append the subdirectories.
        set(MANAGED_DLL_DESTINATION "$<TARGET_FILE_DIR:${PROJECT_NAME}>")

        # 3. Create the custom command to perform the copy after the build.
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD

                # First, create the destination directory to make sure it exists
                COMMAND ${CMAKE_COMMAND} -E make_directory "${MANAGED_DLL_DESTINATION}"

                # Next, copy the DLL using the variables we defined
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MANAGED_DLL_SOURCE}"
                "${MANAGED_DLL_DESTINATION}"

                COMMENT "Copying managed dependency ${MANAGED_DLL_SOURCE}"
        )
    else ()
        message(FATAL_ERROR "${MANAGED_DLL_SOURCE} not found at ${CMAKE_DOTNET_TARGET_FRAMEWORK}.")
    endif ()
endforeach (NET_LIB)


# On Windows, this ensures that the functions are exported from the DLL.
# On other platforms (Linux, macOS), __attribute__((visibility("default"))) is typically used.
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE POWERSHELL_PLUGIN_EXPORTS)
endif ()

# Glob all files in the source assets directory
file(GLOB ASSET_FILES "${MANAGED_LIBRARY_PUBLISH_DIR}/*") # Gets files directly in the folder

if (EXISTS ${CMAKE_DOTNET_TARGET_FRAMEWORK})
    target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_DOTNET_TARGET_FRAMEWORK}")
    target_link_libraries(${PROJECT_NAME} PRIVATE nethost)
endif ()

foreach (ASSET_FILE ${ASSET_FILES})
    set(MANAGED_DLL_DESTINATION "$<TARGET_FILE_DIR:${PROJECT_NAME}>/managed")

    # 3. Create the custom command to perform the copy after the build.
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD

            # First, create the destination directory to make sure it exists
            COMMAND ${CMAKE_COMMAND} -E make_directory "${MANAGED_DLL_DESTINATION}"

            # Next, copy the DLL using the variables we defined
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${ASSET_FILE}"
            "${MANAGED_DLL_DESTINATION}"

            # COMMENT "Copying managed dependency ${ASSET_FILE}"
    )
endforeach ()

# On Windows, also place the PDB (Program Database for debugging) file alongside the DLL.
set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_FULL_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_RELEAS "${PLUGIN_FULL_OUTPUT_DIR}"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PLUGIN_FULL_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PLUGIN_FULL_OUTPUT_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PLUGIN_FULL_OUTPUT_DIR}"
        PDB_OUTPUT_DIRECTORY "${PLUGIN_FULL_OUTPUT_DIR}"
)
