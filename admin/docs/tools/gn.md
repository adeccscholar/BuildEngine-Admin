# GN als eigenständiges BuildEngine-Hosttool

GN wird als Host-Programm benötigt, um GN-Projekte in Ninja-Builddateien zu überführen. **GN gehört weder zu Skia noch zu PDFium.** Der zentrale Vertrag für selbst gebaute Werkzeuge ist `admin/build-source-tools.xml`.

## Vorgesehener Lebenszyklus

1. **Bootstrap:** `build-tools.xml` stellt Python, Ninja, Compiler und Archivwerkzeuge bereit.
2. **Source Tools:** `build-source-tools.xml` beschreibt gepinnte Werkzeuge einschließlich GN. Aus dem Cache laden oder bei Fehlen aus verifizierten Quellen bauen.
3. **Registrierung:** Nach Build, Probe und Paketierung das Werkzeug im zentralen ToolRegistry unter `{Tool:gn}` freigeben.
4. **Libraries:** Erst anschließend `build-libraries.xml` ausführen. Alle Quellzugriffe enden vor der jeweiligen Build-Phase.

## GN-Pin

- Revision: `07d3c6f4dc290fae5ca6152ebcb37d6815c411ab`
- Ursprung: `https://gn.googlesource.com/gn`
- Anlass: Chromium-Build in PDFium 2025.11.19 benötigt `path_exists()`.
- Gewünschter Compiler: **BCC64X**; ein erfolgreicher Windows-Hostbuild ist noch nicht nachgewiesen.
- Microsoft Visual Studio bleibt eine mögliche explizite Fallback-Toolchain, nicht eine heimliche Änderung des normalen Compilervertrags.

## Abnahme

Das Tool wird erst freigegeben, wenn: Source-Hash/Commit geprüft; Build und GN-Tests bestanden; `gn --version` und `path_exists()`-Regressionstest bestanden; Laufzeitabhängigkeiten und Lizenz erfasst; Paketmanifest und SHA-256 berechnet; ein zweiter Offline-Lauf das gleiche Paket wiederverwendet. Es darf keine Skia-Verzeichnisse nach GN durchsuchen.

## Dokumentationsserver

Diese Datei gehört im Dokumentationsbaum in die Kategorie **Tools / Source-built Tools**. Der Dokumentationsserver soll den neuen Ordner als navigierbare Kategorie darstellen und die Dokumente wie alle anderen Markdown-Dateien rendern. Die Verkabelung im Server ist noch zu implementieren.

**Implementierungsstand:** XML und XSD beschreiben den Zielvertrag. Der Parser, die Source-Tool-FSM, die ToolRegistry-Publikation sowie der Server-Menüeintrag sind noch nicht implementiert; die Datei darf nicht als bereits ausführbar verstanden werden.
