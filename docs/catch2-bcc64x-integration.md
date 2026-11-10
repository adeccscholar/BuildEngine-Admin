# Catch2 3.16.0 with BCC64X 20.1.7

Status date: 8 September 2026  
Toolchain: RAD Studio 13 Florence / C++Builder 13 / BCC64X 20.1.7 / Embarcadero C++ 7.80 / Win64 Modern

## Status

Catch2 3.16.0 successfully built with BCC64X in both Release and Debug. The complete upstream CTest suites passed in both variants for this documented run.

Verified BuildEngine run:

```text
library:catch2:build:Debug    PASS, 170 build steps, warnings=0, errors=0
library:catch2:build:Release  PASS, 170 build steps, warnings=0, errors=0
library:catch2:test:Debug     PASS, 82/82 upstream CTest cases
library:catch2:test:Release   PASS, 82/82 upstream CTest cases
library:catch2:install:Debug  PASS
library:catch2:install:Release PASS
library:catch2:install:common PASS
library:catch2:install        PASS
library:catch2:metadata       PASS, CycloneDX 1.7, components=1
library:catch2:publish        PASS, 188 files
```

The only remaining failure in that recorded run concerned the two package-consumer smokes at `find_package(Catch2 CONFIG)`.

## BCC64X adaptation 1: Catch2 `__BORLANDC__` detection

When `__BORLANDC__` is defined, Catch2 3.16.0 enables the legacy `CATCH_CONFIG_POLYFILL_ISNAN` polyfill. It uses `std::_isnan()`, which is not present in the current BCC64X 20.1.7 standard-library environment.

Catch2 itself provides this negative override:

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
```

This disables the incompatible legacy polyfill without modifying Catch2 source code.

## BCC64X adaptation 2: execution code page

Catch2 SelfTests include real UTF-8 characters such as `š` and `👾` in ordinary narrow string literals and check their UTF-8 byte sequences.

Minimal native tests using the actually installed BCC64X 20.1.7 produced the following results for `š`:

```text
Default                                             -> 9A
-finput-charset=UTF-8                              -> 9A
-finput-charset=UTF-8 -fexec-charset=UTF-8         -> 9A
UTF-8 source with BOM                           -> 9A
-fborland-input-code-page=65001                    -> 9A
-fborland-exec-code-page=65001                     -> C5 A1
-fborland-input-code-page=65001
  -fborland-exec-code-page=65001                   -> C5 A1
```

BCC64X explicitly documents the following Embarcadero-specific options in `--help`:

```text
-fborland-input-code-page=<code page>
-fborland-exec-code-page=<code page>
```

Only the execution code page is relevant for Catch2. Therefore the contract uses:

```text
-fborland-exec-code-page=65001
```

Mit

```text
-DCATCH_CONFIG_NO_POLYFILL_ISNAN
-fborland-exec-code-page=65001
```

the previously failing Catch2 `RunTests` self-test passed 1/1 in an isolated check. The subsequent BuildEngine run passed all 82 CTest cases in both Release and Debug.

## Package smoke: CMake config is installed per variant

Upstream Catch2 defines:

```cmake
set(CATCH_CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Catch2")
```

The BuildEngine contract deliberately uses variant-specific library directories:

```text
Release: lib/win64/Release
Debug:   lib/win64/Debug
```

Consequently, the unchanged Catch2 CMake packages are installed at:

```text
lib/win64/Release/cmake/Catch2
lib/win64/Debug/cmake/Catch2
```

BuildEngine's `scope="package"` smoke sets `CMAKE_PREFIX_PATH` to the package root and `CMAKE_LIBRARY_PATH` to `lib/win64/<Configuration>`. However, the configuration-specific directory `lib/win64/<Configuration>/cmake/Catch2` is not a standard `find_package(... CONFIG)` lookup location below that prefix.

Therefore both consumer smokes failed at `find_package(Catch2 3.16 CONFIG REQUIRED)`, even though the build, tests, installation and publishing had passed.

### Selected correction

The package smoke explicitly receives the real, unmodified Catch2 config path:

```text
-DCatch2_DIR:PATH={ConsumerRoot}\lib\win64\{Configuration}\cmake\Catch2
```

This is preferable to relocating generated Catch2 export files afterward: CMake export files contain relocation-related import-prefix logic and should remain in their original installation layout.

The correction affects only the Catch2 package smoke. Neither Catch2 source code nor the BuildEngine core needs changing.

## Architecture decision

Catch2 remains static. For a testing framework, this is an explicitly accepted exception to the usual shared-library default.

Both BCC64X compiler settings remain local to Catch2. In particular, `-fborland-exec-code-page=65001` is not enabled globally for all libraries without a separate toolchain policy decision.

## Source-Pin

Version: `3.16.0`  
Tag: `v3.16.0`  
Commit: `317ac1ed4c0bb6e6b91eafc817e05c488feffcb3`

No Catch2 source patches are required.
