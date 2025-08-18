# Use your pre-built image as the builder environment
FROM ghcr.io/tkasozi/win-vcpkg-mingw64-qt-cpp-minimal:latest AS builder

# Stage 1: The Build Environment (using a full Windows Server image)
USER ContainerUser

# Copy all your source code into the image
COPY . C:/app

# Build the dontnet library in the ManagedLibrary directory
WORKDIR C:/app/ManagedLibrary

# Run the build script for the ManagedLibrary project
# RUN cmd /c "C:\msys64\usr\bin\bash.exe build.sh"

# Set the working directory
WORKDIR C:/app

# Run the build script to compile the app and create the setup.exe
RUN C:/msys64/usr/bin/bash.exe ./build.sh
