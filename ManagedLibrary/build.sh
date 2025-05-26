#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

#rm -r publish bin obj ../Release/bin/plugins/managed

# clean
dotnet clean

# Restore dependencies
dotnet restore ./ManagedLibrary.sln

# Build project
dotnet build ./ManagedLibrary.sln --configuration Release --no-restore

# Publish project
dotnet publish -c Release -r win-x64 --no-self-contained -o "./publish"
