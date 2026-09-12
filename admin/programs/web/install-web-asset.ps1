param(
   [Parameter(Mandatory = $true)][string]$Archive,
   [Parameter(Mandatory = $true)][string]$ManagedRoot,
   [Parameter(Mandatory = $true)][string]$RelativePath
)

$ErrorActionPreference = 'Stop'

if (-not (Test-Path -LiteralPath $Archive -PathType Leaf)) {
   throw "Web asset source does not exist: $Archive"
}

if ([System.IO.Path]::IsPathRooted($RelativePath) -or $RelativePath.Contains('..')) {
   throw "Web asset relative path is invalid: $RelativePath"
}

$target = Join-Path $ManagedRoot $RelativePath
$targetDirectory = Split-Path -Parent $target
New-Item -ItemType Directory -Force -Path $targetDirectory | Out-Null
Copy-Item -LiteralPath $Archive -Destination $target -Force

if (-not (Test-Path -LiteralPath $target -PathType Leaf)) {
   throw "Web asset target was not created: $target"
}
