# Use your pre-built image as the builder environment
FROM ghcr.io/tkasozi/win-vcpkg-mingw64-qt-cpp-minimal:latest AS builder

# Copy all your source code into the image
COPY . C:/app

# Build the dontnet library in the ManagedLibrary directory
WORKDIR C:/app/ManagedLibrary

RUN C:/msys64/usr/bin/bash.exe -lc "export PATH='/c/msys64/mingw64/bin:/c/msys64/usr/bin:/c/Program Files (x86)/Inno Setup 6:'$PATH"

# Run the build script for the ManagedLibrary project
RUN C:/msys64/usr/bin/bash.exe ./build.sh

# Set the working directory
WORKDIR C:/app

ENV VCPKG_TARGET_TRIPLET="x64-mingw-dynamic"

# Explicitly switch to the Administrator user to ensure correct permissions
USER ContainerAdministrator

# Run the build script to compile the app and create the setup.exe
RUN C:/msys64/usr/bin/bash.exe ./build.sh
