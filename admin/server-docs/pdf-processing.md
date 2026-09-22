# PDF and E-Invoice Processing Roadmap

[TOC|Content]

**Status:** active architectural and BuildEngine integration direction as of 22 September 2026. Poppler 26.09.0, libxml2 2.15.3, QPDF 12.4.1 and PoDoFo 1.1.1 are declared in the Schema-16 BuildEngine stack. The three newly added build/test/package paths are **[nicht verifiziert]** until the next BCC64X run.

## License traffic light for this stack

- **🟢 Green — libxml2 2.15.3:** MIT; normal notice-preservation obligations.
- **🟢 Green — QPDF 12.4.1:** Apache-2.0; preserve license/NOTICE obligations and patent terms.
- **🟡 Yellow — PoDoFo 1.1.1:** MPL-2.0 or LGPL-2.0-or-later for the library; the concrete product must record the selected licensing path. GPL command-line tools remain outside the first library-focused package.
- **🟢 Green — win-iconv 0.0.8:** upstream places this implementation in the public domain. This is not GNU libiconv.
- **🔴 Red — Poppler 26.09.0:** GPL; do not plan as an in-process dependency of proprietary software without a different product/distribution architecture or another licensing path.

The traffic light is an engineering integration marker. The exact upstream license texts remain authoritative.

## Objective

The PDF track should grow from a rendering/parsing capability into a reusable document-processing stack that can:

- read normal PDF documents;
- detect and extract embedded electronic-invoice payloads;
- parse and validate selected E-invoice XML formats;
- inspect interactive PDF forms;
- enumerate fields and their metadata;
- fill fields under explicit validation rules;
- write a new PDF result;
- optionally pass the completed document to a direct-print path.

Parsing, semantic invoice interpretation, PDF modification and printing stay separate responsibilities.

## Intended component roles

```mermaid
flowchart LR
    PDF[PDF input] --> Poppler[Poppler: read / inspect / render]
    PDF --> QPDF[QPDF: structural inspection / rewrite]
    PDF --> PoDoFo[PoDoFo: modification / forms / attachments]
    PDF --> Attach[Embedded XML]
    Attach --> XML[LibXML2: XML parser]
    XML --> Invoice[E-invoice semantic model]
    Poppler --> App[Application model]
    QPDF --> App
    PoDoFo --> App
    App --> Output[New PDF]
    Output --> Print[Optional print adapter]
```

### Poppler

Role: PDF reading, page/document inspection, text/rendering-related capabilities and the enabled C++ API.

**Licensing note:** Poppler is GPL-licensed. A shipped proprietary in-process integration requires a deliberate product/legal architecture decision before deployment.

### QPDF 12.4.1

Role: low-level PDF structure, object inspection, transformations, validation/repair-oriented workflows and deterministic rewriting. The current BuildEngine contract builds the shared library against managed zlib, libjpeg-turbo and OpenSSL and retains the upstream test suite. **[nicht verifiziert]**

### PoDoFo 1.1.1

Role: higher-level PDF modification, AcroForm inspection/update, annotations and attachments where appropriate, and writing modified documents. The first BuildEngine profile uses managed zlib, OpenSSL, FreeType, libxml2, JPEG, PNG and TIFF, uses the Win32 GDI font search path, and deliberately disables AFDKO plus the GPL command-line tools. **[nicht verifiziert]**

The exact PoDoFo/QPDF responsibility boundary must be proven with small real programs before both become mandatory runtime dependencies.

### libxml2 2.15.3

Role: low-level XML parsing and schema-related mechanics for extracted invoice XML. The BuildEngine profile keeps XML Schema, Relax NG, XPath, XInclude, serialization and zlib support enabled while Python, ICU, dynamic modules and external iconv are disabled in the first BCC64X profile. Business meaning, invoice rules and profile validation remain a separate typed application layer. **[nicht verifiziert]**

## E-invoice processing

The first PDF-based target is PDF/A-3 with embedded invoice XML, especially ZUGFeRD / Factur-X.

```text
PDF
-> identify embedded/associated files
-> select invoice XML by metadata/profile
-> extract XML bytes
-> parse safely
-> identify invoice version/profile
-> validate syntax/schema/profile rules
-> project into a typed invoice model
```

XML-only invoice formats can reuse the same XML and semantic layer without going through PDF extraction.

The exact supported profiles, schemas and validation artefacts must be pinned as versioned product data before the feature is considered complete.

## PDF form inspection tool

The first dedicated form utility should be diagnostic. Given a PDF, it should enumerate:

- field name and fully qualified name;
- field type;
- current and default value;
- flags such as read-only and required;
- widget/page association;
- widget rectangle;
- choice values where applicable;
- appearance-related information needed to determine whether a filled value will render correctly.

That evidence comes before a generic filler.

The filling path should accept a typed mapping, validate it against the discovered form contract, modify a copy of the input document and reopen the result for verification.

## Printing

Direct printing remains downstream from PDF modification:

```text
inspect -> validate -> fill -> save -> reopen/verify -> print
```

Printer selection and Windows spooler policy do not belong inside the PDF libraries. Printing is an output adapter so the same completed PDF can instead be saved, archived, transmitted or tested.

## BuildEngine work items

1. Verify Poppler `ENABLE_CPP=ON` with a real BCC64X C++ consumer.
2. Run libxml2 2.15.3 through source, Release/Debug build, upstream tests, install, publish and package smoke.
3. Run QPDF 12.4.1 through source, Release/Debug build, upstream tests, install, publish and package smoke.
4. Run PoDoFo 1.1.1 through source, Release/Debug build, upstream tests, install, publish and package smoke.
5. Add small comparison programs that establish the real responsibility boundary between Poppler, QPDF and PoDoFo.
6. Add representative E-invoice and form samples to `BuildEngine-Tests`, not to generic package smokes.
7. Extend the first PoDoFo smoke from construction to AcroForm discovery and a save/reopen round trip after the package itself passes.
8. Extend the QPDF smoke from object construction to structural read/rewrite validation after the package itself passes.
9. Record exact source archive SHA-256 values where upstream publishes stable digests.
10. Decide the product/distribution boundary for GPL Poppler before a proprietary deliverable relies on in-process linkage.
11. Record which PoDoFo license path, MPL-2.0 or LGPL-2.0-or-later, is selected for each distributable product.
