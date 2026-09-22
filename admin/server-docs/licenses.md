# Open-Source License Overview

[TOC|Content]

**Status:** general engineering reference as of 22 September 2026.

This page is intentionally **license-centered**. It explains the license types that occur in the BuildEngine third-party stack without turning the page into a library-to-license inventory.

It is an engineering aid, not legal advice. The authoritative terms are always the exact license, copyright and notice files of the distributed upstream version. Mixed projects and bundled components can contain file-level terms that are more specific than the project-level license.

## Reading the risk classification

The risk classification below is about **integration and distribution effort**, not about whether a license is good or bad.

| Risk | Meaning |
| --- | --- |
| Low | Usually compatible with proprietary and open-source products when notices are preserved. |
| Low to medium | Permissive, but attribution, NOTICE, patent, advertising or mixed-component obligations need explicit packaging work. |
| Medium | Weak copyleft, dual licensing or file-level copyleft requires a deliberate integration and redistribution model. |
| High | Strong copyleft can affect the licensing of a distributed combined work and therefore requires an explicit product architecture decision. |

## Public domain / permissive dedication

### Public domain / blessing

**Type:** public-domain dedication or very permissive fallback permission.

**Typical rights:** use, copy, modify, embed, redistribute and commercialize with very few copyright restrictions.

**Typical obligations:** usually minimal. Bundled code and optional extensions still require separate review.

**Combination with proprietary software:** generally straightforward.

**Combination with other open-source licenses:** generally straightforward.

**Integration risk:** **Low**.

**Main risk:** assuming that every file in a project follows the public-domain status of the core. Always inspect bundled and generated components separately.

---

### 0BSD

**Type:** highly permissive BSD-family license.

**Typical rights:** use, copy, modify and redistribute with essentially no attribution condition beyond the license text itself.

**Combination with proprietary software:** straightforward.

**Combination with GPL and other OSS:** generally straightforward.

**Integration risk:** **Low**.

**Main risk:** mixed projects can contain files under additional licenses even when the principal project uses 0BSD.

---

## Permissive licenses

### MIT

**Type:** permissive.

**Typical rights:** commercial and non-commercial use, modification, distribution, sublicensing and sale.

**Typical obligations:** preserve the copyright notice and permission notice in copies or substantial portions.

**Combination with proprietary software:** straightforward; the surrounding application can remain proprietary.

**Combination with other OSS:** broadly compatible, including GPL-family projects.

**Integration risk:** **Low**.

**Main risk:** losing copyright/license notices when source fragments are copied or vendored.

---

### MIT-0

**Type:** permissive, even lighter than ordinary MIT.

**Typical rights:** broad use, modification and redistribution.

**Typical obligations:** very small; no attribution condition comparable to standard MIT.

**Combination:** broadly compatible with proprietary and open-source software.

**Integration risk:** **Low**.

---

### Old MIT / X11-style licenses

**Type:** permissive.

**Typical rights:** broad use, copying, modification and redistribution.

**Typical obligations:** retain specified copyright and permission text.

**Combination:** generally compatible with proprietary software and most OSS licenses.

**Integration risk:** **Low to medium** because older projects often combine this license with file-level exceptions or historical notices.

---

### zlib License

**Type:** permissive.

**Typical rights:** use for any purpose, including commercial applications, modification and redistribution.

**Typical obligations:** do not misrepresent origin, clearly mark altered source versions, preserve the notice in source distributions.

**Combination with proprietary software:** straightforward.

**Combination with other OSS:** broadly compatible.

**Integration risk:** **Low**.

---

### BSD-2-Clause / BSD-3-Clause

**Type:** permissive.

**Typical rights:** source and binary redistribution with or without modification, including commercial use.

**Typical obligations:** preserve copyright/license notices. BSD-3-Clause adds a non-endorsement condition.

**Combination with proprietary software:** straightforward.

**Combination with other OSS:** broadly compatible.

**Integration risk:** **Low**.

**Main risk:** binary redistributions must not silently drop the required notices.

---

### Boost Software License 1.0

**Type:** permissive.

**Typical rights:** use, reproduce, modify, distribute, execute and create derivative works.

**Typical obligations:** retain the license statement in source and derivative source distributions. Pure machine-generated object-code distributions have especially light obligations.

**Combination with proprietary software:** straightforward.

**Combination with other OSS:** broadly compatible.

**Integration risk:** **Low**.

---

### curl License

**Type:** permissive, MIT-like.

**Typical rights:** use, copy, modify and distribute for any purpose.

**Typical obligations:** preserve the copyright and permission notice; do not use copyright-holder names for promotion without permission.

**Combination:** broadly compatible with proprietary and open-source software.

**Integration risk:** **Low**.

---

### bzip2 License

**Type:** permissive BSD-style license.

**Typical rights:** commercial and non-commercial use, source/binary redistribution and modification.

**Typical obligations:** retain notices and observe the naming/endorsement conditions in the upstream license.

**Combination:** generally straightforward.

**Integration risk:** **Low**.

---

### PNG Reference Library / libpng License

**Type:** permissive.

**Typical rights:** use, modification and redistribution, including commercial products.

**Typical obligations:** preserve the required copyright, permission and acknowledgement text.

**Combination:** broadly compatible.

**Integration risk:** **Low to medium** because historical libpng distributions contain version-specific notice language that should be shipped intact.

---

### IJG License

**Type:** permissive, but with specific acknowledgement requirements.

**Typical rights:** commercial and open-source use, modification and redistribution.

**Typical obligations:** preserve the IJG notices; binary/product documentation can require acknowledgement that the software is based in part on the work of the Independent JPEG Group.

**Combination with proprietary software:** permitted.

**Combination with other OSS:** usually possible, but the acknowledgement requirement remains.

**Integration risk:** **Low to medium**.

**Main risk:** treating the license as ordinary BSD and omitting the required product documentation acknowledgement.

---

### DOC License

**Type:** permissive open-source license used by the Distributed Object Computing project.

**Typical rights:** commercial and non-commercial use, modification and redistribution.

**Typical obligations:** preserve applicable copyright and license notices; bundled subcomponents can have independent terms.

**Combination with proprietary software:** normally possible.

**Combination with other OSS:** generally possible subject to bundled-component terms.

**Integration risk:** **Low to medium**.

---

### Unicode License v3

**Type:** permissive.

**Typical rights:** use, copy, modify, merge, publish, distribute and sell software/data.

**Typical obligations:** preserve the Unicode copyright and permission notice either with copies or in associated documentation.

**Combination:** broadly compatible with proprietary and OSS products.

**Integration risk:** **Low to medium**.

**Main risk:** Unicode distributions often carry third-party data/software notices that must remain with the package.

---

## Permissive licenses with explicit patent terms

### Apache License 2.0

**Type:** permissive with explicit patent grant.

**Typical rights:** commercial use, modification, distribution and sublicensing.

**Typical obligations:**

- provide a copy of the license;
- preserve relevant attribution, copyright, patent and trademark notices;
- mark modified files;
- reproduce applicable NOTICE information;
- respect the patent-termination clause.

**Combination with proprietary software:** straightforward when obligations are met.

**Combination with GPLv3:** compatible.

**Combination with GPLv2-only:** not generally considered compatible without an additional permission or another licensing path.

**Integration risk:** **Low to medium**.

**Main risks:** ignoring NOTICE obligations, modifying source without marking changes, or overlooking the patent-termination provision.

---

## Weak copyleft and file-level copyleft

### LGPL 2.0 / 2.1 or later

**Type:** weak copyleft.

**Typical rights:** commercial and proprietary use is possible.

**Typical obligations:** modifications to the LGPL-covered library remain under the LGPL when distributed. The recipient must retain practical rights to replace/relink the LGPL component under the applicable version and distribution model.

**Dynamic linking:** commonly used as a clean operational boundary because the LGPL library remains independently replaceable.

**Static linking:** possible, but generally creates additional relinking/object-file obligations.

**Combination with proprietary software:** possible when the LGPL boundary and redistribution obligations are respected.

**Combination with GPL:** normally possible according to the applicable LGPL/GPL version rules.

**Integration risk:** **Medium**.

**Main risks:** static linking without relinking provisions, embedding the library so it is no longer replaceable, or modifying LGPL code without offering the required corresponding source.

---

### Mozilla Public License 2.0

**Type:** file-level weak copyleft.

**Typical rights:** commercial/proprietary use, modification and redistribution.

**Typical obligations:** files containing MPL-covered code and modifications to those files remain available under MPL when distributed. Unrelated files in the larger program can remain under another license.

**Combination with proprietary software:** generally practical because copyleft is file-scoped.

**Combination with other OSS:** generally good; MPL 2.0 also contains defined GPL/LGPL/AGPL compatibility mechanisms unless a project marks itself incompatible with secondary licenses.

**Integration risk:** **Medium**.

**Main risk:** copying MPL code into proprietary source files instead of keeping the boundary clear.

---

### Common Public License 0.5

**Type:** weak/file-level copyleft-style historical license.

**Typical rights:** use, modification and redistribution.

**Typical obligations:** covered modifications and source availability requirements apply according to the CPL terms.

**Combination with proprietary software:** possible with a clearly separated covered component.

**Combination with GPL-family software:** compatibility is not as straightforward as with modern permissive licenses and requires explicit review.

**Integration risk:** **Medium**.

**Main risk:** selecting the CPL path in a multi-licensed project without considering a simpler LGPL/MPL alternative.

---

## Dual and multi licensing

### Dual or multi licensing

A project can offer the same component under several alternative licenses. This is **not** the same as having to satisfy all offered licenses simultaneously.

The integration must record which licensing path is selected for the concrete product.

Typical examples of choices are:

- permissive license **or** GPL;
- MPL **or** LGPL;
- LGPL **or** GPL;
- CPL **or** LGPL.

**Combination with proprietary software:** often possible when one offered path permits it.

**Integration risk:** **Medium**.

**Main risks:**

1. failing to record which alternative was selected;
2. confusing alternative licenses with cumulative obligations;
3. overlooking individual files that have a different set of alternatives.

---

## Strong copyleft

### GPL 2.0 or later / GPL 3.0

**Type:** strong copyleft.

**Typical rights:** use, study, modification and redistribution.

**Typical distribution obligation:** when a combined derivative work is distributed, the corresponding source and GPL rights can extend to the combined work. The exact boundary depends on the technical and legal relationship between components.

**Internal use:** GPL obligations are principally triggered by distribution rather than private internal execution.

**Dynamic linking:** is **not automatically a safe proprietary boundary**. A normal linked application can still be regarded as one combined work.

**Process separation / IPC:** can create a stronger separation, but it is not an automatic legal exemption. The protocol, coupling and product design matter.

**Combination with proprietary software:** high-risk for in-process distributed integration unless the proprietary component is separately licensed or the architecture establishes a legally sufficient separation.

**Combination with OSS:** requires GPL-compatible licensing for the combined distributed work.

**Integration risk:** **High**.

**Main risks:** assuming that DLL linkage avoids copyleft, shipping a proprietary in-process integration without a licensing strategy, or overlooking GPL-covered tools that are separate from a more permissively licensed library.

---

## Mixed-license distributions

Many mature C and C++ projects are not accurately represented by a single SPDX identifier.

Typical causes:

- bundled third-party code;
- generated parsers or tables;
- imported algorithms;
- test frameworks;
- command-line tools under a different license than the library;
- font or data files with separate terms;
- public-domain files inside an otherwise licensed project.

For such a package BuildEngine should retain:

1. the upstream top-level license;
2. NOTICE/COPYING files;
3. bundled-component license evidence;
4. the exact source version;
5. the selected build profile, because disabled components can materially change the effective license set.

**Integration risk:** **Low to high**, depending on the most restrictive enabled component.

## Integration risk rules for the BuildEngine stack

### 1. Do not derive product licensing from the dependency graph alone

A dependency edge says that software is technically used. It does not by itself answer whether use is internal, dynamically linked, statically linked, embedded, modified, distributed or process-separated.

### 2. Preserve exact license evidence with every versioned package

A normalized SPDX field is useful for automation but must not replace the original upstream license and NOTICE files.

### 3. Record the selected path for multi-licensed code

If a component offers MPL or LGPL, or another set of alternatives, the package metadata should eventually state which path the product uses.

### 4. Treat bundled components as first-class license evidence

A permissive top-level project can still contain components that require attribution, source disclosure or another compatibility check.

### 5. Distinguish libraries from tools

A library can be LGPL/MPL while its command-line tools are GPL. Building or using the library does not automatically mean the tools should be distributed with the product.

### 6. Treat GPL in-process dependencies as a product architecture issue

Where proprietary distribution is intended, a GPL library must be reviewed before it becomes a mandatory in-process runtime dependency. Possible outcomes include an open-source product, a differently licensed component, or a deliberately separated service/process architecture.

### 7. XML/PDF processing is an untrusted-input boundary

License compliance is only one part of integration risk. XML and PDF parsers process attacker-controlled complex data structures. The product architecture should therefore also enforce:

- bounded memory and file sizes;
- disabled network/entity resolution unless explicitly required;
- parser limits where available;
- fuzz/regression inputs;
- CVE monitoring;
- isolation for especially risky transformations;
- deterministic failure instead of silent recovery when integrity matters.

## BuildEngine documentation rule

The license overview remains general and license-centered.

Library-specific pages may state **which** license evidence a concrete package contains, but explanations of rights, obligations, combination rules and generic integration risk belong here and should not be duplicated independently for every library.
