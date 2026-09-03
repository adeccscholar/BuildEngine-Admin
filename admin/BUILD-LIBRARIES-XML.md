# `build-libraries.xml` – aktueller Vertragsleitfaden

**Stand:** 3. September 2026  
**Schema:** 13  
**Primärsprache:** Deutsch

Dieses Dokument beschreibt die aktuelle Semantik von `admin/build-libraries.xml`. Die XSD-Datei `admin/schemas/build-libraries.xsd` ist die formale Strukturdefinition; dieses Dokument erklärt die fachliche Bedeutung.

## 1. Grundidee

`build-libraries.xml` ist der einzige normative Bibliotheks- und Dependency-Vertrag des Projekts.

```xml
<buildLibraries schemaVersion="13">
   <library ...>
      ...
   </library>
</buildLibraries>
```

Jede Bibliothek beschreibt deklarativ:

- ID und Version,
- Änderungszeitpunkt des vollständigen Vertrags,
- Dependencies,
- Metadaten/Lizenzen,
- Upstream-Quelle,
- Build/Varianten,
- Install/Paketierung,
- Publish,
- kleine Package-Smokes.

## 2. Library-Knoten

Beispiel:

```xml
<library id="example" version="1.2.3" timestamp="2026-09-03T12:00:00Z">
   ...
</library>
```

`timestamp` wird erhöht, wenn sich der wirksame Library-Vertrag ändert, also beispielsweise Quelle, Buildoptionen, Patch, Installlayout, Require-Gates, Publish oder Smoke.

## 3. Dependencies

```xml
<dependency library="zlib" version="1.3.2"/>
```

Dependencies werden nur modelliert, wenn eine echte technische Abhängigkeit existiert. Sie dienen nicht dazu, Scheduler-Reihenfolgen künstlich zu erzwingen.

BuildEngine stellt abhängigen Producer-Builds die verwalteten Package-Pfade über Environment/CMake-Suchpfade bereit.

## 4. Metadaten und Lizenz

Bibliotheken können Upstream- und Lizenzinformationen deklarieren. Diese Daten fließen in Paketmetadaten, `LICENSE-INFO.txt` und CycloneDX-SBOM-Evidence ein.

## 5. Source

Ein Source-Vertrag besteht aus generischen Aktionen, typischerweise:

```xml
<source>
   <download .../>
   <extract ...>
      <require path="CMakeLists.txt"/>
      <require path="include\library.h"/>
   </extract>
</source>
```

### Download

```xml
<download url="..." archive="..." sha256="..."/>
```

`sha256` wird verwendet, wenn ein stabiler Upstream-Hash verfügbar ist bzw. der Vertrag eine feste Archividentität verlangt.

### Extract

```xml
<extract format="zip" root="library-{LibraryVersion}">
   <require path="CMakeLists.txt"/>
</extract>
```

Unterstützte Formate umfassen ZIP sowie libarchive-basierte Archive.

Wichtig: Das **verschachtelte** `<extract><require>` ist derzeit bewusst ein Dateinachweis im extrahierten Upstream-Artefakt. Es ist nicht identisch mit der allgemeinen späteren `<require>`-Action.

## 6. Build

Ein typischer Build-Vertrag enthält:

```xml
<build>
   <environment name="..." value="..."/>
   <argument value="..."/>
   <cmake mode="configure" .../>
   <cmake mode="build" .../>

   <variant name="Release">...</variant>
   <variant name="Debug">...</variant>

   <install>...</install>
</build>
```

Release und Debug verwenden getrennte Buildverzeichnisse. Gemeinsame Argumente stehen im Hauptknoten; Varianten enthalten nur die Unterschiede.

## 7. CMake

```xml
<cmake mode="configure"
       executable="{Tool:cmake}"
       source="..."
       build="..."/>

<cmake mode="build"
       executable="{Tool:cmake}"
       build="..."/>

<cmake mode="install"
       executable="{Tool:cmake}"
       build="..."/>
```

Der CMake-Pfad ersetzt das Upstream-CMake-Projekt nicht. BuildEngine erzeugt lediglich den reproduzierbaren Aufruf und die Dependency-Umgebung.

## 8. Execute

```xml
<execute name="test"
         executable="..."
         workingDirectory="..."
         successExitCode="0"
         showOutput="false">
   <environment name="..." value="..."/>
   <argument value="..."/>
</execute>
```

`execute` ist generisch und wird für reale Upstream-Tools, Testtreiber oder notwendige Buildsystem-Kommandos verwendet.

## 9. Copy

### Einfache Kopie

```xml
<copy source="..." target="..." overwrite="true"/>
```

### Rekursive Kopie

```xml
<copy source="..." target="..." recursive="true" overwrite="true"/>
```

### Gefilterte Kopie

```xml
<copy source="..."
      target="..."
      recursive="true"
      overwrite="true"
      flatten="true"
      cleanTarget="true">
   <include pattern="**/*.dll"/>
   <include pattern="**/*.pdb"/>
   <exclude pattern="**/tests/**"/>
</copy>
```

Semantik:

- `recursive`: rekursive Suche/Kopie,
- `overwrite`: vorhandene Zieldateien ersetzen,
- `flatten`: ausgewählte Dateien ohne ursprüngliche Unterverzeichnisse im Ziel ablegen,
- `cleanTarget`: Ziel vor der Operation bereinigen,
- `singleFile`: exakt eine ausgewählte Quelldatei verlangen und auf den angegebenen Zielpfad kopieren,
- `<include pattern="...">`: positive Auswahl,
- `<exclude pattern="...">`: nachgelagerte Ausschlüsse.

Pattern-Verhalten:

- `*` überquert keine Verzeichnisgrenze,
- `**` darf über Unterverzeichnisse laufen,
- `?` steht für ein einzelnes Nicht-Trennzeichen,
- Windows-Semantik wird ASCII-case-insensitiv behandelt.

Bei `flatten` führen kollidierende Dateinamen zu einem Fehler statt zu stillem Überschreiben.

Die erweiterte Copy-Funktion ist real durch die ACE/TAO-Paketierung belegt. Der vollständige Zielmaschinenlauf nach Ablösung des Python-Installers endete mit 295/295 PASS.

## 10. `preserveCurrentArtifact`

Mehrere Actions verändern normalerweise `{CurrentArtifact}`. Wenn eine technische Hilfsoperation diesen Kontext nicht übernehmen soll, kann der Vertrag – wo unterstützt – `preserveCurrentArtifact="true"` setzen.

## 11. Require

### Historischer und weiterhin gültiger Default

```xml
<require path="...\file.dll"/>
```

ist identisch zu:

```xml
<require path="...\file.dll" kind="file"/>
```

### Verzeichnis

```xml
<require path="...\include" kind="directory"/>
```

Der Pfad muss existieren und tatsächlich ein Verzeichnis sein.

### Beliebiger Filesystem-Eintrag

```xml
<require path="...\generated" kind="any"/>
```

Der Pfad muss lediglich existieren.

Zulässige Werte:

```text
file
directory
any
```

## 12. Testgebundene Aktionen

Aktionen können mit:

```xml
test="true"
```

an die globale Testauswahl gekoppelt werden. Optional kann über `phase` zwischen Test- und Validierungsrollen unterschieden werden, sofern der jeweilige Action-Typ dies erlaubt.

Upstream-Tests werden nicht ohne Analyse entfernt; wenn die Produktform einzelne Upstream-Tests logisch ausschließt, wird dies dokumentiert.

## 13. Install

Bei kompilierten Bibliotheken:

```xml
<install>
   <perVariant>
      ...
   </perVariant>
   <common>
      ...
   </common>
</install>
```

`perVariant` verarbeitet Release-/Debug-spezifische Artefakte. `common` verarbeitet gemeinsame Header, Lizenztexte oder andere konfigurationsunabhängige Inhalte.

## 14. Publish

```xml
<publish root="Win64x"
         configuration="Release"
         consumer="{BuildFileDir}\cmake\consumer">
   <tree .../>
   <files .../>
   <cmake .../>
</publish>
```

Publish projiziert das versionierte Producer-Paket in den gemeinsamen Consumer-Baum. Das versionierte Paket bleibt autoritativ.

`requiresAllVariants="true"` kann verwendet werden, wenn Publish erst nach vollständig stabilem gemeinsamen SDK-Baum beginnen darf.

## 15. Smoke

Kleine Smokes werden direkt am Library-Knoten registriert:

```xml
<smoke id="consumer"
       source="smokes\example\consumer"
       configuration="Release"
       cmake="{Tool:cmake}"
       ninja="{Tool:ninja}"
       toolchain="{BuildFileDir}\cmake\toolchains\bcc64x-buildengine-cxx.cmake"
       executable="example-consumer.exe">
   <environment name="..." value="..."/>
   <argument value="..."/>
   <run executable="optional-helper.exe"/>
</smoke>
```

Smokes sind keine zweite Upstream-Test-Suite. Sie beweisen die Nutzbarkeit des installierten/publizierten Pakets.

Komplexere Integrationstests und Demos gehören nach `BuildEngine-Tests`.

## 16. ACE/TAO als Referenzfall für den erweiterten Vertrag

ACE/TAO nutzt den generischen XML-Vertrag für eine komplexe Paketierung:

- Release/Debug,
- viele import libraries und DLLs,
- rekursive gefilterte Kopien,
- Flattening,
- Tools wie `tao_idl` und `ace_gperf`,
- kanonisch benannte Service-Executables,
- Headerbäume,
- Lizenz-/Versionsdateien,
- Publish in den gemeinsamen SDK-Baum.

Damit ist der frühere eigene Python-Installer technisch nicht mehr erforderlich.

## 17. Aktuelle Produktpolitik

Für normale Runtime-Bibliotheken gilt Shared DLL + Import-Library als bevorzugte Produktform, wenn Upstream dies sinnvoll unterstützt.

Static ist erlaubt, wenn es technisch begründet ist. GoogleTest wird bewusst als Testinfrastruktur separat bewertet und kann eine dokumentierte statische Ausnahme erhalten.

## 18. Änderungsdisziplin

Bei Änderungen an `build-libraries.xml` gilt:

1. vollständigen aktuellen XML-Stand verwenden,
2. keine Rekonstruktion aus Teilfragmenten,
3. Library-Timestamp bei wirksamer Änderung fortschreiben,
4. XSD und Implementierung synchron halten,
5. Patches versionsgebunden halten,
6. realen Zielmaschinenlauf durchführen,
7. erst danach neue Artefaktnamen/Verträge als bewiesen dokumentieren.
