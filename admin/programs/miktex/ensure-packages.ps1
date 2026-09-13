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

function Test-DoxygenLatexStack
{
   $binDirectory = Split-Path -Parent $Miktex
   $texify = Join-Path $binDirectory 'texify.exe'
   if (-not (Test-Path -LiteralPath $texify -PathType Leaf))
   {
      throw "MiKTeX texify executable not found: $texify"
   }

   $preflightRoot = Join-Path ([System.IO.Path]::GetTempPath()) ('buildengine-miktex-preflight-' + [Guid]::NewGuid().ToString('N'))
   New-Item -ItemType Directory -Path $preflightRoot | Out-Null

   try
   {
      $input = Join-Path $preflightRoot 'preflight.tex'
      @'
\documentclass{book}
\usepackage{infwarerr}
\usepackage{float}
\usepackage{varwidth}
\usepackage{xcolor}
\usepackage{colortbl}
\usepackage{xltabular}
\usepackage{tabularray}
\usepackage{fancyvrb}
\usepackage{multirow}
\usepackage{hanging}
\usepackage{adjustbox}
\usepackage{stackengine}
\usepackage{enumitem}
\usepackage{alphalph}
\usepackage[normalem]{ulem}
\usepackage{iftex}
\usepackage{wasysym}
\usepackage{geometry}
\usepackage{changepage}
\usepackage{fancyhdr}
\usepackage{natbib}
\usepackage{tocloft}
\usepackage{hyperref}
\usepackage{caption}
\usepackage{etoc}
\begin{document}
BuildEngine Doxygen 1.18.0 MiKTeX preflight.
\end{document}
'@ | Set-Content -LiteralPath $input -Encoding ASCII

      Write-Output '[MIKTEX] Doxygen LaTeX stack preflight : START'
      Push-Location $preflightRoot
      try
      {
         & $texify --pdf --batch --max-iterations=2 --tex-option=--disable-installer 'preflight.tex'
         $exitCode = $LASTEXITCODE
      }
      finally
      {
         Pop-Location
      }

      if ($exitCode -ne 0)
      {
         $log = Join-Path $preflightRoot 'preflight.log'
         if (Test-Path -LiteralPath $log -PathType Leaf)
         {
            Write-Output '[MIKTEX] Doxygen LaTeX stack preflight log follows:'
            Get-Content -LiteralPath $log | Select-Object -Last 80 | ForEach-Object { Write-Output $_ }
         }
         throw "MiKTeX Doxygen LaTeX stack preflight failed (exit=$exitCode)."
      }

      Write-Output '[MIKTEX] Doxygen LaTeX stack preflight : PASS'
   }
   finally
   {
      Remove-Item -LiteralPath $preflightRoot -Recurse -Force -ErrorAction SilentlyContinue
   }
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

# A package can be marked as installed while its payload is incomplete. Verify
# the complete declared package surface. Only if the bulk verification fails do
# we pay the cost of checking packages individually and repairing the broken
# subset.
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

Test-DoxygenLatexStack
exit 0
