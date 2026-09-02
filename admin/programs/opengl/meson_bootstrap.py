#!/usr/bin/env python3
"""Run pinned Meson with the managed BuildEngine Python runtime."""

from __future__ import annotations

import runpy
import sys
from pathlib import Path


def patch_windows_lld_allow_undefined(meson_root: Path) -> None:
    """Apply the Meson 1.12.0 Windows-LLD compatibility fix used by BCC64X."""
    linkers = meson_root / "mesonbuild" / "linkers" / "linkers.py"
    if not linkers.is_file():
        raise SystemExit(f"Meson linker implementation not found: {linkers}")

    original_probe = """        self.has_allow_shlib_undefined = self._supports_flag('--allow-shlib-undefined', always_args)
"""
    patched_probe = """        self.has_allow_shlib_undefined = (
            not env.machines[for_machine].is_windows()
            and self._supports_flag('--allow-shlib-undefined', always_args)
        )
"""

    previous_method = """    def get_allow_undefined_args(self) -> T.List[str]:
        if self.system == 'windows':
            return []
        if self.has_allow_shlib_undefined:
            return self._apply_prefix('--allow-shlib-undefined')
        return []
"""
    original_method = """    def get_allow_undefined_args(self) -> T.List[str]:
        if self.has_allow_shlib_undefined:
            return self._apply_prefix('--allow-shlib-undefined')
        return []
"""

    text = linkers.read_text(encoding="utf-8")

    # Migrate an already patched workspace from the previous compatibility
    # attempt back to the upstream method before applying the real probe fix.
    if previous_method in text:
        text = text.replace(previous_method, original_method, 1)

    if patched_probe in text:
        linkers.write_text(text, encoding="utf-8", newline="\n")
        return

    if original_probe not in text:
        raise SystemExit(
            "Meson 1.12.0 Windows-LLD capability patch context does not match"
        )

    text = text.replace(original_probe, patched_probe, 1)
    linkers.write_text(text, encoding="utf-8", newline="\n")


def main() -> None:
    if len(sys.argv) < 3:
        raise SystemExit("usage: meson_bootstrap.py <meson-source-root> <meson arguments...>")

    meson_root = Path(sys.argv[1]).resolve()
    meson_entry = meson_root / "meson.py"
    if not meson_entry.is_file():
        raise SystemExit(f"Meson entry point not found: {meson_entry}")

    patch_windows_lld_allow_undefined(meson_root)

    sys.path.insert(0, str(meson_root))
    sys.argv = [str(meson_entry), *sys.argv[2:]]
    runpy.run_path(str(meson_entry), run_name="__main__")


if __name__ == "__main__":
    main()
