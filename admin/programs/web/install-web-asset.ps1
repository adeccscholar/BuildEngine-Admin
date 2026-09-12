param(
   [Parameter(Mandatory = $true)][string]$Archive,
   [Parameter(Mandatory = $true)][string]$ManagedRoot,
   [Parameter(Mandatory = $true)][string]$RelativePath,
   [string]$AdditionalSource = '',
   [string]$AdditionalRelativePath = ''
)

$ErrorActionPreference = 'Stop'

function Assert-SafeRelativePath([string]$Path, [string]$Description) {
   if ([string]::IsNullOrWhiteSpace($Path) -or [System.IO.Path]::IsPathRooted($Path) -or $Path.Contains('..')) {
      throw "$Description is invalid: $Path"
   }
}

function Copy-Asset([string]$Source, [string]$RelativeTarget) {
   Assert-SafeRelativePath $RelativeTarget 'Web asset relative path'
   if (-not (Test-Path -LiteralPath $Source -PathType Leaf)) {
      throw "Web asset source does not exist: $Source"
   }

   $target = Join-Path $ManagedRoot $RelativeTarget
   $targetDirectory = Split-Path -Parent $target
   New-Item -ItemType Directory -Force -Path $targetDirectory | Out-Null
   Copy-Item -LiteralPath $Source -Destination $target -Force

   if (-not (Test-Path -LiteralPath $target -PathType Leaf)) {
      throw "Web asset target was not created: $target"
   }
}

Copy-Asset $Archive $RelativePath

if (-not [string]::IsNullOrWhiteSpace($AdditionalSource) -or
    -not [string]::IsNullOrWhiteSpace($AdditionalRelativePath)) {
   if ([string]::IsNullOrWhiteSpace($AdditionalSource) -or
       [string]::IsNullOrWhiteSpace($AdditionalRelativePath)) {
      throw 'AdditionalSource and AdditionalRelativePath must be supplied together'
   }
   Copy-Asset $AdditionalSource $AdditionalRelativePath
}
