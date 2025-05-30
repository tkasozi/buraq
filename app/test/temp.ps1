# This ie example #1 (original)
#You can add your script code here.
# For example:.
 
$name="tkasozi & Nihlah & sumi"
Write-Output "This is an example script by $name."
 
Get-ChildItem . | Format-Table -Property Name, Length, LastWriteTime
 
Get-ChildItem | Format-Table -Property Name, Length, LastWriteTime -AutoSize
 
# new update.
 
$idk = "what this is."
 
Get-Process | Sort-Object Memory -Descending | Select-Object -First 5
 
cd C:\Users\talik && ls  