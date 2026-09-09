# Zentrale Bibliotheksbewertung

Fuer den aktuellen BuildEngine-Stand existiert genau **eine** Security-Bewertungsebene: die zentrale Bewertung der von BuildEngine erzeugten Bibliotheken.

## Fuehrende Datenbank

Die fuehrende Arbeitsdatenbank ist:

```text
<ProductionRoot>\admin\security\build-assessments.sqlite
```

Diese Datei enthaelt gemeinsam:

- Sprachen,
- lokalisierte Referenzwerte,
- Provider-Findings,
- Build-/Bibliotheks-Findings,
- zentrale Assessment-Revisionen.

`BuildEngine-Manager` liest und schreibt diese Datei direkt. `BuildEngine-Server` oeffnet dieselbe Datei ausschliesslich read-only.

Es gibt aktuell **keine** zweite Arbeitsdatenbank unter `build`, keine lokale Bewertungsebene und keine Anwendungsbewertung.

## Verteilung

Im `BuildEngine-Admin`-Repository liegt eine initialisierte, fachlich leere Datenbank unter:

```text
admin\security\build-assessments.sqlite
```

Sie enthaelt Schema, Sprachen und Referenzwerte, aber anfangs keine Provider-Findings und keine Assessments.

Beim ersten Repository-Sync wird diese Datei in den Arbeits-Admin kopiert, wenn dort noch keine fuehrende Datenbank existiert.

Sobald die Datei unter `<ProductionRoot>\admin\security` existiert, darf ein normaler Admin-Sync sie nicht mehr durch die Repository-Kopie ueberschreiben. Nach einer zentralen Bewertung kann der Manager den Stand explizit in den ausgecheckten `BuildEngine-Admin`-Arbeitsbaum zurueckschreiben. Commit und Push bleiben ein bewusster separater Schritt.

## Aktueller fachlicher Umfang

Bewertet wird ausschliesslich die konkrete BuildEngine-Bibliothekskonfiguration, z. B.:

- betroffene Funktion beim Build deaktiviert,
- betroffene Komponente nicht gebaut,
- betroffene Komponente nicht ausgeliefert,
- Exploit-Voraussetzung in der Buildkonfiguration nicht vorhanden,
- lokaler Patch vorhanden,
- aktualisierte Version enthaelt den Fix.

Anwendungsspezifische Aussagen sind nicht Bestandteil dieses Modells und werden erst in einer spaeteren Ausbaustufe eingefuehrt.
