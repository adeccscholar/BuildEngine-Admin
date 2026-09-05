@echo off
rem Copyright (c) 2026 adecc Systemhaus GmbH
rem SPDX-License-Identifier: MIT
rem Project: adecc Scholar

setlocal EnableExtensions DisableDelayedExpansion
if not defined CB_LLVM_AR (
   echo ERROR: CB_LLVM_AR is not defined for the Skia archive driver.
   exit /b 127
)
"%CB_LLVM_AR%" %*
exit /b %ERRORLEVEL%
