from __future__ import annotations

import argparse
import os
import shutil
import tarfile
from pathlib import Path, PurePosixPath


def parse_arguments() -> argparse.Namespace:
   parser = argparse.ArgumentParser(
      description="Safely extract selected regular files from a tar.xz archive."
   )
   parser.add_argument("--archive", required=True, type=Path)
   parser.add_argument("--target", required=True, type=Path)
   parser.add_argument("--root", required=True)
   parser.add_argument("--include", action="append", default=[])
   parser.add_argument("--require", action="append", default=[])
   return parser.parse_args()


def relative_member_path(name: str, root: PurePosixPath) -> PurePosixPath | None:
   path = PurePosixPath(name)
   if(path.is_absolute() or ".." in path.parts):
      raise RuntimeError(f"unsafe archive path: {name}")
   if(path.parts[:len(root.parts)] != root.parts):
      return None
   relative = PurePosixPath(*path.parts[len(root.parts):])
   if(not relative.parts):
      return None
   return relative


def selected(path: PurePosixPath, patterns: list[str]) -> bool:
   return any(path.match(pattern) for pattern in patterns)


def extract_archive(arguments: argparse.Namespace) -> None:
   archive = arguments.archive.resolve(strict=True)
   target = arguments.target.resolve()
   temporary = target.with_name(target.name + ".extracting")
   root = PurePosixPath(arguments.root)

   if(root.is_absolute() or not root.parts or ".." in root.parts):
      raise RuntimeError("archive root must be a safe relative path")
   if(not arguments.include):
      raise RuntimeError("at least one --include pattern is required")

   shutil.rmtree(temporary, ignore_errors=True)
   temporary.mkdir(parents=True)

   try:
      extracted = 0
      with tarfile.open(archive, mode="r:xz") as source:
         for member in source:
            relative = relative_member_path(member.name, root)
            if(relative is None or not selected(relative, arguments.include)):
               continue
            if(not member.isfile()):
               raise RuntimeError(f"selected archive entry is not a regular file: {member.name}")
            destination = temporary.joinpath(*relative.parts)
            destination.parent.mkdir(parents=True, exist_ok=True)
            stream = source.extractfile(member)
            if(stream is None):
               raise RuntimeError(f"cannot read archive entry: {member.name}")
            with stream, destination.open("wb") as output:
               shutil.copyfileobj(stream, output)
            extracted += 1

      if(extracted == 0):
         raise RuntimeError("archive selection did not extract any file")
      for required in arguments.require:
         required_path = PurePosixPath(required)
         if(required_path.is_absolute() or ".." in required_path.parts):
            raise RuntimeError(f"unsafe required path: {required}")
         if(not temporary.joinpath(*required_path.parts).is_file()):
            raise RuntimeError(f"required extracted file is missing: {required}")

      if(target.exists()):
         shutil.rmtree(target)
      os.replace(temporary, target)
      print(f"extracted {extracted} selected files to {target}")
   except Exception:
      shutil.rmtree(temporary, ignore_errors=True)
      raise


def main() -> int:
   extract_archive(parse_arguments())
   return 0


if(__name__ == "__main__"):
   raise SystemExit(main())
