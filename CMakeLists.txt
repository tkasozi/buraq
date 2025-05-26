cmake_minimum_required(VERSION 3.27)

project(MainProject)

set(CMAKE_ROOT_PROJECT_POSTFIX ITools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set global output directories for executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") # For .lib/.a if any

add_subdirectory(app)       # Assuming MainApp.exe project is here
add_subdirectory(exts)  # Assuming libPowershellExt.dll project is here
