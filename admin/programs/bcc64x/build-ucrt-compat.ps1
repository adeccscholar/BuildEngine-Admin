# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

param(
   [Parameter(Mandatory = $true)]
   [string]$Bds,

   [Parameter(Mandatory = $true)]
   [string]$OutputPath
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$theBds = [System.IO.Path]::GetFullPath($Bds)
$theOutput = [System.IO.Path]::GetFullPath($OutputPath)
$theLlvmAr = Join-Path $theBds 'bin64\llvm-ar.exe'
$theUcrtArchive = Join-Path $theBds 'x86_64-w64-mingw32\lib\libucrt.a'
$theMembers = @(
   'lib64_libucrt_extra_a-fabsf.o',
   'lib64_libucrt_extra_a-nextafterl.o',
   'lib64_libucrt_extra_a-nexttoward.o',
   'lib64_libucrt_extra_a-nexttowardf.o'
)

if(-not (Test-Path -LiteralPath $theLlvmAr -PathType Leaf)) {
   throw "BCC64X llvm-ar not found: $theLlvmAr"
}
if(-not (Test-Path -LiteralPath $theUcrtArchive -PathType Leaf)) {
   throw "BCC64X UCRT archive not found: $theUcrtArchive"
}

$theArchiveMembers = @(& $theLlvmAr t $theUcrtArchive)
if($LASTEXITCODE -ne 0) {
   throw "llvm-ar failed while reading $theUcrtArchive (exit $LASTEXITCODE)"
}
foreach($theMember in $theMembers) {
   if($theArchiveMembers -notcontains $theMember) {
      throw "Required BCC64X UCRT compatibility member is missing: $theMember"
   }
}

$theOutputDirectory = Split-Path -Parent $theOutput
if(-not (Test-Path -LiteralPath $theOutputDirectory -PathType Container)) {
   New-Item -ItemType Directory -Path $theOutputDirectory -Force | Out-Null
}

$theScriptFile = $MyInvocation.MyCommand.Path
$theNeedsRebuild = -not (Test-Path -LiteralPath $theOutput -PathType Leaf)
if(-not $theNeedsRebuild) {
   $theOutputTime = (Get-Item -LiteralPath $theOutput).LastWriteTimeUtc
   $theNeedsRebuild = (Get-Item -LiteralPath $theUcrtArchive).LastWriteTimeUtc -gt $theOutputTime
   if(-not $theNeedsRebuild -and $theScriptFile) {
      $theNeedsRebuild = (Get-Item -LiteralPath $theScriptFile).LastWriteTimeUtc -gt $theOutputTime
   }
}

if(-not $theNeedsRebuild) {
   Write-Output $theOutput
   exit 0
}

$theWorkDirectory = Join-Path $theOutputDirectory ('.ucrt-compat-work-' + $PID)
$theTemporaryArchive = Join-Path $theWorkDirectory 'libbcc64x-ucrt-compat.a'

if(Test-Path -LiteralPath $theWorkDirectory) {
   Remove-Item -LiteralPath $theWorkDirectory -Recurse -Force
}
New-Item -ItemType Directory -Path $theWorkDirectory -Force | Out-Null

try {
   Push-Location $theWorkDirectory
   try {
      & $theLlvmAr x $theUcrtArchive @theMembers
      if($LASTEXITCODE -ne 0) {
         throw "llvm-ar failed while extracting BCC64X UCRT compatibility objects (exit $LASTEXITCODE)"
      }

      foreach($theMember in $theMembers) {
         if(-not (Test-Path -LiteralPath (Join-Path $theWorkDirectory $theMember) -PathType Leaf)) {
            throw "Extracted BCC64X UCRT compatibility object is missing: $theMember"
         }
      }

      & $theLlvmAr rcs $theTemporaryArchive @theMembers
      if($LASTEXITCODE -ne 0) {
         throw "llvm-ar failed while creating BCC64X UCRT compatibility archive (exit $LASTEXITCODE)"
      }
   }
   finally {
      Pop-Location
   }

   $theCompatMembers = @(& $theLlvmAr t $theTemporaryArchive)
   if($LASTEXITCODE -ne 0) {
      throw "llvm-ar failed while verifying BCC64X UCRT compatibility archive (exit $LASTEXITCODE)"
   }
   foreach($theMember in $theMembers) {
      if($theCompatMembers -notcontains $theMember) {
         throw "Generated BCC64X UCRT compatibility archive is incomplete: $theMember"
      }
   }

   Move-Item -LiteralPath $theTemporaryArchive -Destination $theOutput -Force
}
finally {
   if(Test-Path -LiteralPath $theWorkDirectory) {
      Remove-Item -LiteralPath $theWorkDirectory -Recurse -Force
   }
}

Write-Output $theOutput
