# Zentrale Bibliotheksbewertung

Fuer den aktuellen BuildEngine-Stand existiert genau **eine** Security-Bewertungsebene: die zentrale Bewertung der von BuildEngine erzeugten Bibliotheken.

## Zentrale Datenbank im Arbeitsbereich

Die von den Programmen verwendete Datenbank liegt unter:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

Sie enthaelt gemeinsam:

- Sprachen,
- lokalisierte Referenzwerte,
- Provider-Findings,
- Build-/Bibliotheks-Findings,
- zentrale Assessment-Revisionen.

`BuildEngine-Manager` liest und schreibt diese Datei direkt. `BuildEngine-Server` oeffnet dieselbe Datei ausschliesslich read-only.

Es gibt aktuell **keine** zweite Arbeitsdatenbank unter `build`, keine lokale Bewertungsebene und keine Anwendungsbewertung.

## Repository als Verteilungsquelle

Im `BuildEngine-Admin`-Repository liegt dieselbe Datenbank unter:

```text
admin\security\build-assessments.sqlite
```

Der Repository-Stand ist die verteilte Referenzversion. Eine neue Installation bzw. ein Repository-Sync uebernimmt diese Datei in den Arbeitsbereich.

Wenn im Repository eine aktualisierte Version der Datenbank vorhanden ist, **darf und soll** der normale Admin-Sync die vorhandene Datenbank unter

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

ueberschreiben. Genau dadurch werden neue Sprachen, Wertebereiche, Provider-Findings und bereits zentral erarbeitete Bibliotheksbewertungen verteilt.

Die Datenbank darf deshalb im Repository-Sync **nicht** als `preserve` behandelt werden.

## Zentrale Bearbeitung

Die fachliche Bearbeitung erfolgt zentral auf der Datenbank des Arbeitsbereichs:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

Der Manager kann dort:

- neue CVE-/Provider-Findings aus dem Monitoring uebernehmen,
- neue Bibliotheksbewertungen anlegen,
- bestehende Bewertungen als neue Revision fortschreiben,
- Sprachen und Wertebereiche verwenden bzw. spaeter administrieren.

Nach einer geprueften Aenderung wird diese Datenbank bewusst in den ausgecheckten `BuildEngine-Admin`-Arbeitsbaum kopiert:

```text
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Danach folgen normaler Git-Review, Commit und Push. Ab diesem Zeitpunkt ist die neue Datenbankversion die verteilte Referenz und wird bei anderen BuildEngine-Arbeitsbereichen durch den normalen Admin-Sync uebernommen.

## Verteilungskreislauf

```text
BuildEngine-Admin Repository
   admin\security\build-assessments.sqlite
        |
        | RepositorySync
        v
<ProductionRoot>\admin\security\build-assessments.sqlite
        |
        | BuildEngine-Manager: Monitoring + Bewertung
        v
<ProductionRoot>\admin\security\build-assessments.sqlite
        |
        | bewusst zur Verteilung kopieren
        v
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
        |
        | Git Review / Commit / Push
        v
BuildEngine-Admin Repository
```

## Aktueller fachlicher Umfang

Bewertet wird ausschliesslich die konkrete BuildEngine-Bibliothekskonfiguration, z. B.:

- betroffene Funktion beim Build deaktiviert,
- betroffene Komponente nicht gebaut,
- betroffene Komponente nicht ausgeliefert,
- Exploit-Voraussetzung in der Buildkonfiguration nicht vorhanden,
- lokaler Patch vorhanden,
- aktualisierte Version enthaelt den Fix.

Anwendungsspezifische Aussagen sind nicht Bestandteil dieses Modells und werden erst in einer spaeteren Ausbaustufe eingefuehrt.
