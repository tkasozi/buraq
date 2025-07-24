#!/bin/bash

configs=("powershell_plugin" "updater" "buraq" "deploy_main")

CMAKE_EXE="C:\\Users\\talik\\AppData\\Local\\JetBrains\\CLion 2023.3.2\\bin\\cmake\\win\\x64\\bin\\cmake.exe"

# Ensure this BUILD_DIR matches the one you just configured in Step 1
BUILD_DIR="C:\\Users\\talik\\projects\\buraq\\cmake-build-debug" # Or -release-visual-studio

"C:\Users\talik\AppData\Local\JetBrains\CLion 2023.3.2\bin\cmake\win\x64\bin\cmake.exe" \
 -DCMAKE_BUILD_TYPE=Debug \
 -DCMAKE_MAKE_PROGRAM="C:/Users/talik/AppData/Local/JetBrains/CLion 2023.3.2/bin/ninja/win/x64/ninja.exe" \
 -DCMAKE_TOOLCHAIN_FILE="C:\Users\talik\.vcpkg-clion\vcpkg\scripts\buildsystems\vcpkg.cmake" \
 -DCMAKE_DOTNET_TARGET_FRAMEWORK="C:\Program Files\dotnet\packs\Microsoft.NETCore.App.Host.win-x64\9.0.6\runtimes\win-x64\native" \
-G Ninja -S "C:\Users\talik\projects\buraq" -B "${BUILD_DIR}"

for config in "${configs[@]}"; do
    echo "====================================================="
    echo "Building target: ${config} ..."
    echo "====================================================="

    # For Visual Studio generators, you MUST specify --config here.
    # Use Debug if your BUILD_DIR is '...debug-visual-studio'
    # Use Release if your BUILD_DIR is '...release-visual-studio'
    "${CMAKE_EXE}" --build "${BUILD_DIR}" --target "${config}" -j 10 # Adjust --config

    if [ $? -eq 0 ]; then
        echo "Successfully built ${config}."
    else
        echo "ERROR: Building ${config} FAILED!"
        exit 1
    fi
    echo ""
done

echo "All specified targets have been processed."