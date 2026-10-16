#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
"""Preflight all pinned Skia archives without invoking a BuildEngine build.

Uses a ranged GET and closes the response after a short header read.
The response must look like a ZIP or GZIP stream. Redirects are followed.
For hosts ignoring Range the response is deliberately not fully downloaded.
"""
import argparse
import sys
import urllib.error
import urllib.request
import xml.etree.ElementTree as ET
from pathlib import Path


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--catalog", type=Path, default=Path(__file__).resolve().parents[1] / "build-libraries.xml")
    ap.add_argument("--timeout", type=float, default=20.0)
    args = ap.parse_args()
    root = ET.parse(args.catalog).getroot()
    skia = next((e for e in root.iter("library") if e.get("id") == "skia" and e.get("version") == "153"), None)
    if skia is None:
        ap.error("skia 153 is absent from catalog")
    source = skia.find("source")
    downloads = [] if source is None else source.findall("download")
    if not downloads:
        ap.error("skia source has no downloads")
    failures = 0
    print("SKIA DOWNLOAD PREFLIGHT | {} pinned archives".format(len(downloads)), flush=True)
    for index, item in enumerate(downloads, 1):
        url, name = item.attrib["url"], item.attrib["archive"]
        request = urllib.request.Request(url, headers={
            "User-Agent": "BuildEngine-Skia-Preflight/1.0",
            "Range": "bytes=0-1023",
            "Accept": "application/octet-stream",
        })
        try:
            with urllib.request.urlopen(request, timeout=args.timeout) as response:
                status = response.status
                header = response.read(8)
                ctype = response.headers.get("Content-Type", "")
                actual_url = response.url
            gzip = header.startswith(b"\\x1f\\x8b")
            zipfile = header.startswith(b"PK\\x03\\x04")
            expected_zip = name.lower().endswith(".zip")
            valid = (zipfile if expected_zip else gzip) and status in (200, 206)
            detail = "HTTP {} type={} magic={} destination={}".format(
                status, ctype, header.hex(), actual_url)
            if not valid:
                failures += 1
            print("{:02d}/{} {} {} | {}".format(index, len(downloads), "PASS" if valid else "FAIL", name, detail), flush=True)
        except (OSError, urllib.error.URLError, TimeoutError) as exc:
            failures += 1
            print("{:02d}/{} FAIL {} | {} | {}".format(index, len(downloads), name, url, exc), flush=True)
    print("SKIA DOWNLOAD PREFLIGHT: {} passed, {} failed".format(len(downloads) - failures, failures))
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main())
