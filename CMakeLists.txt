cmake_minimum_required(VERSION 3.27)

project(ITools)

set(CMAKE_ROOT_PROJECT_POSTFIX ITools)
set(CSHARP_PROJECT_OUTPUT_DIR "C:\\Users\\talik\\workspace\\ITools\\ManagedLibrary\\bin\\Release\\net9.0") # Adjust as needed

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set global output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # For .lib/.a if any
# For DLLs on Windows and .so on Linux, they often go to CMAKE_RUNTIME_OUTPUT_DIRECTORY
# or CMAKE_LIBRARY_OUTPUT_DIRECTORY depending on target type and policies.
# For simplicity, let's assume executables go to <build_dir>/<config>/
# and we'll define specific output for the plugin.

# If you build Release, Debug, etc., these will be subfolders like Release/Debug
# You can also customize this per configuration:
# foreach(config ${CMAKE_CONFIGURATION_TYPES})
#     message("${CMAKE_BINARY_DIR}/${config}")
#   set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} "${CMAKE_BINARY_DIR}/${config}")
# endforeach()
# For single-configuration generators (like MinGW Makefiles), CMAKE_BINARY_DIR is often the build root.

add_subdirectory(app)       # Assuming MainApp.exe project is here
add_subdirectory(exts)  # Assuming libPowershellExt.dll project is here
