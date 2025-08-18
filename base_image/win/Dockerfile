# Stage 1: The Build Environment (using a full Windows Server image)
FROM mcr.microsoft.com/windows/servercore:ltsc2022 AS builder

USER ContainerAdministrator

# Install MSYS2 and the MinGW toolchain in a single, reliable step
RUN powershell -Command Invoke-WebRequest -Uri 'https://repo.msys2.org/distrib/x86_64/msys2-base-x86_64-20250221.sfx.exe' -OutFile 'C:\msys2-base-x86_64-20250221.sfx.exe'; \
    Start-Process 'C:\msys2-base-x86_64-20250221.sfx.exe' -Wait;

#RUN powershell -Command Get-ChildItem -Path "." -Recurse -Filter "pacman.conf"

# This single RUN command handles the entire initialization and update process
RUN C:\msys64\usr\bin\bash -lc "\
    # 1. Create the initial keyring structure
    pacman-key --init && \
    \
    # 2. Refresh package databases and install the keyring package itself.
    #    This is the key step that fixes the "keyring file does not exist" error.
    pacman -Sy --noconfirm msys2-keyring && \
    \
    # 3. Now that the keyring is fixed, perform the full two-stage system update
    pacman -Syu --noconfirm && \
    pacman -Su --noconfirm" || true

# Install the MinGW toolchain and other necessary packages
RUN C:\msys64\usr\bin\bash -lc "\
    pacman -S --noconfirm --needed \
    base-devel \
    mingw-w64-x86_64-toolchain \
    git \
    make \
    mingw-w64-x86_64-cmake"

# Stage 2: The Runtime Environment
# This stage uses dotnet/sdk image and just copies the final executable.
FROM mcr.microsoft.com/dotnet/sdk:9.0.302-windowsservercore-ltsc2022 AS vcpkg-builder

USER ContainerAdministrator

COPY --from=builder C:/msys64 C:/msys64

# Now install vcpkg itself if not already in your base image
RUN git clone https://github.com/microsoft/vcpkg.git

RUN C:\vcpkg\bootstrap-vcpkg.bat -disableMetrics

# Install Inno setup for packaging
RUN powershell -Command \
    Invoke-WebRequest -Uri "https://jrsoftware.org/download.php/is.exe?site=1" -OutFile "inno_setup.exe"; \
    \
    Start-Process -Wait -FilePath "C:\inno_setup.exe" -ArgumentList "/SILENT"

# Set the PATH to include the MinGW compiler binaries.
# This ensures that vcpkg's build system can find g++.
RUN setx -m PATH "C:/msys64/mingw64/bin;C:/msys64/usr/bin;C:/vcpkg;C:/Program Files (x86)/Inno Setup 6;%PATH%"
RUN powershell -Command "echo Current PATH: $env:PATH"

# Set environment variables for vcpkg
ENV VCPKG_ROOT=C:/vcpkg

WORKDIR C:/app
COPY vcpkg.json C:/app/vcpkg.json

# Install dependencies from vcpkg.json using the MinGW triplet
RUN cmd /c "C:/vcpkg/vcpkg.exe install --triplet=x64-mingw-dynamic --host-triplet=x64-mingw-dynamic --allow-unsupported"

# Set a label to make this a unique, publishable image.
LABEL maintainer="tkasozi/win-vcpkg-mingw64-qt-cpp-minimal"
