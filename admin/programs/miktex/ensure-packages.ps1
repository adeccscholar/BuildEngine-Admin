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

function Get-MiktexPackageState
{
   $output = & $Miktex packages list --template '{isInstalled}|{id}'
   $exitCode = $LASTEXITCODE
   if ($exitCode -ne 0)
   {
      throw "MiKTeX package list failed (exit=$exitCode)."
   }

   $state = @{}
   foreach ($line in @($output))
   {
      if ($line -notmatch '^(true|false)\|(.+)$')
      {
         continue
      }

      $state[$Matches[2]] = ($Matches[1] -eq 'true')
   }

   return $state
}

$state = Get-MiktexPackageState
$missing = New-Object System.Collections.Generic.List[string]

foreach ($packageId in $packageIds)
{
   if (-not $state.ContainsKey($packageId))
   {
      throw "MiKTeX package '$packageId' is not available in the current package database."
   }

   if ($state[$packageId])
   {
      Write-Output "[MIKTEX] package:$packageId : PRESENT"
   }
   else
   {
      Write-Output "[MIKTEX] package:$packageId : MISSING"
      $missing.Add($packageId)
   }
}

if ($missing.Count -gt 0)
{
   Write-Output "[MIKTEX] installing $($missing.Count) missing package(s) in one transaction"
   $installArguments = @('packages', 'install', '--repository', $Repository) + $missing.ToArray()
   & $Miktex $installArguments
   $exitCode = $LASTEXITCODE
   if ($exitCode -ne 0)
   {
      throw "MiKTeX package installation failed (exit=$exitCode)."
   }

   $state = Get-MiktexPackageState
   foreach ($packageId in $missing)
   {
      if (-not $state.ContainsKey($packageId) -or -not $state[$packageId])
      {
         throw "MiKTeX package '$packageId' is still not installed after a successful install command."
      }
      Write-Output "[MIKTEX] package:$packageId : INSTALLED"
   }
}
else
{
   Write-Output '[MIKTEX] all requested packages are already installed'
}

exit 0
