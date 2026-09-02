#!/usr/bin/env python3
"""Run a pinned Meson source tree with the managed BuildEngine Python runtime."""

from __future__ import annotations

import os
import runpy
import sys
from pathlib import Path


def configure_bcc64x_library_path() -> None:
    """Expose available RAD Studio Win64X and Platform SDK library roots to BCC64X/Meson."""
    compiler = os.environ.get("CC", "").strip().strip('"')
    compiler_path = Path(compiler)
    if compiler_path.name.casefold() not in {"bcc64x", "bcc64x.exe"}:
        return

    bds_value = os.environ.get("BDS", "").strip().strip('"')
    if bds_value:
        bds = Path(bds_value)
    else:
        try:
            bds = compiler_path.resolve().parent.parent
        except OSError:
            return

    candidates = [
        bds / "lib" / "win64x" / "release",
        bds / "lib" / "psdk",
        bds / "lib" / "win64x" / "release" / "psdk",
    ]
    library_roots = [path for path in candidates if path.is_dir()]
    if not library_roots:
        return

    existing_library = [entry for entry in os.environ.get("LIBRARY", "").split(os.pathsep) if entry]
    known = {str(Path(entry)).casefold() for entry in existing_library}
    library_prefix = [str(path) for path in library_roots if str(path).casefold() not in known]
    os.environ["LIBRARY"] = os.pathsep.join([*library_prefix, *existing_library])

    existing_ldflags = os.environ.get("LDFLAGS", "").strip()
    link_prefix = " ".join(f'-L"{path}"' for path in library_roots)
    os.environ["LDFLAGS"] = f"{link_prefix} {existing_ldflags}".strip()


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
