# Tools – BuildEngine

Diese Seite zeigt die Werkzeugverträge aus `build-tools.xml` und
`build-source-tools.xml`. Die Spalte **Bereitstellung** unterscheidet
**zugeordnet** (vorhandenes SDK/Hostwerkzeug), **bereitgestellt**
(verifiziertes Paket) und **übersetzt** (eigener Source-Tool-FSM-Build).

| Tool | Version / Pin | Bereitstellung |
|---|---|---|
| C++Builder / bcc64x | 20.1.7 | zugeordnet |
| CMake | 4.1.1 | zugeordnet |
| Windows Resource Compiler | Windows SDK | zugeordnet |
| embarcadero-make | 5.43 | zugeordnet |
| llvm-ar / llvm-ranlib / ld-lld | 20.1.7 | zugeordnet |
| bcc64x-windres | 20.1.7 | zugeordnet |
| tdump | RAD Studio 13 | zugeordnet |
| Git / Bash | 2.55.0.3 / 5.2 | bereitgestellt |
| Ninja | 1.13.2 | bereitgestellt |
| Python | 3.14.7 | bereitgestellt |
| Strawberry Perl | 5.42.2.1 | bereitgestellt |
| Meson | 1.12.0 | bereitgestellt |
| Doxygen | 1.18.0 | bereitgestellt |
| Graphviz | 15.1.1 | bereitgestellt |
| NASM | 3.02 | bereitgestellt |
| winflexbison | 2.5.24 | bereitgestellt |
| pkg-config | 0.28-1 | bereitgestellt |
| fonttools | 4.65.0 | bereitgestellt |
| llvm-cov | 20.1.7 | bereitgestellt |
| Mermaid | 11.17.2 | bereitgestellt |
| Highlight.js | 11.12.0 | bereitgestellt |
| MathJax | 3.2.2 | bereitgestellt |
| GN (bisher) | b2afae122eeb6ce09c52d63f67dc53fc517dbdc8 | zugeordnet (Skia; Übergangslösung) |
| GN (geplant) | 07d3c6f4dc290fae5ca6152ebcb37d6815c411ab | übersetzt (noch nicht gebaut) |

Weitere Tool-Einträge werden ausschließlich in ihren XML-Quellen gepflegt.
Diese Markdown-Übersicht bildet derzeit den Konfigurationsstand ab, keinen
Live-Zustand der ToolRegistry. Die endgültige Serverseite muss die drei
Kategorien unmittelbar aus den XML-Verträgen und Runtime-Nachweisen erzeugen,
statt diese Tabelle dauerhaft zu duplizieren.

## Eigenständig übersetzte Werkzeuge

Der vorgesehene Zyklus ist **Source/Patch → Prepare → Release → Smoke →
Publish**. Er besitzt eigene Zustandsnachweise unter
`.buildengine/source-tools`, unabhängig von den Library-FSMs. Siehe
[GN](gn.md).
