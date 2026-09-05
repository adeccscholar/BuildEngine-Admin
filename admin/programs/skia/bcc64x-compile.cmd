@echo off
rem Copyright (c) 2026 adecc Systemhaus GmbH
rem SPDX-License-Identifier: MIT
rem Project: adecc Scholar

setlocal EnableExtensions DisableDelayedExpansion
if not defined CB_BCC64X (
   echo ERROR: CB_BCC64X is not defined for the Skia BCC64X compile driver.
   exit /b 127
)
set "ADECC_BCC64X_ARGS=%*"
set "ADECC_BCC64X_ARGS=%ADECC_BCC64X_ARGS:/Zc:__cplusplus=%"
set "ADECC_BCC64X_ARGS=%ADECC_BCC64X_ARGS:/std:c++17=-std=c++17%"
set "ADECC_BCC64X_ARGS=%ADECC_BCC64X_ARGS:/std:c++20=-std=c++20%"
"%CB_BCC64X%" -tM %ADECC_BCC64X_ARGS%
exit /b %ERRORLEVEL%
