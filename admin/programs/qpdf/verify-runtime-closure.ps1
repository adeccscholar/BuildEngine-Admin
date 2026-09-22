param(
   [Parameter(Mandatory = $true)]
   [string]$TDump,

   [Parameter(Mandatory = $true)]
   [string]$BuildDir
)

$ErrorActionPreference = "Stop"

function Get-ImportedModules {
   param([Parameter(Mandatory = $true)][string]$File)

   if (-not (Test-Path -LiteralPath $File -PathType Leaf)) {
      throw "PE import probe input does not exist: $File"
   }

   $output = & $TDump -em. $File 2>&1
   if ($LASTEXITCODE -ne 0) {
      throw "tdump failed for '$File' with exit code $LASTEXITCODE"
   }

   $text = ($output | Out-String)
   $matches = [regex]::Matches($text, '(?i)\b[A-Za-z0-9_.+\-]+\.dll\b')
   $modules = @()
   foreach ($match in $matches) {
      $name = $match.Value.ToLowerInvariant()
      if ($modules -notcontains $name) {
         $modules += $name
      }
   }
   return $modules
}

$entryPoints = @(
   (Join-Path $BuildDir "qpdf\qpdf.exe"),
   (Join-Path $BuildDir "libtests\json_parse.exe")
)

$qpdfDll = Get-ChildItem -LiteralPath (Join-Path $BuildDir "libqpdf") -Filter "qpdf3*.dll" -File |
   Sort-Object Name |
   Select-Object -First 1

if ($null -eq $qpdfDll) {
   throw "No qpdf3*.dll found below '$BuildDir\libqpdf'"
}
$entryPoints += $qpdfDll.FullName

$forbiddenPatterns = @(
   "libcrypto*.dll",
   "libssl*.dll",
   "*brotli*.dll",
   "*zstd*.dll"
)

$failed = $false

foreach ($file in $entryPoints) {
   $modules = Get-ImportedModules -File $file
   Write-Host ("[PE] {0}" -f $file)
   foreach ($module in $modules) {
      Write-Host ("[PE]   -> {0}" -f $module)
      foreach ($pattern in $forbiddenPatterns) {
         if ($module -like $pattern) {
            Write-Host ("[PE][FORBIDDEN] {0} -> {1} matches {2}" -f $file, $module, $pattern)
            $failed = $true
         }
      }
   }
}

if ($failed) {
   throw "QPDF runtime closure contains forbidden OpenSSL/Brotli/Zstd imports"
}

Write-Host "[PE] QPDF runtime closure PASS: no OpenSSL/Brotli/Zstd imports in qpdf.exe, json_parse.exe or qpdf DLL"
