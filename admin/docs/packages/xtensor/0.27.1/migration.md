# xtensor 0.27.1 — native BCC64X integration and showcase migration

[TOC|Content]

## Identity and intent

**Supplier:** xtensor contributors  
**Source:** https://github.com/xtensor-stack/xtensor.git  
**Pinned upstream ref:** `0.27.1`  
**Product form:** `interface`  
**License signal:** green

Multidimensional numerical arrays and broadcasting, with explicit managed dependencies on xtl, xsimd and nlohmann-json. Tensor Nebula computes its real rank-two wave field in xtensor before uploading an independent OpenGL texture.

## Managed C++Builder integration contract

- **Compiler:** Embarcadero C++Builder 13 / BCC64X, C++23 when supported by upstream and native consumer.
- **Source archive:** `https://github.com/xtensor-stack/xtensor/archive/refs/tags/0.27.1.tar.gz`.
- **Direct managed dependencies:** `doctest@2.4.12`, `nlohmann-json@3.12.0`, `xtl@0.8.0`, `xsimd@13.2.0`.
- **Native package gate:** installed headers, license and explicit package smoke, without alternatives from host PATH or Visual Studio.
- **Runtime rule:** all BCC64X executable/DLL products link the dynamic runtime (`-tR`); an `interface` package is not required to produce a shared implementation DLL.

## Ordered BCC64X source corrections

The XML source contract references the following version-specific patch files, which must remain ordered and reproducible:

- `xtensor\0.27.1\bcc64x-managed-json-tests.patch`
- `0.27.1\bcc64x-managed-json-tests.patch`

## Direct dependency structure

```mermaid
flowchart LR
  D0["doctest@2.4.12"] --> P["xtensor@0.27.1"]
  D1["nlohmann-json@3.12.0"] --> P["xtensor@0.27.1"]
  D2["xtl@0.8.0"] --> P["xtensor@0.27.1"]
  D3["xsimd@13.2.0"] --> P["xtensor@0.27.1"]
``

Only direct XML edges are shown; transitive dependency closure remains part of the managed BuildEngine contract.

## Tests and evidence

Declared package-consumer smoke scopes: `consumer/Release/package`, `consumer/Debug/package`.

The operator confirmed the completed 28 November library integration across the developer machine and reduced Windows Server 2025. That is **operator-reported**; this documentation change does not rerun the native BCC64X tests. Existing and future OpenGL/GPU showcases have separate numerical and visual acceptance gates.

[Library catalog](/manual/libraries.md) · [Showcase gallery](/showcases/) · [Engineering retrospective](/news/2026-11-28/)
