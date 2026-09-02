param(
   [Parameter(Mandatory = $true)][string]$Archive,
   [Parameter(Mandatory = $true)][string]$Target,
   [Parameter(Mandatory = $true)][string]$PthFile,
   [Parameter(Mandatory = $true)][string]$SiteCustomize
)

$ErrorActionPreference = 'Stop'

Add-Type -AssemblyName System.IO.Compression.FileSystem

if(Test-Path -LiteralPath $Target) {
   Remove-Item -LiteralPath $Target -Recurse -Force
}

New-Item -ItemType Directory -Path $Target -Force | Out-Null
[System.IO.Compression.ZipFile]::ExtractToDirectory($Archive, $Target)

Copy-Item -LiteralPath $PthFile -Destination (Join-Path $Target 'python314._pth') -Force
Copy-Item -LiteralPath $SiteCustomize -Destination (Join-Path $Target 'sitecustomize.py') -Force

$Required = @(
   (Join-Path $Target 'python.exe'),
   (Join-Path $Target 'python314.zip'),
   (Join-Path $Target 'python314._pth'),
   (Join-Path $Target 'sitecustomize.py')
)

foreach($Path in $Required) {
   if(-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
      throw "Managed build Python is incomplete: $Path"
   }
}
