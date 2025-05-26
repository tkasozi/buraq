#!/usr/bin/env bash

rm -r publish bin obj ../Release/bin/plugins/managed

# clean
dotnet clean

# Restore dependencies
dotnet restore ./ManagedLibrary.sln

# Build project
dotnet build ./ManagedLibrary.sln --configuration Release --no-restore

# Publish project
dotnet publish -c Release -r win-x64 --no-self-contained -o "./publish"
