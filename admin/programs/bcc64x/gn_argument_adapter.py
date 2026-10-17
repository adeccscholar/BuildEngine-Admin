# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
"""Translate explicitly supported Chromium/MSVC arguments for BCC64X.

This is a generic compiler-driver adapter. It never silently drops flags.
An unsupported MSVC option fails before invoking the compiler.
"""
import re
import subprocess
import sys


WARNING_EQUIVALENTS = {
    "4244": ("-Wno-shorten-64-to-32", "-Wno-implicit-int-conversion",
             "-Wno-implicit-float-conversion", "-Wno-float-conversion"),
}


def Translate(a_strArgument: str) -> list[str]:
    a_match = re.fullmatch(r"/wd([0-9]{4})", a_strArgument, re.IGNORECASE)
    if a_match:
        strNumber = a_match.group(1)
        if strNumber not in WARNING_EQUIVALENTS:
            raise ValueError(
                f"no verified BCC64X warning equivalent for MSVC C{strNumber}"
            )
        return list(WARNING_EQUIVALENTS[strNumber])
    if a_strArgument.startswith("/") and not a_strArgument.startswith("//"):
        raise ValueError(f"unsupported MSVC option for BCC64X: {a_strArgument}")
    return [a_strArgument]


def Main() -> int:
    if len(sys.argv) < 3:
        print("usage: gn_argument_adapter.py <compiler> <arguments...>", file=sys.stderr)
        return 2
    vecArguments = []
    try:
        for strArgument in sys.argv[2:]:
            vecArguments.extend(Translate(strArgument))
    except ValueError as aError:
        print(f"BCC64X argument translation failed: {aError}", file=sys.stderr)
        return 2
    return subprocess.call([sys.argv[1], *vecArguments])


if __name__ == "__main__":
    sys.exit(Main())
