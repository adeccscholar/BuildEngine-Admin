# BuildEngine Security-Bewertungen

Dieser Bereich enthaelt die **zentrale, verteilbare Bewertung der konkreten BuildEngine-Bibliotheks- und Buildkonfiguration**.

Eine anwendungsspezifische Bewertung existiert im aktuellen Ausbaustand noch nicht.

## Eine gemeinsame Datenbank

Die zentrale BuildEngine-Bewertung verwendet genau eine SQLite-Datenbank:

```text
admin\security\build-assessments.sqlite
```

Im laufenden BuildEngine-Arbeitsbereich liegt dieselbe Datenbank nach dem Admin-Sync unter:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

Die Datenbank enthaelt gemeinsam:

- Provider-Findings,
- BuildEngine-/Bibliotheks-Findings,
- Bewertungsrevisionen,
- Sprachen,
- lokalisierte Referenzwerte,
- Review-Queue und Views.

Es gibt **keine zweite Datenbank nur fuer Referenzwerte** und aktuell auch **keine zweite Arbeitsdatenbank unter `build`**. `schema.sql` und `reference-data.sql` sind textuell nachvollziehbare Bootstrap-/Migrationsvertraege fuer dieselbe SQLite-Datei.

## Repository als Verteilungsquelle

Die Datei im `BuildEngine-Admin`-Repository ist die verteilte Referenzversion:

```text
BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Der normale Repository-Sync kopiert diese Datei nach:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

Wenn die Repository-Version aktualisiert wurde, **darf und soll** sie die vorhandene Datenbank im Arbeitsbereich ueberschreiben. So werden zentral gepflegte Sprachen, Wertebereiche, Provider-Findings und Bibliotheksbewertungen an andere Installationen verteilt.

Die Datenbank darf daher im Repository-Sync **nicht** als `preserve` behandelt werden.

## Zentrale Bearbeitung

`BuildEngine-Manager` oeffnet die Datenbank unter

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

lesend und schreibend. Dort werden:

- neue CVE-/Provider-Findings aus dem Monitoring uebernommen,
- Bibliotheks-Findings aktualisiert,
- zentrale Bewertungen als Revisionen gespeichert,
- Sprache und lokalisierte Werte aus derselben Datenbank geladen.

`BuildEngine-Server` oeffnet dieselbe Datei ausschliesslich read-only und zeigt die zentral verteilte Bewertung an.

## Rueckverteilung

Nach einer geprueften zentralen Aenderung kopiert der Manager die Datenbank bewusst in den ausgecheckten Admin-Repository-Arbeitsbaum:

```text
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Danach folgen normaler Git-Review, Commit und Push. Erst damit wird die neue Version zur verteilten Referenz fuer andere BuildEngine-Arbeitsbereiche.

## Sprache

Sprachen sind Datensaetze mit stabiler ID in `language`.

Die Referenzwerte besitzen ebenfalls stabile IDs und werden ueber `reference_value_text` pro Sprache lokalisiert.

Der Server liest seine Standardsprache aus `BuildEngine.xml`, z. B.:

```xml
<parameters serverLanguage="de" ... />
```

Der Manager liest seine Startsprache aus `managerLanguage`. Fehlt dieser Eintrag, gilt Englisch (`en`). Beide Sprachcodes werden auf `language.code` aufgeloest.

Deutsch (`id=1`, `code=de`) und Englisch (`id=2`, `code=en`) sind initial enthalten.

## Aktueller fachlicher Umfang

Bewertet wird ausschliesslich die konkrete BuildEngine-Bibliothekskonfiguration, z. B.:

- betroffene Funktion beim Build deaktiviert,
- betroffene Komponente nicht gebaut,
- betroffene Komponente nicht ausgeliefert,
- Exploit-Voraussetzung in der Buildkonfiguration nicht vorhanden,
- lokaler Patch vorhanden,
- aktualisierte Version enthaelt den Fix.

Anwendungsspezifische Aussagen sind nicht Bestandteil dieses Modells und werden erst in einer spaeteren Ausbaustufe eingefuehrt.

## Versionierte Begleitdateien

- `schema.sql` — kanonisches Schema der SQLite-Datenbank.
- `reference-data.sql` — initiale Sprachen und lokalisierte Werte fuer Anwendbarkeit, Exposition, effektives Risiko, Entscheidung und Begruendung.
- `build-assessments.sqlite` — versionierte zentrale Bewertungsdatenbank.

Schema und Referenzwerte bleiben neben der binaeren SQLite-Datei im Repository, damit Aufbau, Migrationen und Reviews nachvollziehbar sind.

## Grundsaetze

- Provider-Severity wird nie ueberschrieben.
- `no finding` bedeutet nicht `safe`.
- Eine Buildbewertung ist an Bibliothek, Version, Upstream-Identitaet/Commit und Build-Evidence zu binden.
- Aendert sich die Evidence, muss eine alte Bewertung als veraltet erkennbar sein.
- Bewertungen werden revisionsorientiert gespeichert; alte Entscheidungen werden nicht destruktiv ueberschrieben.
- Unbekannte Findings werden automatisch in die Review-Queue aufgenommen.
- `not applicable` ist nicht dasselbe wie `false positive`.
- Die zentrale Datenbank darf nur Buildkonfigurationsaussagen enthalten.
