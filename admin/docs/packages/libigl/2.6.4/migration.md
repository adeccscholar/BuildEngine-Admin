# libigl 2.6.4 — native BCC64X integration and showcase migration

[TOC|Content]

## Identity and intent

**Supplier:** libigl contributors  
**Source:** https://github.com/libigl/libigl.git  
**Pinned upstream ref:** `v2.6.4`  
**Product form:** `interface`  
**License signal:** yellow

Mesh-processing and vertex-normal algorithms using managed Eigen. Vector Galaxy demonstrates genuine geometric normals. Optional copyleft components require separate license consideration; the MPL core and GPL texts are both retained.

## Managed C++Builder integration contract

- **Compiler:** Embarcadero C++Builder 13 / BCC64X, C++23 when supported by upstream and native consumer.
- **Source archive:** `https://github.com/libigl/libigl/archive/refs/tags/v{LibraryVersion}.tar.gz`.
- **Direct managed dependencies:** `eigen@5.0.1`.
- **Native package gate:** installed headers, license and explicit package smoke, without alternatives from host PATH or Visual Studio.
- **Runtime rule:** all BCC64X executable/DLL products link the dynamic runtime (`-tR`); an `interface` package is not required to produce a shared implementation DLL.

## Direct dependency structure

```mermaid
flowchart LR
  D0["eigen@5.0.1"] --> P["libigl@2.6.4"]
``

Only direct XML edges are shown; transitive dependency closure remains part of the managed BuildEngine contract.

## Tests and evidence

Declared package-consumer smoke scopes: `consumer/Release/package`.

The operator confirmed the completed 28 November library integration across the developer machine and reduced Windows Server 2025. That is **operator-reported**; this documentation change does not rerun the native BCC64X tests. Existing and future OpenGL/GPU showcases have separate numerical and visual acceptance gates.

[Library catalog](/manual/libraries.md) · [Showcase gallery](/showcases/) · [Engineering retrospective](/news/2026-11-28/)
