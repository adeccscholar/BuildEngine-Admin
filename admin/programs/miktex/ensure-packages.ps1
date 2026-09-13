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

   Write-Output "[MIKTEX] installing $($PackageIds.Count) missing package(s) in one transaction"
   $installArguments = @('packages', 'install', '--repository', $Repository) + $PackageIds
   & $Miktex $installArguments
   $exitCode = $LASTEXITCODE
   if ($exitCode -ne 0)
   {
      throw "MiKTeX package installation failed (exit=$exitCode)."
   }
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
            Get-Content -LiteralPath $log | Select-Object -Last 100 | ForEach-Object { Write-Output $_ }
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

   # Refresh the file name database before validating the actual LaTeX stack.
   & $Miktex fndb refresh
   $exitCode = $LASTEXITCODE
   if ($exitCode -ne 0)
   {
      throw "MiKTeX file name database refresh after package installation failed (exit=$exitCode)."
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

# Do not attempt package-by-package repair here. The package database can be
# newer than the bootstrap installer and a broad `packages verify` result is not
# a safe signal for destructive remove/reinstall operations. The declarative XML
# package set is ensured above; the real contract is then validated by compiling
# the Doxygen-relevant LaTeX stack with automatic package installation disabled.
Test-DoxygenLatexStack
exit 0
