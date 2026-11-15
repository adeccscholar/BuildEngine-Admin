# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
"""Fail fast when the generated Skia WebP codec targets lack managed headers.

Usage: verify_skia_webp_gn.py GN_EXE GN_OUT LIBWEBP_INCLUDE

The installed libwebp include root is supplied by the BuildEngine XML dependency
contract. The script checks generated Ninja configuration via GN's target
description, not the presence of a patch file in the repository.
"""

from __future__ import annotations

import json
import os
import pathlib
import subprocess
import sys


def NormalizePath(strValue: str) -> str:
   return os.path.normcase(os.path.normpath(strValue.replace("/", os.sep)))


def Main() -> int:
   if len(sys.argv) != 4:
      print("Expected GN_EXE GN_OUT LIBWEBP_INCLUDE", file=sys.stderr)
      return 2

   strGn, strOut, strInclude = sys.argv[1:]
   pathInclude = pathlib.Path(strInclude)
   if not (pathInclude / "webp" / "encode.h").is_file():
      print(f"[SKIA WEBP] Missing installed header: {pathInclude / 'webp' / 'encode.h'}", file=sys.stderr)
      return 1
   if not (pathInclude / "webp" / "decode.h").is_file():
      print(f"[SKIA WEBP] Missing installed header: {pathInclude / 'webp' / 'decode.h'}", file=sys.stderr)
      return 1

   strExpected = NormalizePath(strInclude)
   for strTarget in ("//:webp_decode", "//:webp_encode"):
      theProcess = subprocess.run(
         [strGn, "desc", strOut, strTarget, "include_dirs", "--format=json"],
         check=False, capture_output=True, text=True, encoding="utf-8", errors="replace")
      if theProcess.returncode:
         print(f"[SKIA WEBP] GN cannot describe {strTarget}: {theProcess.stderr}", file=sys.stderr)
         return 1
      try:
         theIncludes = json.loads(theProcess.stdout)
      except json.JSONDecodeError as theError:
         print(f"[SKIA WEBP] Invalid GN JSON for {strTarget}: {theError}", file=sys.stderr)
         return 1
      if isinstance(theIncludes, dict):
         theIncludes = theIncludes.get("include_dirs", [])
      if not isinstance(theIncludes, list):
         print(f"[SKIA WEBP] Unexpected GN include_dirs for {strTarget}: {theIncludes!r}", file=sys.stderr)
         return 1

      vecIncludes = [NormalizePath(strPath) for strPath in theIncludes if isinstance(strPath, str)]
      if strExpected not in vecIncludes:
         print(f"[SKIA WEBP] {strTarget} does not contain managed include root: {strInclude}", file=sys.stderr)
         print(f"[SKIA WEBP] Actual GN include directories: {theIncludes}", file=sys.stderr)
         print("[SKIA WEBP] Skia sources or generated GN output are stale. Re-run the source patch phase and GN generation.", file=sys.stderr)
         return 1
      print(f"[SKIA WEBP] {strTarget}: managed libwebp headers verified")

   return 0


if __name__ == "__main__":
   raise SystemExit(Main())
