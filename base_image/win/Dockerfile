# Stage 1: The Build Environment (using a full Windows Server image)
FROM mcr.microsoft.com/windows/servercore:ltsc2022 AS builder

USER ContainerAdministrator

# Install MSYS2 and the MinGW toolchain in a single, reliable step
RUN powershell -Command Invoke-WebRequest -Uri 'https://repo.msys2.org/distrib/x86_64/msys2-base-x86_64-20250221.sfx.exe' -OutFile 'C:\msys2-base-x86_64-20250221.sfx.exe'; \
    Start-Process 'C:\msys2-base-x86_64-20250221.sfx.exe' -Wait;

#RUN powershell -Command Get-ChildItem -Path "." -Recurse -Filter "pacman.d"

# Install the MinGW toolchain and other necessary packages
# This avoids PowerShell pathing issues and ensures the environment is set up correctly.
RUN C:\msys64\msys2_shell.cmd -c '\
    rm -r C:\msys64\etc\pacman.d\gnupg && \
    C:\msys64\usr\bin\pacman-key --init && \
    C:\msys64\usr\bin\pacman-key --populate msys && \
    curl -O https://repo.msys2.org/msys/x86_64/msys2-keyring-1~20250619-1-any.pkg.tar.zst && \
    curl -O https://repo.msys2.org/msys/x86_64/msys2-keyring-1~20250619-1-any.pkg.tar.zst.sig && \
    C:/msys64/usr/bin/pacman -U --config <(echo) msys2-keyring-r21.b39fb11-1-any.pkg.tar.xz && \
    C:/msys64/usr/bin/pacman --noconfirm -S --needed git base-devel mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake'

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
