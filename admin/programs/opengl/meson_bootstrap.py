#!/usr/bin/env python3
"""Run pinned Meson with the managed BuildEngine Python runtime."""

from __future__ import annotations

import os
import runpy
import sys
import xml.etree.ElementTree as ET
from pathlib import Path


def is_bcc64x() -> bool:
    """Return whether the configured compiler is Embarcadero BCC64X."""
    compiler = os.environ.get("CC", "").strip().strip('"')
    return Path(compiler).name.casefold() in {"bcc64x", "bcc64x.exe"}


def expose_buildengine_tools() -> None:
    """Expose BuildEngine-verified Mesa helper tools through PATH."""
    tools_state = Path(
        os.environ.get("BUILDENGINE_TOOLS_STATE", Path.cwd() / "admin" / "tools.xml")
    ).resolve()
    if not tools_state.is_file():
        raise SystemExit(f"BuildEngine tool registry not found: {tools_state}")

    try:
        root = ET.parse(tools_state).getroot()
    except (ET.ParseError, OSError) as error:
        raise SystemExit(f"BuildEngine tool registry cannot be read: {error}") from error

    if root.tag != "tools":
        raise SystemExit(f"BuildEngine tool registry has unexpected root: {root.tag}")

    tools = {node.get("id", ""): node for node in root.findall("tool")}
    required = {
        "winflexbison": ("win_flex.exe", "win_bison.exe"),
        "pkg-config": ("pkg-config.exe",),
    }

    directories: list[str] = []
    for tool_id, expected_names in required.items():
        node = tools.get(tool_id)
        if node is None:
            raise SystemExit(f"BuildEngine tool is not registered: {tool_id}")

        executable = Path(node.get("path", ""))
        if not executable.is_file():
            raise SystemExit(
                f"BuildEngine registered tool executable does not exist: {tool_id}: {executable}"
            )

        directory = executable.parent
        for expected_name in expected_names:
            expected = directory / expected_name
            if not expected.is_file():
                raise SystemExit(
                    f"BuildEngine managed tool companion is missing: {tool_id}: {expected}"
                )

        directory_text = str(directory)
        if directory_text.casefold() not in {value.casefold() for value in directories}:
            directories.append(directory_text)

    existing_path = os.environ.get("PATH", "")
    os.environ["PATH"] = os.pathsep.join([*directories, existing_path])


def patch_bcc64x_lld_detection(meson_root: Path) -> None:
    """Let Meson 1.12.0 detect LLD behind the BCC64X compiler banner."""
    if not is_bcc64x():
        return

    detect = meson_root / "mesonbuild" / "linkers" / "detect.py"
    if not detect.is_file():
        raise SystemExit(f"Meson linker detection implementation not found: {detect}")

    original_win = """    if 'LLD' in o.split('\\n', maxsplit=1)[0]:
        if 'compatible with GNU linkers' in o:
"""
    patched_win = """    if 'LLD' in o:
        if 'compatible with GNU linkers' in o:
"""

    original_nix = """    if 'LLD' in o.split('\\n', maxsplit=1)[0] or 'tiarmlnk' in e:
"""
    patched_nix = """    if 'LLD' in o or 'tiarmlnk' in e:
"""

    text = detect.read_text(encoding="utf-8")

    win_is_patched = patched_win in text
    nix_is_patched = patched_nix in text
    if win_is_patched and nix_is_patched:
        return

    if not win_is_patched:
        if text.count(original_win) != 1:
            raise SystemExit(
                "Meson 1.12.0 Windows LLD detection patch context does not match"
            )
        text = text.replace(original_win, patched_win, 1)

    if not nix_is_patched:
        if text.count(original_nix) != 1:
            raise SystemExit(
                "Meson 1.12.0 Unix-like LLD detection patch context does not match"
            )
        text = text.replace(original_nix, patched_nix, 1)

    detect.write_text(text, encoding="utf-8", newline="\n")


def patch_windows_lld_allow_undefined(meson_root: Path) -> None:
    """Do not probe the unsupported GNU allow-undefined switch on Windows LLD."""
    if not is_bcc64x():
        return

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

    # Migrate an already patched workspace from the first compatibility
    # attempt back to the upstream method before applying the capability fix.
    if previous_method in text:
        text = text.replace(previous_method, original_method, 1)

    if patched_probe in text:
        linkers.write_text(text, encoding="utf-8", newline="\n")
        return

    if text.count(original_probe) != 1:
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

    expose_buildengine_tools()
    patch_bcc64x_lld_detection(meson_root)
    patch_windows_lld_allow_undefined(meson_root)

    sys.path.insert(0, str(meson_root))
    sys.argv = [str(meson_entry), *sys.argv[2:]]
    runpy.run_path(str(meson_entry), run_name="__main__")


if __name__ == "__main__":
    main()
