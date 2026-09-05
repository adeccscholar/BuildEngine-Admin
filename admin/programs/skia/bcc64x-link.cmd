@echo off
rem Copyright (c) 2026 adecc Systemhaus GmbH
rem SPDX-License-Identifier: MIT
rem Project: adecc Scholar

setlocal EnableExtensions DisableDelayedExpansion
if not defined CB_BCC64X (
   echo ERROR: CB_BCC64X is not defined for the Skia BCC64X link driver.
   exit /b 127
)
if not defined CB_BDS (
   echo ERROR: CB_BDS is not defined for the Skia BCC64X link driver.
   exit /b 127
)
"%CB_BCC64X%" -L"%CB_BDS%\lib\win64x\release" -L"%CB_BDS%\lib\win64x\release\psdk" %*
exit /b %ERRORLEVEL%
