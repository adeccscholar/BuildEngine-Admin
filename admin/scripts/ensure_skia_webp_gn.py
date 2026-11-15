# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
"""Synchronize the version-pinned external WebP GN include contract in a staged Skia tree.

This guards incremental BuildEngine builds whose source job completed before the
current WebP GN patch was updated. Only the two exact upstream codec target
blocks and the already-externalized libwebp group are accepted.
"""

from __future__ import annotations

import pathlib
import sys


CONFIG_LINE = '  configs = [ "//third_party/libwebp:managed_webp_includes" ]\n'
CONFIG_TEXT = (
   '  # Forward managed libwebp headers to the WebP source sets.\n'
   '  config("managed_webp_includes") {\n'
   '    include_dirs = [ bcc64x_libwebp_include ]\n'
   '  }\n\n'
)


def EnsureCodecConfig(strText: str, strTarget: str) -> tuple[str, bool]:
   strHeader = f'optional("{strTarget}") {{\n'
   if strText.count(strHeader) != 1:
      raise RuntimeError(f"Expected exactly one GN target: {strTarget}")
   iStart = strText.index(strHeader)
   iEnd = strText.find('\n}\n', iStart)
   if iEnd == -1:
      raise RuntimeError(f"Unterminated GN target: {strTarget}")
   strBlock = strText[iStart:iEnd + 3]
   if CONFIG_LINE in strBlock:
      return strText, False
   if '  configs = ' in strBlock:
      raise RuntimeError(f"Unexpected existing GN configs in {strTarget}; manual review required")
   strAnchor = '  public_defines = [ "SK_CODEC_DECODES_WEBP" ]\n' if strTarget == "webp_decode" else '  public = skia_encode_webp_public\n'
   if strBlock.count(strAnchor) != 1:
      raise RuntimeError(f"GN source layout changed for {strTarget}")
   strNewBlock = strBlock.replace(strAnchor, strAnchor + CONFIG_LINE)
   return strText[:iStart] + strNewBlock + strText[iEnd + 3:], True


def Main() -> int:
   if len(sys.argv) != 2:
      print("Usage: ensure_skia_webp_gn.py SKIA_SOURCE_ROOT", file=sys.stderr)
      return 2

   pathRoot = pathlib.Path(sys.argv[1])
   pathThirdParty = pathRoot / "third_party" / "libwebp" / "BUILD.gn"
   pathBuild = pathRoot / "BUILD.gn"

   try:
      strThirdParty = pathThirdParty.read_text(encoding="utf-8")
      strBuild = pathBuild.read_text(encoding="utf-8")

      # The first externalization patch is a mandatory precondition. Do not
      # silently convert a bundled build or alter the managed import archives.
      if 'bcc64x_libwebp_include = ""' not in strThirdParty or 'bcc64x_libwebp_lib = ""' not in strThirdParty:
         raise RuntimeError("Staged third_party/libwebp/BUILD.gn lacks the external WebP GN patch; rerun Skia source preparation")

      bThirdPartyChanged = False
      if 'config("managed_webp_includes")' not in strThirdParty:
         strAnchor = 'if (skia_use_system_libwebp) {\n'
         if strThirdParty.count(strAnchor) != 1:
            raise RuntimeError("Unexpected third_party/libwebp/BUILD.gn externalization layout")
         strThirdParty = strThirdParty.replace(strAnchor, strAnchor + CONFIG_TEXT, 1)
         bThirdPartyChanged = True

      bBuildChanged = False
      for strTarget in ("webp_decode", "webp_encode"):
         strBuild, bChanged = EnsureCodecConfig(strBuild, strTarget)
         bBuildChanged |= bChanged

      if bThirdPartyChanged:
         pathThirdParty.write_text(strThirdParty, encoding="utf-8", newline="\n")
      if bBuildChanged:
         pathBuild.write_text(strBuild, encoding="utf-8", newline="\n")

      print(f"[SKIA WEBP] Staged GN source synchronized: third_party={bThirdPartyChanged}, codecs={bBuildChanged}")
      return 0
   except (OSError, RuntimeError) as theError:
      print(f"[SKIA WEBP] {theError}", file=sys.stderr)
      return 1


if __name__ == "__main__":
   raise SystemExit(Main())
