# BuildEngine-Vertrag `build-libraries.xml`

`admin/build-libraries.xml` ist der zentrale deklarative Buildvertrag für die von BuildEngine verwalteten C/C++-Bibliotheken. Hier werden Versionen, Abhängigkeiten, Quellen, Buildaktionen, Installationsregeln, Veröffentlichung, Smoke Tests, Security-Metadaten und optional noch vorhandene Upstream-Dokumentationsphasen beschrieben.

Verwandte Dokumente:

- [Werkzeugvertrag `build-tools.xml`](build-tools.md)
- [Werkzeugübersicht](tools.md)
- [Dokumentationsvertrag](documentation.md)
- [BuildEngine-Konfiguration](configuration.md)

Das Schema liegt in `admin/schemas/build-libraries.xsd`. Der aktuelle Vertrag verwendet `schemaVersion="14"`.

## Grundstruktur

```xml
<buildLibraries xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
                xsi:noNamespaceSchemaLocation="schemas/build-libraries.xsd"
                schemaVersion="14">
   <library id="example" version="1.2.3" timestamp="2026-09-12T18:00:00Z">
      <dependency library="zlib" version="1.3.2"/>
      <metadata .../>
      <security>...</security>
      <source>...</source>
      <build>...</build>
      <publish ...>...</publish>
      <smoke .../>
   </library>
</buildLibraries>
```

`library/@id` ist innerhalb des Vertrags eindeutig.

## `<library>`

| Attribut | Bedeutung |
| --- | --- |
| `id` | Eindeutige Library-ID innerhalb von BuildEngine. |
| `version` | Exakte, vom Vertrag gebaute Version. |
| `timestamp` | Änderungszeitpunkt des Library-Vertrags. Er ist Bestandteil technischer Zustände und darf nur geändert werden, wenn der Library-Vertrag tatsächlich geändert wurde. |

Versionsnummern gehören nicht in Build-/Installationspfade, wenn der Pfad bereits aus den Vertragsvariablen gebildet wird. Die Version bleibt Vertragsdaten und wird über Variablen in Pfade eingesetzt.

## Abhängigkeiten

```xml
<dependency library="openssl" version="3.5.8"/>
<dependency library="zlib" version="1.3.2"/>
```

Eine Dependency beschreibt eine explizite BuildEngine-Abhängigkeit. BuildEngine kann daraus den DAG, Installationsvoraussetzungen, Metadaten, SBOM-Beziehungen und Dokumentation ableiten.

Abhängigkeiten dürfen nicht durch zufällige Dateifunde ersetzt werden: der Vertrag bleibt autoritativ.

## Metadaten

```xml
<metadata name="Example Library"
          category="network"
          supplier="Example Project"
          homepage="https://example.invalid/">
   <license name="MIT" spdx="MIT" file="LICENSE">
      <summary>Permissive MIT license.</summary>
   </license>
</metadata>
```

| Feld | Zweck |
| --- | --- |
| `name` | Anzeigename. |
| `category` | Gruppierung in UI und Dokumentation. |
| `supplier` | Upstream-Hersteller/-Projekt. |
| `homepage` | Upstream-Projektseite. |
| `license/@name` | Lesbarer Lizenzname. |
| `license/@spdx` | SPDX-Identifier, soweit eindeutig. |
| `license/@licensor` | Optionaler Lizenzgeber. |
| `license/@file` | Lizenzdatei im Source-/Package-Kontext. |

Diese Metadaten fließen in Paketinformationen, SBOM und Dokumentation ein.

## Security-Metadaten

```xml
<security>
   <repository url="https://github.com/curl/curl.git"
               ref="curl-8_22_0"/>
</security>
```

`ref` identifiziert typischerweise den Upstream-Tag. Optional kann ein `commit` angegeben werden, wenn ein exakter Commit Teil des Security-/Source-Vertrags ist.

Security-Metadaten ersetzen nicht den Source-Hash. Repository-Identität, Release-Tag/Commit und heruntergeladenes Archiv sind unterschiedliche Evidenzebenen.

## `<source>`

Die Source-Phase besteht aus deklarativen technischen Aktionen. Unterstützt werden unter anderem:

```text
download | extract | copy | execute | target
```

Aktionen können über `id` und `dependsOn` einen technischen DAG bilden. Ohne explizite Graph-Metadaten gilt die deklarierte Reihenfolge.

### `<download>`

```xml
<download url="https://example.invalid/example-{LibraryVersion}.tar.xz"
          archive="example-{LibraryVersion}.tar.xz"
          sha256="..."/>
```

`sha256` ist optional im Schema, soll für reproduzierbare externe Source-Downloads aber grundsätzlich gesetzt werden, sofern Upstream eine stabile Release-Datei bereitstellt.

### `<extract>`

```xml
<extract format="libarchive" root="example-{LibraryVersion}">
   <require path="CMakeLists.txt"/>
</extract>
```

Unterstützte Formate sind `zip`, `gzip`, `gz` und `libarchive`.

| Attribut | Bedeutung |
| --- | --- |
| `root` | Erwarteter Archivroot. |
| `merge` | Inhalt mit vorhandenem Arbeitsbestand zusammenführen. |
| `preserveCurrentArtifact` | Aktuelles Aktionsartefakt für folgende Schritte erhalten. |

`<require>` prüft erwartete Dateien/Verzeichnisse unmittelbar nach der Extraktion.

### `<copy>`

```xml
<copy source="..." target="..."
      recursive="true" overwrite="true">
   <include pattern="**/*.h"/>
   <exclude pattern="**/test/**"/>
</copy>
```

Optionen:

- `recursive`
- `overwrite`
- `flatten`
- `cleanTarget`
- `singleFile`
- `preserveCurrentArtifact`
- `test`
- `phase="test|validation"`

### `<execute>`

```xml
<execute name="configure"
         executable="{Tool:perl}"
         workingDirectory="{Workspace}"
         successExitCode="0">
   <environment name="NAME" value="VALUE"/>
   <argument value="..."/>
</execute>
```

`showOutput` steuert, ob Prozessausgabe direkt sichtbar ist. Der vollständige Prozesslauf bleibt im BuildEngine-Logging nachvollziehbar.

### `<target>`

`target` ist der deklarative, evaluierte Prozessadapter für bekannte Buildtreiber.

Unterstützte `type`-Werte im Library-Schema:

```text
generic, cmake, meson, perl, gmake, python, bmake
```

Beispiel:

```xml
<target name="build"
        type="cmake"
        tool="cmake"
        workingDirectory="{Build}">
   <argument value="--build"/>
   <argument value="{Build}"/>
</target>
```

`tool` referenziert eine logische Werkzeug-ID aus [build-tools.xml](build-tools.md). Alternativ kann ein explizites `executable` verwendet werden, wenn der Vertrag dies erfordert.

`<parameter>` stellt treiberspezifische Parameter bereit. `<temporaryFile>` kann temporäre Textdateien aus `<line>`-Einträgen materialisieren. Doppelte geschweifte Klammern in Target-Zeilen erzeugen literale Klammern.

## `<build>` und Varianten

Buildparameter, die für alle Varianten gleich sind, gehören auf die gemeinsame Ebene. Varianten enthalten nur die Unterschiede.

```xml
<build>
   <environment name="CC" value="{Tool:bcc64x}"/>
   <argument value="-G"/>
   <argument value="Ninja"/>

   <target .../>

   <variant name="Release">
      <argument value="-DCMAKE_BUILD_TYPE=Release"/>
   </variant>
   <variant name="Debug">
      <argument value="-DCMAKE_BUILD_TYPE=Debug"/>
   </variant>

   <install>
      <perVariant>...</perVariant>
      <common>...</common>
   </install>
</build>
```

Das Modell ist bewusst **ein Buildvertrag mit mehreren Varianten**, nicht zwei voneinander unabhängige Buildverträge.

### `testsAffectBuild`

`testsAffectBuild="true"` bedeutet, dass die Testeinstellung bereits den Build-Fingerprint beeinflusst. Das soll nur gesetzt werden, wenn die Buildkonfiguration selbst durch aktivierte/deaktivierte Tests verändert wird.

## Direkte Installation ohne Build

Für Header-only oder anderweitig nicht zu übersetzende Libraries kann statt `<build>` ein direkter `<install>`-Block verwendet werden. Unterstützte Aktionen sind dort `execute`, `target`, `copy` und `require`.

## Test- und Validation-Aktionen

Mehrere Aktionstypen besitzen:

```xml
test="true" phase="test"
```

oder

```xml
test="true" phase="validation"
```

Damit werden technische Aktionen semantisch einer Test-/Validierungsphase zugeordnet. Tests sollen nicht leichtfertig deaktiviert werden; ein Fehlschlag wird zuerst analysiert.

## Installation

Bei normalen Buildverträgen wird Installation in zwei Ebenen getrennt:

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

`perVariant` verarbeitet Release/Debug-spezifische Artefakte. `common` verarbeitet gemeinsame Header, CMake-Metadaten, Lizenzen oder andere variantsunabhängige Dateien.

Shared Libraries mit DLL plus Importbibliothek sind im Projekt die Standardform. Statische Artefakte müssen, wenn sie zusätzlich erzeugt werden, klar unterscheidbar benannt sein.

## Veröffentlichung mit `<publish>`

`publish` erzeugt den konsumierbaren SDK-/Package-Sichtbereich aus dem installierten Bestand.

```xml
<publish root="..."
         configuration="Release"
         consumer="cmake"
         requiresAllVariants="false">
   <tree source="include" target="include"/>
   <files source="bin" target="bin" extensions=".dll"/>
   <cmake source="lib/cmake" target="lib/cmake"/>
</publish>
```

Unterstützte Einträge:

- `<tree>` – kompletter Teilbaum
- `<files>` – dateitypbasierte Veröffentlichung
- `<cmake>` – CMake-Paketinformationen

`optional="true"` erlaubt bewusst fehlende optionale Artefakte. Das darf nicht verwendet werden, um eigentlich notwendige Buildfehler zu kaschieren.

## Library-lokale Smoke Tests

```xml
<smoke id="basic"
       scope="published"
       source="..."
       configuration="Release"
       cmake="cmake"
       ninja="ninja"
       toolchain="..."
       executable="...">
   <argument value="..."/>
   <run executable="..."/>
</smoke>
```

`scope` ist `published` oder `package`. Smoke Tests prüfen den konsumierbaren Zustand und sollen nicht durch interne Buildverzeichnisse zufällig erfolgreich werden.

## `<documentation>` im Library-Vertrag

Der Library-Vertrag besitzt historisch optionale `doc`- und `doxygen`-Phasen. Diese werden derzeit noch unabhängig unterstützt, bis die betroffenen Upstream-Verträge bereinigt sind.

Die **zentrale BuildEngine-API-Dokumentation** wird dagegen durch [build-documentation.xml](documentation.md) gesteuert. Dort werden Doxygen-Profil, Source-Sicht, Excludes und PDF/LaTeX geregelt.

Insbesondere gilt: wenn zentrale PDF-Dokumentation aktiv ist, erzeugt **derselbe Doxygen-Lauf HTML und LaTeX**. Es gibt keinen zweiten Doxygen-Lauf für PDF.

## BuildEngine-Variablen

Verträge verwenden aufgelöste Variablen statt fest kodierter Maschinenpfade. Typische Beispiele sind:

```text
{LibraryId}
{LibraryVersion}
{ProductionRoot}
{SourceRoot}
{BuildRoot}
{InstallRoot}
{WorkspaceRoot}
{BDS}
{Tool:<id>}
{ToolVersion:<id>}
{ENV:<name>}
```

Konkrete Variablen hängen vom jeweiligen Aktionskontext ab. Pfade und Befehle sollen deklarativ aus diesen Werten zusammengesetzt werden.

## Technischer Zustand

BuildEngine entscheidet nicht primär anhand vorhandener Ausgabedateien, ob ein Schritt aktuell ist. Autoritativ sind die technischen Step-States mit Library-Timestamp und Fingerprint. Die Existenz notwendiger Ergebnisdateien ist zusätzliche Evidenz.

Daraus folgen zwei Regeln:

1. Ein vorhandenes Artefakt ohne passenden Step-State macht einen Schritt nicht automatisch aktuell.
2. Ein unabhängiger Vertragswechsel darf nicht über einen globalen Sammel-Fingerprint alle Libraries neu bauen.

## Änderungsregeln

Bei Änderungen an einem Library-Vertrag:

1. Nur die tatsächlich betroffene Library ändern.
2. `timestamp` dieser Library aktualisieren, wenn sich ihr technischer Vertrag geändert hat.
3. Keine Versionsnummern unnötig in Pfadkonstanten duplizieren.
4. Toolchain und BCC64X-Integrationsziel nicht still durch alternative Compiler ersetzen.
5. Tests nicht ohne Analyse abschalten.
6. Source-Hashes, Patchbindung und Security-Evidenz aktualisieren, wenn sich die Source-Version ändert.
7. Diese Markdown-Dokumentation erweitern, wenn sich Semantik oder XML-Vokabular ändert.

## Pflegegrundsatz

`build-libraries.xml`, sein XSD und dieses Dokument werden gemeinsam gepflegt. Neue XML-Funktionen gelten erst dann als vollständig integriert, wenn ihre Semantik auch hier dokumentiert ist.
