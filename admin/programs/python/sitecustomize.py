"""Import BuildEngine-provided Python module paths into the managed runtime."""

from __future__ import annotations

import os
import sys


def _apply_buildengine_pythonpath() -> None:
    value = os.environ.get("PYTHONPATH", "")
    if not value:
        return

    entries = [entry for entry in value.split(os.pathsep) if entry]
    for entry in reversed(entries):
        if entry not in sys.path:
            sys.path.insert(0, entry)


_apply_buildengine_pythonpath()
