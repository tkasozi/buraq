.SYNOPSIS
    A simple PowerShell script to find files by extension in a specified directory.
 
.DESCRIPTION
    This script will search a given directory (and its subdirectories) for files
    matching a specific extension and then output the list of found files and
    their total count.
 
.NOTES
    Author: Coding Partner AI
    Date:   May 18, 2025
#>
 
# --- Configuration ---
# Define the path to the directory you want to search.
# You can change this to any folder path on your system.
# Example for Windows: $targetDirectory = "C:\Users\YourUserName\Documents"
# Example for macOS/Linux (if using PowerShell Core): $targetDirectory = "/home/yourusername/documents"
$targetDirectory = $env:USERPROFILE + "\Documents" # Defaulting to the current user's Documents folder folder
 
# Define the file extension to search for (e.g., ".txt", ".log", ".jpg").
# Make sure to include the dot.
$fileExtension = ".txt"
 asas
# --- Script Logic ---
 
# Check if the target directory exists
if (Test-Path -Path $targetDirectory -PathType Container) {
    Write-Host "Searching for '$fileExtension' files in directory: $targetDirectory" -ForegroundColor Green
    Write-Host "--------------------------------------------------"
 
    # Get files with the specified extension.
    # Get-ChildItem (alias: gci, ls, dir) is used to get items in a location.
    # -Path: Specifies the directory to search.
    # -Filter: Specifies a filter in the provider's format (e.g., "*.txt"). More efficient than Where-Object for simple extension filtering.
    # -Recurse: Searches in subdirectories as well.
    # -File: Ensures only files are returned (not directories).
$foundFiles = Get-ChildItem -Path $targetDirectory -Filter "*$fileExtension" -Recurse -File -ErrorAction SilentlyContinue
 
    # Check if any files were found
    if ($foundFiles) {
        Write-Host "Found the following '$fileExtension' files:"
        # Loop through each found file and print its full name
        foreach ($file in $foundFiles) {
            Write-Host "- $($file.FullName)" #  $($file.FullName) ensures the FullName property is expanded if the string is complex.
        }
 
        # Count the number of files found
$fileCount = $foundFiles.Count
        Write-Host "--------------------------------------------------"
        Write-Host "Total '$fileExtension' files found: $fileCount" -ForegroundColor Yellow
    } else {
        Write-Host "No '$fileExtension' files found in '$targetDirectory' or its subdirectories." -ForegroundColor Red
    }
} else {
    Write-Host "Error: The directory '$targetDirectory' does not exist." -ForegroundColor Red
    Write-Host "Please update the '\$targetDirectory' variable in the script."
}
 
# --- End of Script ---
Write-Host "Script finished." 