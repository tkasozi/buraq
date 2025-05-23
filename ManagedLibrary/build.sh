#!/usr/bin/env bash

rm -r ./ManagedLibraryPublish ../Release/bin/plugins/ext/ManagedLibraryPublish

# clean
dotnet clean

# In the ManagedLibrary project directory
dotnet publish -c Release -r win-x64 --no-self-contained -o ".\ManagedLibraryPublish"
