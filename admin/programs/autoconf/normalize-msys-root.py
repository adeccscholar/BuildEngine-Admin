# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

"""Normalize one MSYS drive-root prefix in generated text files for native Windows tools."""

from __future__ import annotations

import argparse
from pathlib import Path


def windows_root_to_msys(root: str) -> tuple[str, str]:
    normalized = root.replace("\\", "/").rstrip("/")
    if len(normalized) < 3 or normalized[1:3] != ":/":
        raise ValueError(f"expected absolute Windows drive path, got: {root}")

    drive = normalized[0].lower()
    tail = normalized[2:]
    return f"/{drive}{tail}", normalized


def normalize_tree(tree: Path, windows_root: str) -> int:
    msys_root, native_root = windows_root_to_msys(windows_root)
    changed = 0

    for path in tree.rglob("*"):
        if not path.is_file():
            continue

        try:
            data = path.read_bytes()
        except OSError:
            continue

        if b"\0" in data:
            continue

        try:
            text = data.decode("utf-8")
        except UnicodeDecodeError:
            continue

        replaced = text.replace(msys_root, native_root)
        if replaced == text:
            continue

        path.write_text(replaced, encoding="utf-8", newline="")
        changed += 1

    return changed


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("tree")
    parser.add_argument("windows_root")
    args = parser.parse_args()

    tree = Path(args.tree)
    if not tree.is_dir():
        raise SystemExit(f"generated build tree does not exist: {tree}")

    changed = normalize_tree(tree, args.windows_root)
    print(f"normalized MSYS root in {changed} generated text file(s)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
