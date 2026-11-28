# miniaudio 0.11.26 — native BCC64X integration and showcase migration

[TOC|Content]

## Identity and intent

**Supplier:** David Reid and contributors  
**Source:** https://github.com/mackron/miniaudio.git  
**Pinned upstream ref:** `0.11.26`  
**Product form:** `interface`  
**License signal:** green

Single-header PCM/audio engine, used by Pulse Engine for genuine signal synthesis, RMS measurements and opt-in playback. Compilation occurs inside the consumer, not inside a separately published implementation DLL.

## Managed C++Builder integration contract

- **Compiler:** Embarcadero C++Builder 13 / BCC64X, C++23 when supported by upstream and native consumer.
- **Source archive:** `https://github.com/mackron/miniaudio/archive/refs/tags/{LibraryVersion}.tar.gz`.
- **Direct managed dependencies:** none.
- **Native package gate:** installed headers, license and explicit package smoke, without alternatives from host PATH or Visual Studio.
- **Runtime rule:** all BCC64X executable/DLL products link the dynamic runtime (`-tR`); an `interface` package is not required to produce a shared implementation DLL.

## Tests and evidence

Declared package-consumer smoke scopes: `consumer/Release/package`.

The operator confirmed the completed 28 November library integration across the developer machine and reduced Windows Server 2025. That is **operator-reported**; this documentation change does not rerun the native BCC64X tests. Existing and future OpenGL/GPU showcases have separate numerical and visual acceptance gates.

[Library catalog](/manual/libraries.md) · [Showcase gallery](/showcases/) · [Engineering retrospective](/news/2026-11-28/)
