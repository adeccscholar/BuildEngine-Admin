# BuildEngine Security-Bewertungen

Dieser Bereich enthaelt die **gemeinsam verteilbare Bewertungsebene fuer die konkrete BuildEngine-Bibliotheks- und Buildkonfiguration**.

Er ist ausdruecklich von spaeteren anwendungsspezifischen Bewertungen getrennt.

## Ebenen

1. **Provider-Daten**
   - OSV/CVE/GHSA-Rohdaten und Identitaeten.
   - Werden nicht durch lokale Bewertungen veraendert.

2. **BuildEngine-Buildbewertung (dieser Bereich)**
   - Bewertung einer Schwachstelle fuer die von BuildEngine konkret erzeugte Bibliothek.
   - Diese zentrale Bewertung wird im `BuildEngine-Manager` direkt bearbeitet.
   - Sie darf ueber `BuildEngine-Admin` verteilt werden, wenn die Bewertung fuer die gemeinsame Buildkonfiguration belastbar ist.
   - Beispiel: Eine curl-Schwachstelle betrifft ausschliesslich HTTP/2, waehrend die BuildEngine-Konfiguration `USE_NGHTTP2=OFF` setzt.

3. **Anwendungsbewertung (spaeter, nicht hier)**
   - Bewertung der tatsaechlichen Nutzung innerhalb einer konkreten Anwendung.
   - Liegt spaeter unter dem lokalen Build-/Arbeitsbereich und wird **nicht** ueber `BuildEngine-Admin` verteilt.
   - Beispiel: Eine betroffene API ist zwar Bestandteil der Bibliothek, wird von Anwendung X aber nicht aufgerufen.

## Gemeinsame Datenbank

Die zentrale BuildEngine-Bewertung verwendet genau eine SQLite-Datenbank:

```text
admin\security\build-assessments.sqlite
```

Die Datenbank enthaelt gemeinsam:

- Provider-Findings,
- BuildEngine-Findings,
- Bewertungsrevisionen,
- Sprachen,
- lokalisierte Referenzwerte,
- Review-Queue und Views.

Es gibt **keine zweite Datenbank nur fuer Referenzwerte**. `schema.sql` und `reference-data.sql` sind lediglich die textuell nachvollziehbaren Bootstrap-/Migrationsvertraege fuer dieselbe SQLite-Datei.

## Verteilung und Bearbeitung

Die synchronisierte, fuer Server und andere Leser gedachte Kopie liegt im Production Root unter:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

Der `BuildEngine-Manager` darf die zentrale Bewertung bearbeiten. Damit ein Repository-Sync keine gerade geoeffnete Datenbank ueberschreibt, arbeitet der Manager mit einer editierbaren Kopie unter:

```text
<BuildRoot>\security\shared\build-assessments.sqlite
```

Diese Arbeitskopie **ist die zentrale Bewertungsebene**, keine persoenliche oder anwendungsspezifische Bewertung. Sie wird aus der vom Admin-Repository verteilten Datenbank initialisiert bzw. aktualisiert.

Nach einer Review kann der Manager den geprueften Stand mit **Fuer Admin-Verteilung bereitstellen** in den konfigurierten Admin-Repository-Checkout schreiben, typischerweise:

```text
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Danach erfolgt ein normaler Git-Review/Commit. Auf diesem Weg koennen technisch belastbare Ausschluesse oder Risikobewertungen fuer die gemeinsame BuildEngine-Konfiguration von allen Nutzern uebernommen und vom read-only Server angezeigt werden.

## Sprache

Sprachen sind keine freien Texte, sondern Datensaetze mit stabiler ID in `language`.

Die Referenzwerte besitzen ebenfalls stabile IDs und werden ueber `reference_value_text` pro Sprache lokalisiert. Der Server liest seine Standardsprache aus `BuildEngine.xml`, z. B.:

```xml
<parameters serverLanguage="de" ... />
```

Der Sprachcode wird auf `language.code` aufgeloest. Der Manager kann seine aktuelle Anzeigesprache unabhaengig davon aus der `language`-Tabelle auswaehlen.

Deutsch (`id=1`, `code=de`) und Englisch (`id=2`, `code=en`) sind initial enthalten.

## Versionierte Begleitdateien

- `schema.sql` — kanonisches Schema der SQLite-Datenbank.
- `reference-data.sql` — initiale Sprachen und lokalisierte Werte fuer Anwendbarkeit, Exposition, effektives Risiko, Entscheidung, Begruendung und Verteilungsstatus.
- `build-assessments.sqlite` — versionierte gemeinsame Bewertungsdatenbank.

Schema und Referenzwerte bleiben auch neben der binaeren SQLite-Datei im Repository. So sind Aufbau, Migrationen und Reviews nachvollziehbar.

## Grundsaetze

- Provider-Severity wird nie ueberschrieben.
- `no finding` bedeutet nicht `safe`.
- Eine Buildbewertung ist an Bibliothek, Version, Upstream-Identitaet/Commit und Build-Evidence zu binden.
- Aendert sich die Evidence, muss eine alte Bewertung als veraltet erkennbar sein.
- Bewertungen werden revisionsorientiert gespeichert; alte Entscheidungen werden nicht destruktiv ueberschrieben.
- Unbekannte Findings werden automatisch in die Review-Queue aufgenommen.
- `not applicable` ist nicht dasselbe wie `false positive`.
- Die zentrale Datenbank darf nur Buildkonfigurationsaussagen enthalten.
- Anwendungsspezifische Aussagen duerfen nicht in diese Datenbank publiziert werden.
