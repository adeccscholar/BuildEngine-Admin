param(
   [Parameter(Mandatory = $true)]
   [string]$Miktex,

   [Parameter(Mandatory = $true)]
   [string]$Repository,

   [Parameter(Mandatory = $true)]
   [string]$Packages
)

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

function Get-MiktexPackageInstalled
{
   param(
      [Parameter(Mandatory = $true)]
      [string]$PackageId
   )

   $output = & $Miktex packages info --template '{isInstalled}|{id}' $PackageId
   $exitCode = $LASTEXITCODE

   if ($exitCode -ne 0)
   {
      throw "MiKTeX package '$PackageId' is not available in the current package database (info exit=$exitCode)."
   }

   $line = @($output | Where-Object { $_ -match '^(true|false)\|' } | Select-Object -First 1)
   if ($line.Count -ne 1)
   {
      throw "MiKTeX package '$PackageId' returned no parseable package state."
   }

   $parts = $line[0].Split('|', 2)
   if ($parts.Count -ne 2 -or $parts[1] -ne $PackageId)
   {
      throw "MiKTeX package '$PackageId' returned unexpected package state '$($line[0])'."
   }

   return ($parts[0] -eq 'true')
}

if (-not (Test-Path -LiteralPath $Miktex -PathType Leaf))
{
   throw "MiKTeX executable not found: $Miktex"
}

$packageIds = @(
   $Packages.Split(';') |
      ForEach-Object { $_.Trim() } |
      Where-Object { $_.Length -gt 0 }
)

if ($packageIds.Count -eq 0)
{
   throw 'No MiKTeX package IDs were supplied.'
}

foreach ($packageId in $packageIds)
{
   $installed = Get-MiktexPackageInstalled -PackageId $packageId
   if ($installed)
   {
      Write-Output "[MIKTEX] package:$packageId : PRESENT"
      continue
   }

   Write-Output "[MIKTEX] package:$packageId : INSTALL"
   & $Miktex packages install --repository $Repository $packageId
   $exitCode = $LASTEXITCODE
   if ($exitCode -ne 0)
   {
      throw "MiKTeX package '$packageId' installation failed (exit=$exitCode)."
   }

   if (-not (Get-MiktexPackageInstalled -PackageId $packageId))
   {
      throw "MiKTeX package '$packageId' is still not installed after a successful install command."
   }

   Write-Output "[MIKTEX] package:$packageId : INSTALLED"
}

exit 0
