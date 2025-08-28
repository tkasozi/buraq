#!/usr/bin/env sh

# Exit immediately if a command exits with a non-zero status.
set -e

echo "Env PATH: ${PATH}"

echo "1. Cleaning"

# clean
dotnet clean Buraq.Bridge.csproj

echo "2. Restoring.."

# Restore dependencies
dotnet restore Buraq.Bridge.csproj

echo "3. Building Project.."

# Build project
dotnet build Buraq.Bridge.csproj --configuration Release --no-restore

echo "3. Publishing Project.."

# Publish project
dotnet publish Buraq.Bridge.csproj -c Release --self-contained true -r win-x64

