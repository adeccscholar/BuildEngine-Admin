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

function Install-MiktexPackages
{
   param(
      [Parameter(Mandatory = $true)]
      [string[]]$PackageIds
   )

   if ($PackageIds.Count -eq 0)
   {
      return
   }

   Write-Output "[MIKTEX] installing $($PackageIds.Count) package(s) in one transaction"
   $installArguments = @('packages', 'install', '--repository', $Repository) + $PackageIds
   & $Miktex $installArguments
   $exitCode = $LASTEXITCODE
   if ($exitCode -ne 0)
   {
      throw "MiKTeX package installation failed (exit=$exitCode)."
   }
}

function Test-MiktexPackages
{
   param(
      [Parameter(Mandatory = $true)]
      [string[]]$PackageIds
   )

   if ($PackageIds.Count -eq 0)
   {
      return $true
   }

   & $Miktex packages verify $PackageIds | Out-Null
   return ($LASTEXITCODE -eq 0)
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
   Install-MiktexPackages -PackageIds $missing.ToArray()

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

# A package can be marked as installed while its payload is incomplete. This
# happened after a partial MiKTeX update: package state said PRESENT although a
# required .sty file was absent. Verify the complete declared package surface.
# Only if the bulk verification fails do we pay the cost of checking packages
# individually and repairing the broken subset.
if (-not (Test-MiktexPackages -PackageIds $packageIds))
{
   Write-Output '[MIKTEX] package integrity check failed; isolating damaged package(s)'
   $damaged = New-Object System.Collections.Generic.List[string]

   foreach ($packageId in $packageIds)
   {
      if (-not (Test-MiktexPackages -PackageIds @($packageId)))
      {
         Write-Output "[MIKTEX] package:$packageId : DAMAGED"
         $damaged.Add($packageId)
      }
   }

   if ($damaged.Count -eq 0)
   {
      throw 'MiKTeX bulk package verification failed, but no individual damaged package could be identified.'
   }

   foreach ($packageId in $damaged)
   {
      Write-Output "[MIKTEX] package:$packageId : REINSTALL"
      & $Miktex packages remove $packageId
      $exitCode = $LASTEXITCODE
      if ($exitCode -ne 0)
      {
         throw "MiKTeX package '$packageId' could not be removed for repair (exit=$exitCode)."
      }

      Install-MiktexPackages -PackageIds @($packageId)
   }

   & $Miktex fndb refresh
   $exitCode = $LASTEXITCODE
   if ($exitCode -ne 0)
   {
      throw "MiKTeX file name database refresh after package repair failed (exit=$exitCode)."
   }

   if (-not (Test-MiktexPackages -PackageIds $packageIds))
   {
      throw 'MiKTeX package integrity is still invalid after package repair.'
   }

   foreach ($packageId in $damaged)
   {
      Write-Output "[MIKTEX] package:$packageId : REPAIRED"
   }
}
else
{
   Write-Output '[MIKTEX] package integrity verified'
}

exit 0
