# GN as an Independent BuildEngine Host Tool

GN is a host executable that converts GN project descriptions into Ninja build files. **GN belongs neither to Skia nor to PDFium.** The central intended contract for source-built tools is `admin/build-source-tools.xml`.

## Intended lifecycle

1. **Bootstrap:** `build-tools.xml` provides Python, Ninja, the compiler and archiving tools.
2. **Source tools:** `build-source-tools.xml` defines pinned tools including GN, obtaining them from cache or building them from verified sources if unavailable.
3. **Registration:** After the build, probe and packaging phases, register the tool centrally in ToolRegistry as `{Tool:gn}`.
4. **Libraries:** Only then execute `build-libraries.xml`. Source acquisition must finish before the corresponding build phase.

## GN source pin

- Revision: `07d3c6f4dc290fae5ca6152ebcb37d6815c411ab`
- Upstream: `https://gn.googlesource.com/gn`
- Motivation: Chromium build integration in PDFium 2025.11.19 requires `path_exists()`.
- Intended compiler: **BCC64X**. A successful Windows host build had not been established at the time of this historical contract.
- Microsoft Visual Studio remains a possible **explicit** fallback toolchain, not an invisible replacement for the normal compiler contract.

## Acceptance criteria

Release the tool only after verifying the source hash and commit, passing build and GN tests, passing `gn --version` and a `path_exists()` regression test, recording runtime dependencies and licenses, generating a package manifest and SHA-256 checksum, and proving that a second offline run reuses the same package. Do not search Skia directories to locate GN.

## Documentation server

This file belongs to the **Tools / Source-built Tools** documentation category. The server should display the new folder as a navigable category and render its Markdown documents like other pages. The server wiring was still pending at the time of this plan.

**Historical implementation status:** The XML and XSD described the intended contract. The parser, source-tool FSM, ToolRegistry publication and server menu integration had not yet been implemented at the time of writing. The file must not be interpreted as proof that these features are executable today.
