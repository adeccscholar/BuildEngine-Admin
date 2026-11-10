# Skia 153: extern nutzbare Bibliotheken und DLL-Grenzen

[TOC|Content]

**Vertragsanalyse vom 10. November 2026.** Die nachfolgenden Vorschläge ändern Skia nicht. Die neue DLL-Integration wäre jeweils ein eigener versionierter BCC64X-Build mit Consumer-Smoketests. Der aktuelle PDFium-XML-Vertrag referenziert **Skia 153**; die zugehörige DLL-/ABI-Laufzeit ist gesondert über FSM- und Loader-Smokes zu verifizieren.

## Bereits ausgelagerte Bibliotheken

Der aktuelle BuildEngine-Vertrag für Skia 153 referenziert diese externen Komponenten ausdrücklich:

| Bibliothek | Version | GN-Vertrag |
| --- | --- | --- |
| zlib | 1.4 | `skia_use_system_zlib = true` |
| Brotli | 1.2.1 | `skia_use_system_brotli = true` |
| libpng | 1.6.59 | `skia_use_system_libpng = true` |
| libjpeg-turbo | 3.2.1 | `skia_use_system_libjpeg_turbo = true` |
| HarfBuzz | 15.1.0 | `skia_use_system_harfbuzz = true` |
| FreeType | 2.15.0 | `skia_use_system_freetype2 = true` |

Außerdem ist OpenGL 26.2.1 als BuildEngine-Abhängigkeit deklariert. Das Umschalten einer GN-Option beweist noch keine DLL-Importbeziehung; zur Verifikation müssen die PE-Importtabellen und Debug-/Release-Artefakte geprüft werden.

## Noch intern und potenziell wiederverwendbar

| Komponente | Aktueller Zustand | Bewertung |
| --- | --- | --- |
| Expat | `skia_use_expat=true`, `skia_use_system_expat=false` | **Priorität 1:** vorhandenes `expat@2.10.0` wiederverwenden; XML/SVG-Tests erforderlich |
| ICU | `skia_use_icu=true`, `skia_use_system_icu=false` | **Priorität 2:** `icu@79.1` untersuchen; Chromium-Pin, ICU-Daten, ABI und Unicode-Layout zuerst klären |
| libWebP | Decode/Encode aktiv, `skia_use_system_libwebp=false` | **Priorität 3:** sinnvoller unabhängiger Codec; derzeit kein `webp`-Eintrag im aktiven XML-Katalog |
| libavif / libgav1 | Quellcode intern gepinnt | Später: Codec- und AV1-Abhängigkeitskette gesondert aufbauen |
| JPEG XL / highway | Intern gepinnt | Später: komplexe C++- und SIMD-Schnittstellen |
| SPIRV-Cross, glslang, SPIRV-Tools | Bestandteil der Shader-/GPU-Pipeline | Nicht als triviale Drop-in-DLL behandeln |
| Wuffs, skcms | Skia-interne Decoder- und Farbverarbeitung | Vorerst intern belassen |
| DNG SDK, Vulkan Memory Allocator | Eingebundene Hilfskomponenten | Nur mit konkreten anderen Konsumenten und stabiler DLL-API auslagern |

## Vorgehensweise für einen Austausch

Zunächst muss der bestehende Skia-153-Consumer in Release und Debug nachweisbar funktionieren. Danach jeweils **eine** Bibliothek auslagern, beginnend mit Expat, und die GN-Anbindung mit der tatsächlich installierten DLL und Importbibliothek prüfen. Die BuildEngine-DAG erhält erst nach verifiziertem Build einen zusätzlichen versionsgebundenen Dependency-Eintrag. Die Projekt-SBOM muss zwischen ausgegliederten und weiterhin gebündelten Komponenten unterscheiden.

**Wichtig:** Eine eingebundene Quellbibliothek ist nicht automatisch eine separat austauschbare DLL. Insbesondere ICU-Daten, private Symbolversionen und Shader-Tools können die Austauschbarkeit einschränken. Für die vorgeschlagenen Umstellungen liegen bisher keine gesonderten BCC64X-PASS-Nachweise vor: **[nicht verifiziert]**.

[Skia 153](/package/skia/153/migration) · [News](/news/) · [Bibliotheken](/manual/libraries.md)
