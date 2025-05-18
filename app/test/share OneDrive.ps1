Install-Module -Name Microsoft.Online.SharePoint.PowerShell 
 
echo "test"
 
$TenantURL = "https://yasray-admin.sharepoint.com"
  
Connect-SPOService -Url $TenantURL
 
Get-SPOsite -IncludePersonalSite $True -Limit all -Filter "Url -like'-my.sharepoint.com/personal/Ash'" | SELECT Urlmy.sharepoint.com/personal/Ash'" | SELECT Url
 
 
$siteAddress = "https://yasray-my.sharepoint.com/personal/ashraf_cloudskyspro_com"
 
$AddSecondOwner = "yasray@cloudskyspro.com"
 
Set-SPOUser -Site $siteAddress -LoginName $AddSecondOwner -IsSiteCollectionAdmin $False
 
$name = "tkasozi"
 