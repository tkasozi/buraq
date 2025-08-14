FROM tkasozi/win-vcpkg-mingw64-qt-cpp-minimal:1.0.0 AS builder

# Stage 1: The Build Environment (using a full Windows Server image)
USER ContainerUser

COPY . C:/app

# Build the dontnet library in the ManagedLibrary directory
WORKDIR C:/app/ManagedLibrary

# Run the build script for the ManagedLibrary project
# RUN cmd /c "C:\msys64\usr\bin\bash.exe build.sh"

# Build the C++ project in the main application directory
WORKDIR C:/app

ENV VCPKG_TARGET_TRIPLET="x64-mingw-dynamic"

# Install dependencies from vcpkg.json using the MinGW triplet
RUN cmd /c "C:\msys64\usr\bin\bash.exe build.sh"

# RUN powershell -Command "Get-ChildItem -Path 'C:\' -Recurse -Filter 'ISCC.exe'"

RUN powershell -Command "Get-ChildItem -Path 'C:/app'"
