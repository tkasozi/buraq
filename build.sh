#!/bin/bash

#
# MIT License
#
# Copyright (c)  "2025" Talik A. Kasozi
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

# Exit immediately if a command exits with a non-zero status.
set -e

# Compile C# ManagedLibrary code

if [[ ! -n "${CI}" ]]; then
  echo "CI variable is not set."

  pushd ManagedLibrary

  ./build.sh

  popd
fi

CMAKE_DOTNET_TARGET_FRAMEWORK="${CMAKE_DOTNET_TARGET_FRAMEWORK}"

# --- Configuration ---
# Get the directory where this script is located
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Assume the CMakeLists.txt is in the same directory as this script (project root)
# If your CMakeLists.txt is elsewhere, adjust this path accordingly.
SOURCE_DIR="${SCRIPT_DIR}"

# Default build directory name (will be created inside SOURCE_DIR)
BUILD_SUBDIR="build"
BUILD_DIR="${SOURCE_DIR}/${BUILD_SUBDIR}"

# Default build type
BUILD_TYPE="Release"

# Options
CLEAN_BUILD=false
NUM_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1) # Number of parallel jobs

# --- Helper Functions ---
print_usage() {
    echo "Usage: $0 [options]"
    echo ""
    echo "Options:"
    echo "  --debug         Set build type to Debug (default: Release)"
    echo "  --release       Set build type to Release"
    echo "  --clean         Perform a clean build (removes build directory before configuring)"
    echo "  --build-dir <name> Set the build subdirectory name (default: build)"
    echo "  -j <N>          Set the number of parallel jobs (default: auto-detect)"
    echo "  -h, --help      Show this help message"
}

# --- Argument Parsing ---
while [[ "$#" -gt 0 ]]; do
    case $1 in
    --debug)
        BUILD_TYPE="Debug"
        shift
        ;;
    --release)
        BUILD_TYPE="Release"
        shift
        ;; # Default, but explicit option
    --clean)
        CLEAN_BUILD=true
        shift
        ;;
    --build-dir)
        if [[ -n "$2" && "$2" != --* ]]; then
            BUILD_SUBDIR="$2"
            BUILD_DIR="${SOURCE_DIR}/${BUILD_SUBDIR}" # Update full build path
            shift 2
        else
            echo "Error: --build-dir requires a name." >&2
            exit 1
        fi
        ;;
    -j)
        if [[ -n "$2" && "$2" =~ ^[0-9]+$ ]]; then
            NUM_JOBS="$2"
            shift 2
        else
            echo "Error: -j requires a number." >&2
            exit 1
        fi
        ;;
    -h | --help)
        print_usage
        exit 0
        ;;
    *)
        echo "Unknown parameter passed: $1"
        print_usage
        exit 1
        ;;
    esac
done

# --- Main Script Logic ---

echo "----------------------------------------------------"
echo "Starting CMake Project Build"
echo "----------------------------------------------------"
echo "Source Directory:   ${SOURCE_DIR}"
echo "Build Subdirectory: ${BUILD_SUBDIR}"
echo "Full Build Path:    ${BUILD_DIR}"
echo "Build Type:         ${BUILD_TYPE}"
echo "Clean Build:        ${CLEAN_BUILD}"
echo "Parallel Jobs:      ${NUM_JOBS}"
echo "----------------------------------------------------"

# 1. Clean build directory if requested
if [ "$CLEAN_BUILD" = true ] && [ -d "$BUILD_DIR" ]; then
    echo ""
    echo "--- Cleaning previous build directory: ${BUILD_DIR} ---"
    rm -rf "${BUILD_DIR}"
    echo "Clean complete."
fi

# 2. Create build directory (if it doesn't exist)
echo ""
echo "--- Ensuring build directory exists: ${BUILD_DIR} ---"
mkdir -p "${BUILD_DIR}"
echo "Build directory ready."

# 3. Configure the project using CMake
# We run cmake from any directory, specifying source and build dirs explicitly.
echo ""
echo "--- Configuring CMake project (Build Type: ${BUILD_TYPE}) ---"
# The -S option specifies the source directory.
# The -B option specifies the build directory (created if it doesn't exist).
"$CMAKE_EXE" -S "${SOURCE_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
 -DCMAKE_DOTNET_TARGET_FRAMEWORK="${CMAKE_DOTNET_TARGET_FRAMEWORK}" -G "MinGW Makefiles"

echo "CMake configuration complete."

# 4. Build the project
echo ""
echo "--- Building project using CMake (Jobs: ${NUM_JOBS}) ---"
# The --build option tells CMake to drive the build process.
# --config is mostly for multi-configuration generators (like Visual Studio).
# For single-config (Makefiles, Ninja), CMAKE_BUILD_TYPE is used at configure time.
# The -- -j${NUM_JOBS} part passes the parallel job count to the underlying build tool (make/ninja).
"$CMAKE_EXE"  --build "${BUILD_DIR}" --config "${BUILD_TYPE}" -- -j"${NUM_JOBS}"
echo "Build complete."

echo ""
echo "----------------------------------------------------"
echo "Build process finished successfully!"
echo "Build artifacts are in: ${BUILD_DIR}"
echo "----------------------------------------------------"
