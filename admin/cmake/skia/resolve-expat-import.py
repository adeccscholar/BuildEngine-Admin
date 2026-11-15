# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
"""Resolve an installed Expat import archive for Skia's GN configuration.

Read the installed CMake target export first. Only fall back to inspecting the
installed library directory when the export does not provide an import path.
Never guess the link filename from the package name or build configuration.
"""

from __future__ import annotations

import argparse
from pathlib import Path
import re
import sys


def resolve(root: Path, configuration: str) -> Path:
    config = configuration.upper()
    lib_dir = root / "lib" / "win64" / configuration
    if not lib_dir.is_dir():
        raise RuntimeError(f"Expat library directory is missing: {lib_dir}")

    candidates: set[Path] = set()
    exports = sorted(lib_dir.rglob("*targets-*.cmake"))
    pattern = re.compile(
        r"IMPORTED_IMPLIB_" + re.escape(config) + r'\s+"([^"]+)"'
    )

    for export in exports:
        data = export.read_text(encoding="utf-8")
        for value in pattern.findall(data):
            # CMake target files normally use ${_IMPORT_PREFIX} relative
            # to the installed package root.
            path = value.replace("${_IMPORT_PREFIX}", root.as_posix())
            if "${" in path:
                raise RuntimeError(
                    f"Unresolved CMake export variable in {export}: {value}"
                )
            candidates.add(Path(path))

    if not candidates:
        candidates = {
            path
            for path in lib_dir.iterdir()
            if path.is_file()
            and "expat" in path.name.lower()
            and (
                path.name.lower().endswith(".lib")
                or path.name.lower().endswith(".dll.a")
                or path.name.lower().endswith(".a")
            )
        }

    if len(candidates) != 1:
        detail = ", ".join(str(path) for path in sorted(candidates))
        raise RuntimeError(
            f"Expected exactly one Expat import archive for {configuration}; "
            f"found {len(candidates)}: {detail}"
        )

    result = next(iter(candidates))
    if not result.is_file():
        raise RuntimeError(f"Installed Expat import archive is missing: {result}")
    return result.resolve()


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("package_root", type=Path)
    parser.add_argument("configuration", choices=["Debug", "Release"])
    args = parser.parse_args()

    try:
        print(resolve(args.package_root.resolve(), args.configuration).as_posix())
    except (OSError, RuntimeError) as error:
        print(f"BuildEngine Expat resolution failed: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
