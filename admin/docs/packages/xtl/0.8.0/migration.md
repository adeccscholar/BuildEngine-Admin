# xtl 0.8.0 — native BCC64X integration and showcase migration

[TOC|Content]

## Identity and intent

**Supplier:** xtl contributors  
**Source:** https://github.com/xtensor-stack/xtl.git  
**Pinned upstream ref:** `0.8.0`  
**Product form:** `interface`  
**License signal:** green

Optional/value semantics and template support for the xtensor stack. Tensor Nebula uses real optional scientific probes. Version-bound BCC64X source patches address the compiler-specific test/exception headers.

## Managed C++Builder integration contract

- **Compiler:** Embarcadero C++Builder 13 / BCC64X, C++23 when supported by upstream and native consumer.
- **Source archive:** `https://github.com/xtensor-stack/xtl/archive/refs/tags/0.8.0.tar.gz`.
- **Direct managed dependencies:** `doctest@2.4.12`.
- **Native package gate:** installed headers, license and explicit package smoke, without alternatives from host PATH or Visual Studio.
- **Runtime rule:** all BCC64X executable/DLL products link the dynamic runtime (`-tR`); an `interface` package is not required to produce a shared implementation DLL.

## Ordered BCC64X source corrections

The XML source contract references the following version-specific patch files, which must remain ordered and reproducible:

- `xtl\0.8.0\bcc64x-exception-declaration.patch`
- `xtl\0.8.0\bcc64x-clang-gnu-flags.patch`

## Direct dependency structure

```mermaid
flowchart LR
  D0["doctest@2.4.12"] --> P["xtl@0.8.0"]
``

Only direct XML edges are shown; transitive dependency closure remains part of the managed BuildEngine contract.

## Tests and evidence

Declared package-consumer smoke scopes: `consumer/Release/package`, `consumer/Debug/package`.

The operator confirmed the completed 28 November library integration across the developer machine and reduced Windows Server 2025. That is **operator-reported**; this documentation change does not rerun the native BCC64X tests. Existing and future OpenGL/GPU showcases have separate numerical and visual acceptance gates.

[Library catalog](/manual/libraries.md) · [Showcase gallery](/showcases/) · [Engineering retrospective](/news/2026-11-28/)
