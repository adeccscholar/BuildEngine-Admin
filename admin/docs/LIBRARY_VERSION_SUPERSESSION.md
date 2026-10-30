# Versioned libraries and supersession

Date: 2026-10-15. Implementation on GitHub; native BCC64X test pending.

## Coordinate
A library is identified by **(id, version)**, not by id alone. Duplicate id/version pairs remain invalid; multiple versions of the same id are valid. Dependencies, extensions, FSM states, package paths and work items must resolve the complete coordinate. Unversioned dependency references are rejected as ambiguous when multiple versions are configured.

## XML
The optional `supersededBy` attribute on the **older** `<library>` identifies a newer version of the same library id.

```xml
<library id="openssl" version="3.5.8" supersededBy="3.5.9" ...>
<library id="openssl" version="3.5.9" ...>
```

The attribute is **descriptive**: it must not automatically rewire pinned consumers, delete installed versions, or invalidate old FSM evidence. The catalog validates target existence and rejects cycles.

Initial entries:
- `openssl/3.5.8` superseded by `openssl/3.5.9`.
- `curl/8.21.0` superseded by `curl/8.23.0`.
- `brotli/1.2.0` superseded by `brotli/1.2.1` (security release, 2026-10-21).

## Server dashboard
- Count only non-superseded versions in active version, installed and documented metrics.
- Count logical library IDs once in the "Libraries" metric.
- Keep historical versions available in catalog/list/version detail, with `superseded by …` label.
- Do not equate "superseded" with removed, unlicensed, insecure or deleted.
- Keep explicitly pinned dependent builds on their original versions until independently migrated.

## Acceptance tests required
1. XML-schema validation; two versions of curl/OpenSSL parse without duplicate error.
2. FSM compiles, plans and executes distinct state/paths for both coordinates without cross-contamination.
3. Explicit dependencies resolve only pinned versions; ambiguous unversioned selections fail.
4. Dashboard active counts exclude superseded versions while historic pages remain reachable.
5. Existing single-version libraries and all other packages retain identical behavior.

**No claim of a successful Windows/BCC64X build is made by these commits.**
