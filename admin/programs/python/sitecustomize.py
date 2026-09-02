"""Restore normal build-script imports inside the managed embeddable Python runtime."""

from __future__ import annotations

import os
import sys
from pathlib import Path


def _prepend_path(entry: str) -> None:
    if entry and entry not in sys.path:
        sys.path.insert(0, entry)


def _apply_buildengine_pythonpath() -> None:
    value = os.environ.get("PYTHONPATH", "")
    if not value:
        return

    entries = [entry for entry in value.split(os.pathsep) if entry]
    for entry in reversed(entries):
        _prepend_path(entry)


def _apply_script_directory() -> None:
    """Restore the normal ``python script.py`` import root suppressed by ._pth isolation."""
    if not sys.argv:
        return

    argument = sys.argv[0]
    if not argument or argument.startswith("-"):
        return

    script = Path(argument)
    directory = script if script.is_dir() else script.parent
    if not directory.is_absolute():
        directory = Path.cwd() / directory

    try:
        resolved = str(directory.resolve())
    except OSError:
        resolved = str(directory.absolute())

    _prepend_path(resolved)


_apply_buildengine_pythonpath()
_apply_script_directory()
