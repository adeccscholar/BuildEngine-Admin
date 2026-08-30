# Boost 1.92.0 BCC64X contract

This directory contains project-local integration for the target-machine-verified Boost 1.92.0 C++23 model.

- `bcc64x-native-clang/` routes only Clang-based CodeGear/BCC64X builds to upstream `boost/config/compiler/clang.hpp`. It does not modify Boost source.
- `preflight/` contains the three compile-only gates for native `noexcept`, Clang feature reporting, and Boost.Config classification.
- `BuildEngineBoostProject.cmake` supplies the exact managed OpenSSL input and the Windows system-library requirements used by the verified Boost.Cobalt graph.
- `components.json` is the machine-readable seven-gate / 148-module evidence map.

Production invariant: one official source tree, one Release graph, one Debug graph. Component separation is a consumer/evidence concern; separately configured stage binaries must never be merged into the production package.
