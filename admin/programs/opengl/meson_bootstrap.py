#!/usr/bin/env python3
"""Run a pinned Meson source tree with the managed BuildEngine Python runtime."""

from __future__ import annotations

import os
import runpy
import sys
from pathlib import Path


def configure_bcc64x_library_path() -> None:
    """Expose the RAD Studio Win64X import-library roots to the BCC64X driver."""
    compiler = os.environ.get("CC", "")
    if Path(compiler).name.casefold() != "bcc64x.exe":
        return

    bds = os.environ.get("BDS", "")
    if not bds:
        raise SystemExit("BCC64X Meson bootstrap requires BDS from rsvars.bat")

    library_roots = [
        Path(bds) / "lib" / "win64x" / "release",
        Path(bds) / "lib" / "win64x" / "release" / "psdk",
    ]
    missing = [path for path in library_roots if not path.is_dir()]
    if missing:
        raise SystemExit(
            "BCC64X Meson bootstrap is missing RAD library roots: "
            + "; ".join(str(path) for path in missing)
        )

    existing = [entry for entry in os.environ.get("LIBRARY", "").split(os.pathsep) if entry]
    known = {str(Path(entry)).casefold() for entry in existing}
    prefix = [str(path) for path in library_roots if str(path).casefold() not in known]
    os.environ["LIBRARY"] = os.pathsep.join([*prefix, *existing])


def main() -> None:
    if len(sys.argv) < 3:
        raise SystemExit("usage: meson_bootstrap.py <meson-source-root> <meson arguments...>")

    configure_bcc64x_library_path()

    meson_root = Path(sys.argv[1]).resolve()
    meson_entry = meson_root / "meson.py"
    if not meson_entry.is_file():
        raise SystemExit(f"Meson entry point not found: {meson_entry}")

    sys.path.insert(0, str(meson_root))
    sys.argv = [str(meson_entry), *sys.argv[2:]]
    runpy.run_path(str(meson_entry), run_name="__main__")


if __name__ == "__main__":
    main()
