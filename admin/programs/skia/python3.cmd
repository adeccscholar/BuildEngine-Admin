@echo off
rem Copyright (c) 2026 adecc Systemhaus GmbH
rem SPDX-License-Identifier: MIT
rem Project: adecc Scholar
if not defined CB_PYTHON (
   echo ERROR: CB_PYTHON is not defined for Skia GN helper scripts.
   exit /b 127
)
"%CB_PYTHON%" %*
exit /b %ERRORLEVEL%
