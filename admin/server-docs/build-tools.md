# BuildEngine-Vertrag `build-tools.xml`

`admin/build-tools.xml` beschreibt die Werkzeuge, die BuildEngine für Bootstrap, Quellbeschaffung, Build, Tests, Dokumentation und Serverdarstellung verwendet. Die Datei ist deklarativer Bestandteil des synchronisierten Admin-Vertrags. Werkzeugwissen soll deshalb hier und nicht in bibliotheksspezifischen C++-Sonderfällen liegen.

Verwandte Dokumente:

- [Werkzeugübersicht](tools.md)
- [Bibliotheksvertrag `build-libraries.xml`](build-libraries.md)
- [Dokumentationsvertrag](documentation.md)
- [BuildEngine-Konfiguration](configuration.md)

## Grundstruktur

```xml
<buildTools xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
            xsi:noNamespaceSchemaLocation="schemas/build-tools.xsd"
            schemaVersion="5">
   <tool id="ninja" version="1.13.2" required="always">
      ...
   </tool>
</buildTools>
```

Das Schema liegt unter `admin/schemas/build-tools.xsd`.

## `<tool>`

Jedes Werkzeug besitzt mindestens:

| Attribut | Bedeutung |
| --- | --- |
| `id` | Eindeutige logische Werkzeug-ID, z. B. `cmake`, `ninja`, `doxygen`, `miktex`. |
| `version` | Vom Vertrag erwartete/provisionierte Version. |
| `runtimeVersion` | Optional abweichende Version, die als effektive Runtime-Version registriert wird. |
| `required` | `always` oder `when-used`. |

### `required="always"`

Das Werkzeug gehört zur allgemeinen BuildEngine-Werkzeugpalette und wird während der normalen Tool-Vorbereitung aufgelöst und geprüft.

### `required="when-used"`

Das Werkzeug wird nur provisioniert, wenn ein aktiver Vertrag es tatsächlich benötigt. Beispiel: `miktex` wird nur angefordert, wenn wenigstens eine Library effektiv PDF-Dokumentation erzeugt.

## Genau eine Bereitstellungsart

Ein `<tool>` enthält genau eine der folgenden Alternativen:

```text
managed | generated | bds | discover
```

### `<managed>`

BuildEngine lädt und verwaltet das Werkzeug selbst.

```xml
<tool id="ninja" version="1.13.2" required="always">
   <managed root="ninja\1.13.2" executable="ninja.exe">
      <download
         url="https://.../ninja-win.zip"
         archive="ninja-1.13.2.zip"
         sha256="..."/>
   </managed>
   <probe contains="1.13.2">
      <argument value="--version"/>
   </probe>
</tool>
```

| Attribut | Bedeutung |
| --- | --- |
| `root` | Relativer Zielpfad unter `toolsRoot`. |
| `executable` | Relativer Entry-Point innerhalb des Managed Roots. |

#### `<download>`

| Attribut | Bedeutung |
| --- | --- |
| `url` | Downloadquelle; BuildEngine-Variablen dürfen verwendet werden. |
| `archive` | Dateiname im Downloadbereich. |
| `sha256` | Erwarteter SHA-256-Hash. Der Hash ist Teil des reproduzierbaren Liefervertrags. |

Ein Download gilt nicht allein aufgrund eines vorhandenen Dateinamens als vertrauenswürdig; der deklarierte Hash ist maßgeblich.

### Externe Installation/Extraktion mit `<extract>`

Ein Managed Tool kann nach dem verifizierten Download einen externen Installations- oder Extraktionsprozess ausführen:

```xml
<extract executable="{Archive}">
   <argument value="--private"/>
   <argument value="--unattended"/>
</extract>
```

Verfügbare zusätzliche Variablen sind insbesondere:

| Variable | Bedeutung |
| --- | --- |
| `{Archive}` | Vollständiger Pfad des verifizierten Downloads. |
| `{ManagedRoot}` | Aufgelöster Zielroot des Werkzeugs. |
| `{ToolId}` | Werkzeug-ID. |
| `{ToolVersion}` | Vertragsversion des Werkzeugs. |

MiKTeX verwendet diesen Weg, weil der offizielle Basic Installer eine EXE und kein gewöhnliches ZIP-Archiv ist.

### Native Extraktion mit `<nativeExtract>`

Archive können durch den integrierten libarchive-Pfad extrahiert werden:

```xml
<nativeExtract format="libarchive" root="meson-1.12.0">
   <require path="meson.py"/>
   <require path="mesonbuild\mesonmain.py"/>
</nativeExtract>
```

Optionale `<include>`-Muster begrenzen den extrahierten Inhalt. `<require>` definiert Dateien, die nach der Extraktion zwingend vorhanden sein müssen.

### `<generated>`

BuildEngine kann ein Werkzeugartefakt aus bereits vorhandenen Dateien generieren. Der aktuelle Anwendungsfall ist die BCC64X-UCRT-Kompatibilitätsbibliothek.

```xml
<tool id="bcc64x-ucrt-compat" version="20.1.7-compat1" required="always">
   <generated root="bcc64x-ucrt-compat\20.1.7-compat1"
              entry="libbcc64x-ucrt-compat.a">
      <archive tool="{BDS}\bin64\llvm-ar.exe"
               source="{BDS}\x86_64-w64-mingw32\lib\libucrt.a">
         <member path="..."/>
      </archive>
   </generated>
</tool>
```

Der erzeugte Stand wird nur erneuert, wenn der deklarative Vertrag oder die Quelle dies erforderlich macht.

### `<bds>`

Werkzeuge, die Bestandteil der installierten C++Builder/RAD-Studio-Umgebung sind, werden relativ zu `{BDS}` aufgelöst:

```xml
<tool id="bcc64x" version="20.1.7" required="always">
   <bds executable="bin64\bcc64x.exe"/>
</tool>
```

Das ist ausdrücklich keine alternative Compilerwahl: im BCC64X-Projekt bleibt der konfigurierte C++Builder-Toolchainpfad maßgeblich.

### `<discover>`

Bereits installierte Werkzeuge können über deklarierte Kandidaten gefunden werden:

```xml
<discover executable="rc.exe" path="true">
   <candidate path="{ProgramFilesX86}\Windows Kits\10\bin\*\x64\rc.exe"/>
</discover>
```

`path="true"` erlaubt zusätzlich die Suche über die effektive Umgebung. Kandidaten unterstützen die von BuildEngine vorgesehenen Variablen und Wildcards.

## `<launcher>`

Ein Werkzeug kann über ein anderes Werkzeug gestartet werden. Meson ist beispielsweise ein Python-Programm:

```xml
<launcher tool="python">
   <argument value="{ToolEntry}"/>
</launcher>
```

BuildEngine validiert den Launcher-Graphen auf unbekannte Werkzeuge und Zyklen.

## `<probe>`

Nach Auflösung oder Provisionierung kann ein Werkzeug mit einem Versions-/Funktionsprobe geprüft werden:

```xml
<probe contains="cmake version 4.1.1">
   <argument value="--version"/>
</probe>
```

Der Prozess muss erfolgreich enden und die erwartete Zeichenfolge liefern. Erst danach wird das Werkzeug in `admin/tools.xml` als effektiver Tool-State registriert.

## Managed Tool State

`admin/build-tools.xml` ist der Soll-Vertrag. `admin/tools.xml` ist erzeugter Zustand. Die beiden Dateien haben unterschiedliche Rollen:

```text
build-tools.xml  = deklarative Quelle
       ↓
Download / Discovery / Generate / Probe
       ↓
tools.xml        = auf dieser Maschine tatsächlich aufgelöster Zustand
```

`tools.xml` darf deshalb nicht als zweite handgepflegte Quelle für Werkzeugwissen verwendet werden.

## Versionsänderungen

Bei einem Werkzeugupdate müssen mindestens geprüft werden:

1. Version und ggf. `runtimeVersion`.
2. Download-URL.
3. Archiv-/Installername.
4. SHA-256.
5. erwarteter Entry-Point.
6. Probe und erwartete Ausgabe.
7. Auswirkungen auf technische Fingerprints der Verbraucher.
8. Dokumentation in [tools.md](tools.md), falls sich Rolle oder Version ändert.

Ein Toolupdate darf nicht pauschal unabhängige Library-Builds invalidieren. Die Version soll nur in die States einfließen, deren Ergebnis tatsächlich davon abhängt.

## MiKTeX als Beispiel für `when-used`

```xml
<tool id="miktex" version="25.12" required="when-used">
   <managed root="miktex\25.12"
            executable="texmfs\install\miktex\bin\x64\texify.exe">
      ...
   </managed>
</tool>
```

Die Doxygen-Phase entscheidet anhand des effektiven Dokumentationsprofils, ob LaTeX benötigt wird. Doxygen erzeugt HTML und LaTeX gegebenenfalls in **einem Lauf**. MiKTeX wird nur für den nachgelagerten PDF-Schritt benötigt. Details stehen im [Dokumentationsvertrag](documentation.md).

## Pflegegrundsatz

Änderungen am Werkzeugvertrag und Änderungen an seiner Bedeutung werden zusammen dokumentiert. `build-tools.xml`, dieses Dokument und [tools.md](tools.md) sollen deshalb als eine fachliche Einheit gepflegt werden.
