# Integrating Third-Party Components with C++Builder BCC64X: Lessons Learned from BuildEngine

> **Draft article** — this document preserves the initial article direction and structure. It is intentionally not yet a complete retrospective. The next revision will extend it with the concrete BuildEngine and BCC64X results, failures, fixes, and lessons learned from the project.

## Introduction

Integrating a modern C++ toolchain into an established third-party ecosystem is rarely a matter of changing a compiler switch and rebuilding everything. Libraries have their own build systems, assumptions, platform code, compiler checks, patches, generated sources, tests, installation layouts, and transitive dependencies.

Our work with BuildEngine and Embarcadero C++Builder BCC64X started from a practical question: **how far can the modern BCC64X toolchain participate in the existing C and C++ open-source ecosystem without hiding incompatibilities behind another compiler path?**

The goal was not to produce a collection of isolated one-off builds. We wanted a reproducible process that could describe, build, test, install, inspect, and repeat third-party integrations on another machine.

This article is the story of that work: what we expected, what actually happened, which problems were caused by individual libraries, which belonged to build systems or platform assumptions, and which lessons changed BuildEngine itself.

## 1. Why We Started This Journey

Existing C++Builder applications depend on a substantial third-party ecosystem. Moving to BCC64X therefore raises a question that is larger than compiler syntax compatibility: the complete dependency chain has to work with contemporary source trees and build systems.

BCC64X provides a modern Clang/LLVM-based C++ toolchain and supports current language standards. For our work, **C++23 is the default language level**. That makes it especially interesting to test how well the toolchain fits into projects that were primarily developed and tested with other compilers.

The project therefore deliberately treats compatibility failures as useful results. Replacing a failing BCC64X path silently with MSVC or another compiler would answer a different question and would make the compatibility assessment less meaningful.

## 2. Our BuildEngine Setup

BuildEngine grew into the orchestration layer for the experiment. Instead of hard-coding knowledge about individual libraries into the executable, library and tool knowledge is described declaratively.

The system separates several concerns:

- tool discovery and reproducible tool provisioning;
- source acquisition and verification;
- library-specific build contracts;
- Release and Debug variants;
- dependency ordering and bounded parallel execution;
- patches tied to concrete upstream versions;
- installation and publication;
- upstream tests and BuildEngine smoke tests;
- technical incremental state;
- license, SBOM, security, and documentation metadata.

A central design goal is reproducibility. A successful local build is useful, but a successful build on a clean machine from the same contracts is much stronger evidence.

## 3. The Integration Challenge

Third-party integration exposed several different classes of problems. They should not be mixed together, because they require different solutions and lead to different conclusions about compiler compatibility.

### Compiler and language compatibility

Some projects contain compiler-specific branches, historical Borland checks, assumptions about type layouts, or code that newer compilers diagnose more strictly. A modern compiler can therefore fail in code that was originally intended to support an older compiler from the same vendor family.

### Build-system compatibility

Even when the C or C++ source itself is portable, the surrounding CMake, Meson, Perl, Python, Make, Ninja, code-generation, resource-compilation, or package-discovery logic may not know the BCC64X environment.

### ABI and binary boundaries

Shared libraries, import libraries, runtime dependencies, calling conventions, generated interfaces, and externally supplied binary components introduce a second level of compatibility beyond source compilation.

### Platform assumptions

Windows-specific code paths frequently assume one particular compiler environment. Header selection, SDK detection, resource tools, symbol exports, file handling, and generated configuration tests can all become integration points.

### Dependency chains

A library rarely stands alone. Once a larger component is built, its own dependencies become part of the experiment. This is one reason why BuildEngine models libraries as a dependency graph instead of treating each build as an unrelated script.

## 4. Reproducibility Instead of One-Off Success

A manual command that succeeds once is not yet a build contract.

For this project, a useful integration result must make the important inputs explicit: source version, source identity, tools, compiler, configuration, patches, dependencies, build arguments, output locations, tests, and installed artifacts.

The same distinction applies to incremental builds. The presence of an output file is not sufficient evidence that a technical step is current. BuildEngine therefore separates technical step state from result evidence and validates both for different purposes.

## 5. What We Want to Document Next

The next revision of this article will turn this initial structure into the actual project story. In particular, it should cover:

- the evolution from early manual experiments to declarative BuildEngine contracts;
- the first libraries that compiled cleanly with BCC64X;
- cases where tests exposed issues that a successful link alone would have hidden;
- libarchive and other projects that required source-level compatibility work;
- OpenSSL and the surrounding dependency chain;
- ACE/TAO, including Naming, COS Event, and RT Event services;
- Skia and the much larger graphics dependency graph;
- source pinning, patch provenance, clean-room verification, and technical state;
- build concurrency and scheduler lessons;
- documentation, SBOM, license, and security integration;
- failures in BuildEngine itself that were discovered because the project became large enough to stress the orchestration layer;
- where BCC64X integrates cleanly, where upstream projects make assumptions about supported compilers, and where open questions remain.

## 6. The Question Behind the Project

The most interesting result is not a simple count of libraries that build or fail.

The more useful question is: **when an integration fails, what exactly failed?**

Was it standard C++ source code? A compiler-specific compatibility branch? CMake compiler identification? A Windows assumption? Generated code? A missing tool? An ABI boundary? A test? Or our own orchestration logic?

Keeping those causes separate is what turns a collection of build logs into an engineering assessment.

---

*Draft status: initial article structure recovered and placed under version control. Concrete chronology, measurements, examples, and conclusions will be added in the next revision.*
