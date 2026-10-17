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
    "4267": ("-Wno-shorten-64-to-32", "-Wno-implicit-int-conversion"),
    "4800": ("-Wno-bool-conversion",),
}


def Translate(a_strArgument: str) -> list[str]:
    # C4117 is specific to MSVC macro diagnostics and has no Clang
    # warning ID. The remaining warning diagnostics remain enabled.
    if a_strArgument.lower() == "/wd4117":
        return []
    a_match = re.fullmatch(r"/wd([0-9]{4})", a_strArgument, re.IGNORECASE)
    if a_match:
        strNumber = a_match.group(1)
        if strNumber not in WARNING_EQUIVALENTS:
            raise ValueError(
                f"no verified BCC64X warning equivalent for MSVC C{strNumber}"
            )
        return list(WARNING_EQUIVALENTS[strNumber])
    # Semantically corresponding Clang/GNU frontend options.
    mpDirect = {
        "/WX": "-Werror",
        "/Gy": "-ffunction-sections",
        "/Gw": "-fdata-sections",
        "/Oy-": "-fno-omit-frame-pointer",
        "/O2": "-O2",
        "/Od": "-O0",
        "/Ob0": "-fno-inline",
        "/GF": "-fmerge-all-constants",
        "/GR-": "-fno-rtti",
        "/GR": "-frtti",
        "/Zi": "-gcodeview",
        "/utf-8": "-finput-charset=UTF-8",
    }
    if a_strArgument == "/TC":
        # The C GN tool already selects a .c source file. BCC64X infers C mode
        # from the extension; MSVC /TC is not passed through.
        return []
    if a_strArgument in mpDirect:
        return [mpDirect[a_strArgument]]
    # These MSVC switches have no distinct effect under the BCC64X
    # Clang/COFF frontend. Document every consumed switch explicitly.
    mpIntrinsic = {
        "/FS": "PDB synchronization is MSVC-specific; BCC64X emits per-object debug data",
        "/bigobj": "LLVM COFF writer supports extended sections natively",
        "/Zc:inline": "Clang applies standard C++ inline semantics",
        "/Zc:__cplusplus": "Clang reports the active standard in __cplusplus",
    }
    if a_strArgument in mpIntrinsic:
        return []
    if a_strArgument in ("/MD", "/MDd", "/MT", "/MTd"):
        # Chromium chooses /MD and /MDd for dynamic MSVC CRT when building
        # components. The BuildEngine BCC64X contract always selects its own
        # dynamic RTL (-tR), never MSVC's debug or static CRT. Retain the
        # explicit existing /MT and /MTd compatibility mapping for now.
        return ["-tR"]
    # Skia's skcms chooses ISA-specific compilation units through /arch.
    # Do not translate these to a host-dependent -march=native.
    mpArchitecture = {
        "/arch:AVX2": ("-mavx2", "-mf16c", "-mfma"),
        "/arch:AVX512": ("-mavx512f", "-mavx512bw",
                         "-mavx512dq", "-mavx512vl", "-mf16c", "-mfma"),
    }
    if a_strArgument in mpArchitecture:
        return list(mpArchitecture[a_strArgument])
    if a_strArgument.startswith("/D"):
        return ["-D" + a_strArgument[2:]]
    if a_strArgument.startswith("/std:"):
        return ["-std=" + a_strArgument[5:]]
    if a_strArgument.startswith("/clang:"):
        return [a_strArgument[7:]]
    if a_strArgument.startswith("/") and not a_strArgument.startswith("//"):
        raise ValueError(f"unsupported MSVC option for BCC64X: {a_strArgument}")
    return [a_strArgument]


def Main() -> int:
    if len(sys.argv) < 3:
        print("usage: gn_argument_adapter.py <compiler> <arguments...>", file=sys.stderr)
        return 2
    vecArguments = []
    lstErrors = []
    for strArgument in sys.argv[2:]:
        try:
            vecArguments.extend(Translate(strArgument))
        except ValueError as aError:
            if str(aError) not in lstErrors:
                lstErrors.append(str(aError))
    # Chromium fixes predefined build timestamps for reproducibility.
    # Retain their values; suppress only Clang's dedicated redefinition warning.
    if any(arg.startswith(("-D__DATE__=", "-D__TIME__=",
                           "-D__TIMESTAMP__=")) for arg in vecArguments):
        vecArguments.append("-Wno-builtin-macro-redefined")
    # Some Skia revisions select the MSVC spelling in SkAttributes.h.
    # Restore the precise always-inline intent only for that Skia configuration.
    if any("SK_USER_CONFIG_HEADER=" in arg for arg in vecArguments):
        vecArguments.append("-D__forceinline=inline __attribute__((always_inline))")
    # HSW raster code explicitly calls fused multiply-add intrinsics.
    # Restrict the target feature to its dedicated object.
    if any(arg.replace(chr(92), "/").endswith("/src/opts/SkOpts_hsw.cpp")
           for arg in vecArguments) and "-mfma" not in vecArguments:
        vecArguments.append("-mfma")
    if lstErrors:
        print("BCC64X argument translation failed:", file=sys.stderr)
        for strError in lstErrors:
            print("  " + strError, file=sys.stderr)
        return 2
    return subprocess.call([sys.argv[1], *vecArguments])


if __name__ == "__main__":
    sys.exit(Main())
