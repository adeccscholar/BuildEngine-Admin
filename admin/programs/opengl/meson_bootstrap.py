#!/usr/bin/env python3
"""Run a pinned Meson source tree with the managed BuildEngine Python runtime."""

from __future__ import annotations

import runpy
import sys
from pathlib import Path


def main() -> None:
    if len(sys.argv) < 3:
        raise SystemExit("usage: meson_bootstrap.py <meson-source-root> <meson arguments...>")

    meson_root = Path(sys.argv[1]).resolve()
    meson_entry = meson_root / "meson.py"
    if not meson_entry.is_file():
        raise SystemExit(f"Meson entry point not found: {meson_entry}")

    sys.path.insert(0, str(meson_root))
    sys.argv = [str(meson_entry), *sys.argv[2:]]
    runpy.run_path(str(meson_entry), run_name="__main__")


if __name__ == "__main__":
    main()
