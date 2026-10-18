# SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
"""Translate explicitly supported Chromium/MSVC arguments for BCC64X.

This is a generic compiler-driver adapter. It never silently drops flags.
An unsupported MSVC option fails before invoking the compiler.
"""
import re
import subprocess
import sys
from pathlib import Path


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
        "/Zc:sizedDealloc-": "-fno-sized-deallocation",
        "/Z7": "-gcodeview",
        "/W3": "-Wall",
        "/W4": ("-Wall", "-Wextra"),
        "/guard:cf": "-mguard=cf",
    }
    # Chromium's newer Clang accepts this warning suppression; BCC64X's
    # pinned Clang does not define that diagnostic. It cannot suppress an
    # unknown diagnostic, so the exact upstream suppression is inapplicable.
    if a_strArgument == "-Wno-uninitialized-const-pointer":
        return []
    if a_strArgument == "/TC":
        # The C GN tool already selects a .c source file. BCC64X infers C mode
        # from the extension; MSVC /TC is not passed through.
        return []
    if a_strArgument in mpDirect:
        mapping = mpDirect[a_strArgument]
        return list(mapping) if isinstance(mapping, tuple) else [mapping]
    # These MSVC switches have no distinct effect under the BCC64X
    # Clang/COFF frontend. Document every consumed switch explicitly.
    mpIntrinsic = {
        "/FS": "PDB synchronization is MSVC-specific; BCC64X emits per-object debug data",
        "/bigobj": "LLVM COFF writer supports extended sections natively",
        "/Zc:inline": "Clang applies standard C++ inline semantics",
        "/Zc:twoPhase": "Clang applies standard two-phase C++ name lookup",
        "/Brepro": "Clang reproducibility is enforced by existing -ffile-reproducible and debug-prefix configuration",
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



def TranslateLinkResponse(strContent: str) -> str:
    """Forward GN's MSVC linker switches to LLD's COFF driver on Windows.

    BCC64X uses Clang's MinGW driver: an unconverted /CETCOMPAT is a file
    name. -Xlinker passes -Xlink=/CETCOMPAT to lld's MinGW driver, which
    forwards that exact switch to lld's COFF driver. Preserve comma-valued
    options such as /OPT:REF,ICF by using -Xlinker, not -Wl's comma syntax.
    Leave paths and GNU flags unchanged. The COFF driver still validates
    whether a forwarded switch is supported.
    """
    if re.search(r'(?<!\S)"/[A-Za-z][^"]*"', strContent):
        raise ValueError("quoted MSVC linker switch requires explicit handling")
    # GN prefixes *both* system libraries and full absolute import-library
    # paths with -l. LLD MinGW looks up -lfoo by name, so -lD:\...\foo.lib
    # is never an exact file path. Convert drive-qualified imports into
    # direct input files with forward slashes *before* the BCC64X driver
    # tokenizes backslashes as response-file escapes. Preserve quoting.
    def full_lib_path(match: re.Match[str]) -> str:
        return match.group(1).replace("\\", "/")

    strContent = re.sub(
        r'(?<!\S)"-l([A-Za-z]:[\\/][^"]+)"',
        lambda m: '"' + full_lib_path(m) + '"', strContent)
    strContent = re.sub(
        r'(?<!\S)-l"([A-Za-z]:[\\/][^"]+)"',
        lambda m: '"' + full_lib_path(m) + '"', strContent)
    strContent = re.sub(
        r'(?<!\S)-l([A-Za-z]:[\\/][^\s"]+)',
        full_lib_path, strContent)
    if re.search(r'(?<!\S)-l[A-Za-z]:', strContent):
        raise ValueError("unhandled drive-qualified GN import-library path")

    # For GN system libraries, -ladvapi32.lib causes LLD's -l lookup
    # to append .lib again. Plain -lname.lib becomes -lname; exact -l:
    # names and path-qualified import libraries are not changed.
    strContent = re.sub(
        r'(?<!\S)-l([A-Za-z0-9_+.-]+)\.lib(?=\s|$)',
        r'-l\1', strContent, flags=re.IGNORECASE)
    return re.sub(r'(?<!\S)/(?=[A-Za-z])',
                  "-Xlinker -Xlink=/", strContent)


def RunLinkResponse() -> int:
    if len(sys.argv) < 5:
        print("usage: gn_argument_adapter.py --link-rsp <compiler> <GN-rsp> <compiler-flags>",
              file=sys.stderr)
        return 2
    strCompiler, strInput = sys.argv[2], sys.argv[3]
    theSource = Path(strInput)
    try:
        strOriginal = theSource.read_text(encoding="utf-8")
        strTranslated = TranslateLinkResponse(strOriginal)
        if re.search(r'(?<!\S)/[A-Za-z]', strTranslated):
            raise ValueError("raw MSVC-style linker switch remains after translation")
        theTranslated = Path(str(theSource) + ".bcc64x")
        theTranslated.write_text(strTranslated, encoding="utf-8", newline="\n")
    except (OSError, ValueError) as theError:
        print("BCC64X linker response translation failed: " + str(theError),
              file=sys.stderr)
        return 2
    return subprocess.call([strCompiler, *sys.argv[4:], "@" + str(theTranslated)])


def Main() -> int:
    if len(sys.argv) > 1 and sys.argv[1] == "--link-rsp":
        return RunLinkResponse()
    if len(sys.argv) < 3:
        print("usage: gn_argument_adapter.py <compiler> <arguments...>", file=sys.stderr)
        return 2
    vecArguments = []
    lstErrors = []
    # This LLVM backend tuning option belongs to Chromium's own LLVM
    # revision, not BCC64X. Consume only the exact two-token sequence;
    # every other -mllvm option is still forwarded for compiler validation.
    arguments = iter(sys.argv[2:])
    for strArgument in arguments:
        if strArgument == "-mllvm":
            try:
                strBackendOption = next(arguments)
            except StopIteration:
                lstErrors.append("missing value after -mllvm")
                break
            if strBackendOption == "-pick-merged-source-locations":
                continue
            vecArguments.extend(("-mllvm", strBackendOption))
            continue
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
