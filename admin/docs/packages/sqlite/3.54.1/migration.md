## Aktualisierter BuildEngine-Vertrag (10. November 2026)

**Deklarierte Konfiguration, nicht neuer Test-PASS.**

### Vollständige transitive Abhängigkeiten

```mermaid
flowchart LR
   N0["sqlite 3.54.1"]
```

Direkt: keine

### Aktuelle Konfiguration

- `-DSQLITE_SOURCE_DIR:PATH={Workspace}`
- `-DBCC64X_EXECUTABLE:FILEPATH={Tool:bcc64x}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib\win64\{Configuration}`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin\win64\{Configuration}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`

---

## Historische Integrationsnotizen

# SQLite 3.54.1

[TOC|Content]

## Aktueller BuildEngine-Vertrag (10. November 2026)

**Vertragsdaten, keine bestätigte Build-Ausführung.** Ergebnisse aus früheren Tests sind versions- und buildgebunden; neue Ausführungen bleiben **[nicht verifiziert]**.

Embedded relational database engine providing a self-contained SQL database in a single library.

**Koordinate:** `sqlite@3.54.1` · **Kategorie:** `database` · **Lizenzampel:** `green`

### Deklarierte Abhängigkeiten (vollständig transitiv)

Die Diagrammkanten stammen aus den exakt gepinnten XML-Abhängigkeiten. Gestrichelte Kanten kennzeichnen Erweiterungen, durchgezogene Kanten Paketabhängigkeiten. Upstream-intern gebündelte Komponenten sind keine zusätzlichen BuildEngine-Koordinaten.

```mermaid
flowchart LR
   N0["sqlite 3.54.1"]
```

**Direkte Abhängigkeiten:** Keine deklariert.

### Konfiguration und Besonderheiten

**CMake-Argumente:**

- `-DSQLITE_SOURCE_DIR:PATH={Workspace}`
- `-DBCC64X_EXECUTABLE:FILEPATH={Tool:bcc64x}`
- `-DCMAKE_INSTALL_PREFIX:PATH={BuildRoot}\stage\packages\{LibraryId}\{LibraryVersion}\{Configuration}`
- `-DCMAKE_INSTALL_LIBDIR:STRING=lib\win64\{Configuration}`
- `-DCMAKE_INSTALL_BINDIR:STRING=bin\win64\{Configuration}`
- `-DCMAKE_BUILD_TYPE:STRING=Release`
- `-DCMAKE_BUILD_TYPE:STRING=Debug`

**Source-Downloads:**

- `https://www.sqlite.org/2026/sqlite-amalgamation-3540100.zip` (SHA-256: `0a9c8e34352dad46006aa82836d5383bff9dc6bc9ce5a60553c05aa30e6ad1f9`)

**Deklarierte Smoke-Tests:**

- `consumer` / `Release` / `published` / `smokes\sqlite\consumer`

**Explizit geforderte Installationsartefakte:**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\bin\win64\{Configuration}\{BaseName}.dll`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\lib\win64\{Configuration}\{BaseName}.lib`

**Verbindliche Quellen:** `admin/build-libraries.xml` für Konfiguration und Pins; FSM/Buildlogs für tatsächliche Ausführung. [Bibliotheksverzeichnis](/manual/libraries.md).
