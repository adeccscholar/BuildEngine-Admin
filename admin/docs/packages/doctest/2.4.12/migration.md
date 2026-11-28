# doctest 2.4.12 — native BCC64X integration and showcase migration

[TOC|Content]

## Identity and intent

**Supplier:** Viktor Kirilov and contributors  
**Source:** https://github.com/doctest/doctest.git  
**Pinned upstream ref:** `v2.4.12`  
**Product form:** `interface`  
**License signal:** green

Header-only native C++ test infrastructure, used for geometry and xtensor-stack verification. The BCC64X compatibility policy retains original upstream example/test suites and does not globally disable warning checks.

## Managed C++Builder integration contract

- **Compiler:** Embarcadero C++Builder 13 / BCC64X, C++23 when supported by upstream and native consumer.
- **Source archive:** `https://github.com/doctest/doctest/archive/refs/tags/v2.4.12.tar.gz`.
- **Direct managed dependencies:** none.
- **Native package gate:** installed headers, license and explicit package smoke, without alternatives from host PATH or Visual Studio.
- **Runtime rule:** all BCC64X executable/DLL products link the dynamic runtime (`-tR`); an `interface` package is not required to produce a shared implementation DLL.

## Ordered BCC64X source corrections

The XML source contract references the following version-specific patch files, which must remain ordered and reproducible:

- `doctest\2.4.12\bcc64x-test-warning-policy.patch`
- `2.4.12\bcc64x-test-warning-policy.patch`

## Tests and evidence

Declared package-consumer smoke scopes: `consumer/Release/package`.

The operator confirmed the completed 28 November library integration across the developer machine and reduced Windows Server 2025. That is **operator-reported**; this documentation change does not rerun the native BCC64X tests. Existing and future OpenGL/GPU showcases have separate numerical and visual acceptance gates.

[Library catalog](/manual/libraries.md) · [Showcase gallery](/showcases/) · [Engineering retrospective](/news/2026-11-28/)
