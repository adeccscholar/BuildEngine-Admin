# Third-Party License Overview

[TOC|Content]

**Status:** maintained overview for the libraries declared in `admin/build-libraries.xml` as of 22 September 2026.

This is an engineering overview, not legal advice. The authoritative terms are the license and notice files shipped by the exact upstream version and retained in the BuildEngine package. For mixed projects and bundled components, file-level notices override this summary.

## License families

**Permissive licenses** such as MIT, BSD, zlib, BSL-1.0 and similar licenses normally permit proprietary and open-source use, modification and redistribution. Typical obligations are preservation of copyright/license notices, sometimes attribution, non-endorsement clauses, or marking changed source.

**Apache-2.0** is permissive and adds an explicit patent grant, patent-termination terms, and NOTICE/modification obligations. It combines well with proprietary software and GPLv3 code. GPLv2-only combinations require separate compatibility review.

**Weak copyleft / file-level copyleft** such as LGPL or MPL can normally be used alongside proprietary code, but the covered component or covered files remain under their license when distributed. LGPL distributions may also require practical relinking/replacement rights.

**Strong copyleft** such as GPL requires a product-level distribution decision. Distribution of a combined derivative work can require the whole combined work to be distributed under GPL-compatible terms. Linking and deployment boundaries therefore matter.

## Current library inventory

| Library | Version | License / family | Classification | Use and combination |
| --- | --- | --- | --- | --- |
| pugiXML | 1.16 | MIT | Permissive | Proprietary and OSS use allowed; preserve notice. Broadly combinable. |
| zlib | 1.3.2 | zlib License | Permissive | Commercial use allowed; do not misrepresent origin, mark altered source, preserve notice. |
| Brotli | 1.2.0 | MIT | Permissive | Proprietary and OSS use allowed with notice preservation. |
| Zstandard | 1.5.7 | BSD-3-Clause | Permissive | Source/binary redistribution allowed with notice and non-endorsement conditions. |
| XZ Utils | 5.8.3 | primarily 0BSD with file-level exceptions | Mixed / permissive-dominant | Preserve `COPYING`, `COPYING.0BSD`, `COPYING.GPLv2`; check file-level terms for redistributed subsets. |
| libzip | 1.11.4 | BSD-style | Permissive | Suitable for proprietary/OSS use; preserve upstream license text. |
| libarchive | 3.8.9 | BSD-style overall plus file-level exceptions | Mixed / permissive-dominant | Core use is permissive; public-domain and alternative-license files exist. Preserve complete upstream notices for redistributed source/build material. |
| OpenSSL | 3.5.8 | Apache-2.0 | Permissive + patent grant | Proprietary/OSS use allowed subject to Apache obligations. GPLv3 compatible; GPLv2-only needs review. |
| curl | 8.21.0 | curl license, MIT-like | Permissive | Use, modification and redistribution allowed with copyright/permission notice preservation. |
| Boost | 1.92.0 | BSL-1.0 | Permissive | Broadly usable. Source/derivative source retains the BSL text; object-code-only distribution has light obligations. |
| nlohmann/json | 3.12.0 | MIT | Permissive | Proprietary and OSS use allowed with notice preservation. |
| ACE | 8.0.6 | DOC License | Permissive | Commercial/proprietary use is intended to be possible; preserve upstream license evidence and review bundled subcomponents. |
| TAO | 4.0.6 | DOC License | Permissive | Same general DOC licensing model as ACE; bundled/generated tools can carry their own notices. |
| bzip2 | 1.0.8 | bzip2 License | Permissive | Commercial and OSS use allowed with notice/attribution requirements and naming restrictions. |
| GLEW | 2.3.1 | BSD/MIT/SGI-style notices | Mixed / permissive | Preserve the complete upstream `LICENSE.txt`; do not collapse the package to one license identifier. |
| OpenGL / Mesa | 26.2.1 | mixed permissive component licenses | Mixed | Mesa contains several permissive licenses. Preserve the license set for the actually enabled components. |
| raylib | 6.0 | zlib License | Permissive | Proprietary/OSS use allowed; preserve notice and mark altered source as required. |
| SDL2 | 2.32.10 | zlib License | Permissive | Proprietary/OSS use allowed; broad compatibility. |
| SQLite | 3.53.4 | public-domain dedication / blessing | Public domain | Very few copyright restrictions for core SQLite; optional/bundled material still needs separate review. |
| Xerces-C | 3.3.0 | Apache-2.0 | Permissive + patent grant | Proprietary/OSS use allowed subject to Apache/NOTICE obligations. |
| SOIL2 | 1.3.0 | MIT-0 | Permissive | Commercial/proprietary and OSS use with minimal conditions. |
| VTK | 9.6.2 | BSD-3-Clause | Permissive | Proprietary/OSS use allowed with notice preservation and non-endorsement clause. |
| OpenCV | 5.0.0 | Apache-2.0 plus bundled notices | Mixed / permissive-dominant | Main project is Apache-2.0; preserve bundled third-party licenses for enabled components. |
| cmark-gfm | 0.29.0.gfm.13 | BSD-style plus bundled notices | Mixed / permissive | Suitable for proprietary/OSS use; retain the complete COPYING/notices. |
| GoogleTest | 1.17.0 | BSD-3-Clause | Permissive | Can be used with proprietary and OSS test code; preserve notice when redistributed. |
| Catch2 | 3.16.0 | BSL-1.0 | Permissive | Broadly combinable; object-code-only distribution is especially simple. |
| BitmapPlusPlus | 1.1.1 | MIT | Permissive | Proprietary/OSS use allowed with notice preservation. |
| libjpeg-turbo | 3.2.0 | IJG + BSD-3-Clause, with component notices | Mixed / permissive | Commercial use is possible. Binary/product documentation can require IJG attribution; preserve the complete upstream license set. |
| libpng | 1.6.58 | PNG Reference Library License v2 / libpng-2.0 | Permissive | Commercial/proprietary use allowed; preserve required notices/acknowledgements. |
| HarfBuzz | 14.4.0 | Old MIT with file-level exceptions | Mixed / permissive-dominant | Broadly usable; retain upstream `COPYING` and subdirectory notices for included files. |
| FreeType | 2.14.3 | FreeType License OR GPL-2.0-or-later | Dual-license | Proprietary products commonly choose the FreeType License, including its attribution requirement; GPL is the alternative for GPL-compatible distributions. |
| libtiff | 4.7.2 | libtiff BSD-style license | Permissive | Proprietary/OSS use allowed with copyright/license preservation. |
| Skia | 153 | BSD-3-Clause main project plus third-party licenses | Mixed / permissive-dominant | Skia itself is permissive; its dependency/source tree carries many independent notices. |
| Graphite2 | 1.3.15 | LGPL-2.1-or-later OR MPL/GPL alternatives | Weak copyleft / multi-license | Proprietary use is possible under an appropriate offered license. Record the selected license and satisfy relinking/file-level obligations where applicable. |
| Expat | 2.8.4 | MIT | Permissive | Proprietary/OSS use allowed with notice preservation. |
| TECkit | 2.5.13 | CPL-0.5-or-later OR LGPL-2.1-or-later; some files have LGPL/GPL/MPL alternatives | Mixed / weak copyleft | License choice and file-level exceptions matter. Preserve all upstream evidence and record the selected licensing path. |
| ICU | 78.3 | Unicode-3.0 plus third-party notices | Permissive / mixed notices | Commercial/proprietary and OSS use allowed; preserve the complete ICU license because it contains third-party data/software notices. |
| Poppler | 26.09.0 | GPL-2.0-or-later / GPL-3.0 terms in upstream distribution | Strong copyleft | Internal use and redistribution are different cases. Distribution of a proprietary application linked in-process with Poppler requires deliberate GPL compatibility analysis. Keep `COPYING` and `COPYING3`. |

## Combination rules used by BuildEngine documentation

A permissive library normally does **not** force the surrounding program to adopt the same license. The surrounding code may remain proprietary or use another OSS license, while the third-party notices and specific attribution/patent obligations stay intact.

For weak-copyleft and multi-license libraries, BuildEngine should record which offered license is actually selected for the product instead of treating alternative licenses as cumulative. Dynamic-library boundaries are often operationally useful for LGPL compliance, but the exact obligations remain license-specific.

For GPL libraries, the technical dependency graph alone is not a licensing decision. Linking, modification, process boundaries, plugins, IPC and the actual distribution model matter. Poppler is therefore explicitly called out in the PDF roadmap.

## BuildEngine practice

BuildEngine keeps four layers separate:

1. exact upstream license and notice files in the versioned package;
2. normalized machine-readable license identifiers where unambiguous;
3. this human-readable engineering overview;
4. product-specific legal/compliance decisions for a concrete distribution.

The overview must never replace the upstream license evidence, and a single SPDX identifier must not erase mixed or bundled-component licensing.
