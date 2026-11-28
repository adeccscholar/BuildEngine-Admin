# xsimd 13.2.0 — native BCC64X integration and showcase migration

[TOC|Content]

## Identity and intent

**Supplier:** xsimd contributors  
**Source:** https://github.com/xtensor-stack/xsimd.git  
**Pinned upstream ref:** `13.2.0`  
**Product form:** `interface`  
**License signal:** green

Portable SIMD batches and reductions; Tensor Nebula uses actual lane operations rather than reporting SIMD just because the header exists. The XML enables the original tests with managed doctest.

## Managed C++Builder integration contract

- **Compiler:** Embarcadero C++Builder 13 / BCC64X, C++23 when supported by upstream and native consumer.
- **Source archive:** `https://github.com/xtensor-stack/xsimd/archive/refs/tags/13.2.0.tar.gz`.
- **Direct managed dependencies:** `doctest@2.4.12`.
- **Native package gate:** installed headers, license and explicit package smoke, without alternatives from host PATH or Visual Studio.
- **Runtime rule:** all BCC64X executable/DLL products link the dynamic runtime (`-tR`); an `interface` package is not required to produce a shared implementation DLL.

## Direct dependency structure

```mermaid
flowchart LR
  D0["doctest@2.4.12"] --> P["xsimd@13.2.0"]
``

Only direct XML edges are shown; transitive dependency closure remains part of the managed BuildEngine contract.

## Tests and evidence

Declared package-consumer smoke scopes: `consumer/Release/package`, `consumer/Debug/package`.

The operator confirmed the completed 28 November library integration across the developer machine and reduced Windows Server 2025. That is **operator-reported**; this documentation change does not rerun the native BCC64X tests. Existing and future OpenGL/GPU showcases have separate numerical and visual acceptance gates.

[Library catalog](/manual/libraries.md) · [Showcase gallery](/showcases/) · [Engineering retrospective](/news/2026-11-28/)
