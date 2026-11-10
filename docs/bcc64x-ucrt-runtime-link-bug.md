# BCC64X 20.1.7: Incorrect UCRT Symbol Resolution on Win64

Status: **reproduced and confirmed by control experiment**  
Toolchain: RAD Studio 13 Florence / BCC64X 20.1.7 / Win64 Modern / LLD 20.1.7

## Summary

When BCC64X takes the address of certain C runtime math functions, it emits a PE import from `api-ms-win-crt-math-l1-1-0.dll`. The DLL links successfully but fails to load on Windows (`ERROR_PROC_NOT_FOUND`, error 127).

The `x86_64-w64-mingw32/lib/libucrt.a` shipped with RAD Studio also contains local `libucrt_extra` implementations of those symbols. Linking these objects explicitly ahead of the normal runtime removes the problematic API-set imports and allows `LoadLibrary` to succeed.

The defect was initially exposed by a Skia Debug build, but is **not specific to Skia**.

## Affected symbols

In the inspected `libucrt.a`, each of these symbols has both an API-set definition and a local `libucrt_extra` implementation:

- `fabsf`
- `nextafterl`
- `nexttoward`
- `nexttowardf`
- `nexttowardl`

The corresponding local implementations reside in four object files:

- `lib64_libucrt_extra_a-fabsf.o`
- `lib64_libucrt_extra_a-nextafterl.o` (also provides `nexttowardl`)
- `lib64_libucrt_extra_a-nexttoward.o`
- `lib64_libucrt_extra_a-nexttowardf.o`

## Minimal reproducer

Source: `admin/programs/bcc64x/ucrt-runtime-probe.c`

Build without workaround:

```cmd
"C:\Program Files (x86)\Embarcadero\Studio\37.0\bin64\bcc64x.exe" -tD ucrt-runtime-probe.c -o ucrt-runtime-probe.dll
```

Import table:

```cmd
tdump -em ucrt-runtime-probe.dll | findstr /i "fabsf nextafterl nexttoward"
```

Observed result:

```text
IMPORT: api-ms-win-crt-math-l1-1-0.dll='fabsf'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nextafterl'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nexttoward'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nexttowardf'
IMPORT: api-ms-win-crt-math-l1-1-0.dll='nexttowardl'
```

`LoadLibrary` then fails with error 127 (`The specified procedure could not be found`).

## Evidence from `libucrt.a`

For `fabsf`, `llvm-nm --print-armap` reports both:

```text
__imp_fabsf in api-ms-win-crt-math-l1-1-0.dll
fabsf in api-ms-win-crt-math-l1-1-0.dll
...
lib64_libucrt_extra_a-fabsf.o:
00000000 T fabsf
```

The ordinary link map includes other `libucrt_extra` objects but not `lib64_libucrt_extra_a-fabsf.o`. The resulting PE imports the API-set symbol instead.

The exact internal LLD selection rule (for example, the first matching archive-map entry) is **[not verified]**. What has been verified is the provider selected in the final PE and the absence of extraction of the local implementation during the normal link.

## Positive control experiment

The four objects were extracted from the same locally installed `libucrt.a` and combined into `libbcc64x-ucrt-compat.a`. The same reproducer was then linked with this archive ahead of the normal runtime.

Result:

- none of the five problematic API-set imports remains;
- the DLL links successfully;
- `LoadLibrary` reports `LOAD OK`.

This reproduces the underlying problem below the third-party library layer.

## BuildEngine workaround

The workaround is neither a Skia source patch nor a PowerShell helper script. All provisioning logic belongs to the BuildEngine tool contract.

`admin/build-tools.xml` declares a generated tool called `bcc64x-ucrt-compat`. Its contract includes:

- the locally installed BCC64X `llvm-ar` as the archiver,
- the locally installed `x86_64-w64-mingw32/lib/libucrt.a` as the source archive,
- exactly the four verified `libucrt_extra` members,
- the versioned destination directory under the BuildEngine tools root,
- `libbcc64x-ucrt-compat.a` as the registered tool entry.

BuildEngine generates the archive itself. If the versioned tool entry already exists and is not older than the source `libucrt.a`, generation is skipped. Otherwise BuildEngine extracts the declared members using `llvm-ar`, verifies the extracted files, creates the compatibility archive in a temporary directory and promotes it to the final tool path only after success.

The archive is provisioned once per tool version and production environment, then reused. No Embarcadero object files or archives are stored in the repository.

The generic CMake/BCC64X contract consumes the provisioned tool and links its archive ahead of normal runtime libraries. CMake itself no longer generates a compatibility archive.

The workaround applies to Win64 BCC64X linking generally, not only Debug. Debug merely exposed the defect because optimization frequently eliminates the external reference to `fabsf`.

## Proposed Embarcadero bug report (English)

**Title:** BCC64X 20.1.7 libucrt.a resolves x64 math compatibility symbols to non-loadable API-set imports

**Description:**

On RAD Studio 13 / BCC64X 20.1.7, a Win64 DLL that takes the address of `fabsf`, `nextafterl`, `nexttoward`, `nexttowardf`, or `nexttowardl` links successfully but fails to load with Windows error 127 (`ERROR_PROC_NOT_FOUND`).

The generated PE imports these symbols from `api-ms-win-crt-math-l1-1-0.dll`.

The shipped `x86_64-w64-mingw32/lib/libucrt.a` contains both API-set definitions for these symbols and local `libucrt_extra` implementations. For example, `llvm-nm --print-armap libucrt.a` reports both an API-set `fabsf` definition and `lib64_libucrt_extra_a-fabsf.o` with a real `T fabsf` definition.

During the normal BCC64X DLL link, the local compatibility object is not extracted and the resulting DLL contains the API-set import. If the already shipped `lib64_libucrt_extra_a-fabsf.o` is linked explicitly, the API-set import disappears and the DLL loads successfully. The same result is obtained for the complete five-symbol set by linking the four corresponding `libucrt_extra` objects.

This reproduces independently of Skia or any third-party library.

**Expected:** the normal BCC64X runtime link selects the supplied x64 `libucrt_extra` implementations where required, producing a loadable Win64 binary.

**Actual:** LLD selects/emits the API-set imports and the resulting DLL fails at runtime with `ERROR_PROC_NOT_FOUND`.


## Separate dynamic-runtime invariant

The UCRT compatibility archive above solves a specific BCC64X/LLD symbol-selection defect. It does **not** replace the general BCC64X runtime-linkage rule discovered later while validating PoDoFo.

BCC64X's Borland-style target modes default to static runtime linkage unless `-tR` is present. Separate static C++ runtimes in an executable and a DLL are unsafe when C++ ownership, STL objects, exceptions, or allocator activity cross the module boundary.

BuildEngine therefore enforces:

```text
console EXE : -tC -tR
GUI EXE     : -tW -tR
DLL         : -tD -tR
MODULE      : -tD -tR
```

Static BCC64X runtime linkage is forbidden project-wide. CMake configuration fails if `-tR` disappears from one of these link rules.

These two runtime controls address different issues:

- `libbcc64x-ucrt-compat.a` selects working implementations for the verified UCRT math-symbol defect;
- `-tR` ensures one shared BCC64X C++ runtime/allocator domain across Windows module boundaries.
