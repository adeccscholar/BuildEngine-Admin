# CGAL 6.2.1: installed CMake package resolution
[TOC|Content]

**Target:** C++Builder 13 / BCC64X / Windows x64.
**Status:** configuration correction declared; native consumer verification **[not verified]**.

## Purpose and dependency

CGAL is consumed through its genuine installed CMake package. The managed Boost 1.92.0 dependency is retained; the small kernel consumer evaluates the squared distance between (0,0) and (3,4) and expects 25.

~~~mermaid
flowchart LR
   C[CGAL 6.2.1] --> B[Boost 1.92.0]
~~~

## Incident and correction

The consumer found the configuration-specific `CGALConfig.cmake`, but that file set `CGAL_FOUND=FALSE`. This is not a missing find_package search path.

CGAL's upstream config requires `include/CGAL/config.h` under its resolved root. The managed package uses `lib/win64/<Configuration>/cmake/CGAL`. Its companion `CGALConfig-installation-dirs.cmake` now resolves the package root relative to the config and verifies that required header. The XML install gate requires `config.h` and the consumer checks config files and headers before running the unmodified upstream CMake package.

## Boost configuration resolution in the consumer

CGAL's installed `CGAL_SetupBoost.cmake` invokes `find_package(Boost 1.74 REQUIRED)`.
With CMake 4.1, providing only `Boost_ROOT` or `BOOST_ROOT` is insufficient to guarantee that the managed Boost 1.92.0 CMake export is selected.
The consumer derives the package version from its declared managed `BOOST_PACKAGE_ROOT`, requires the matching `lib/win64/<configuration>/cmake/Boost-<version>/BoostConfig.cmake`, sets `Boost_DIR`, and resolves the exact managed Boost package before loading CGAL.
No build features or dependency tests are disabled. This is a consumer-only correction and does not invalidate successful CGAL source, build, install, publish or Doxygen scopes.

## Native BCC64X Boost.Config include precedence

Boost 1.92.0's compiler selection excludes `__CODEGEARC__` from the default Clang branch. The existing BuildEngine native-Clang overlay at `admin/cmake/consumer/boost/bcc64x-native-clang/boost/config/user.hpp` selects the upstream Boost Clang compiler configuration without modifying Boost's installed headers.

The CGAL consumer now verifies the overlay, loads the proven `project-include.cmake` helper, and places the overlay directory **first** using `target_include_directories(cgal-consumer BEFORE PRIVATE ...)`, ahead of the managed Boost include root. The source file is `boost/config/user.hpp`; `boost/version.hpp` remains the regular installed Boost version header. This affects only CGAL consumer configuration, not the existing compiled Boost or CGAL packages.

Native consumer build remains **[not verified]** until BCC64X produces a successful smoke.

## Acceptance

Release and Debug must resolve their own installed configuration and pass finite and squared-distance CHECK records plus RESULT. No algorithm or dependency has been disabled. Native BCC64X verification remains **[not verified]**.
