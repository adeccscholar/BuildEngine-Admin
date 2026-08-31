# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar

from __future__ import annotations

import argparse
import shutil
from pathlib import Path


HEADER_EXTENSIONS = {".h", ".hpp", ".inl", ".ipp", ".tpp", ".idl", ".pidl"}


def copy_file(source: Path, target: Path) -> None:
    target.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(source, target)


def copy_headers(source: Path, target: Path) -> None:
    for file in source.rglob("*"):
        if not file.is_file() or file.suffix.lower() not in HEADER_EXTENSIONS:
            continue
        copy_file(file, target / file.relative_to(source))


def find_executable(directory: Path, name: str) -> Path:
    matches = sorted(directory.rglob(name))
    if not matches and name.lower().endswith(".exe"):
        matches = sorted(directory.rglob(name[:-4] + "d.exe"))
    if not matches:
        raise RuntimeError(f"required executable {name!r} is missing below {directory}")
    return matches[0]


def install_variant(root: Path, package: Path, configuration: str) -> None:
    ace_lib = root / "lib"
    ace_bin = root / "bin"
    if not ace_lib.is_dir():
        raise RuntimeError(f"ACE library directory is missing: {ace_lib}")

    library_target = package / "lib" / "win64" / configuration
    binary_target = package / "bin" / "win64" / configuration
    service_target = package / "services" / "bin" / "win64" / configuration
    tool_target = package / "tools" / "bin" / "win64" / configuration

    for target in (library_target, binary_target, service_target, tool_target):
        if target.exists():
            shutil.rmtree(target)
        target.mkdir(parents=True, exist_ok=True)

    for file in ace_lib.rglob("*"):
        if not file.is_file():
            continue
        extension = file.suffix.lower()
        if extension == ".lib":
            copy_file(file, library_target / file.name)
        elif extension in {".dll", ".pdb"}:
            copy_file(file, binary_target / file.name)

    if ace_bin.is_dir():
        for file in ace_bin.iterdir():
            if file.is_file() and file.suffix.lower() in {".dll", ".pdb"}:
                copy_file(file, binary_target / file.name)

    copy_file(ace_bin / "tao_idl.exe", tool_target / "tao_idl.exe")
    copy_file(ace_bin / "ace_gperf.exe", tool_target / "ace_gperf.exe")

    tao = root / "TAO"
    services = {
        "tao_cosnaming.exe": tao / "orbsvcs" / "Naming_Service",
        "tao_cosevent.exe": tao / "orbsvcs" / "CosEvent_Service",
        "tao_rtevent.exe": tao / "orbsvcs" / "Event_Service",
    }
    for name, directory in services.items():
        copy_file(find_executable(directory, name), service_target / name)

    if not list(library_target.glob("ACE*.lib")):
        raise RuntimeError("ACE import libraries were not installed")
    if not list(library_target.glob("TAO*.lib")):
        raise RuntimeError("TAO import libraries were not installed")
    if not list(library_target.glob("ACE_SSL*.lib")):
        raise RuntimeError("ACE SSL import library was not installed")
    if not list(library_target.glob("TAO_SSLIOP*.lib")):
        raise RuntimeError("TAO SSLIOP import library was not installed")


def install_common(root: Path, package: Path) -> None:
    include = package / "include"
    if include.exists():
        shutil.rmtree(include)

    copy_headers(root / "ace", include / "ace")
    copy_headers(root / "TAO" / "tao", include / "tao")
    copy_headers(root / "TAO" / "orbsvcs" / "orbsvcs", include / "orbsvcs")

    for source, name in (
        (root / "COPYING", "COPYING"),
        (root / "VERSION.txt", "ACE_VERSION.txt"),
        (root / "TAO" / "VERSION.txt", "TAO_VERSION.txt"),
    ):
        if source.is_file():
            copy_file(source, package / name)

    required = (
        include / "ace" / "ACE.h",
        include / "tao" / "corba.h",
        include / "orbsvcs" / "CosNamingC.h",
        package / "COPYING",
    )
    for path in required:
        if not path.is_file():
            raise RuntimeError(f"required ACE+TAO package file is missing: {path}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("mode", choices=("variant", "common"))
    parser.add_argument("--root", required=True, type=Path)
    parser.add_argument("--package", required=True, type=Path)
    parser.add_argument("--configuration")
    args = parser.parse_args()

    root = args.root.resolve()
    package = args.package.resolve()
    package.mkdir(parents=True, exist_ok=True)

    if args.mode == "variant":
        if not args.configuration:
            raise RuntimeError("variant install requires --configuration")
        install_variant(root, package, args.configuration)
    else:
        install_common(root, package)

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
