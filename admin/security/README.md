# BuildEngine Security-Bewertungen

Dieser Bereich enthaelt die **gemeinsam verteilbare Bewertungsebene fuer die konkrete BuildEngine-Bibliotheks- und Buildkonfiguration**.

Er ist ausdruecklich von spaeteren anwendungsspezifischen Bewertungen getrennt.

## Ebenen

1. **Provider-Daten**
   - OSV/CVE/GHSA-Rohdaten und Identitaeten.
   - Werden nicht durch lokale Bewertungen veraendert.

2. **BuildEngine-Buildbewertung (dieser Bereich)**
   - Bewertung einer Schwachstelle fuer die von BuildEngine konkret erzeugte Bibliothek.
   - Darf ueber `BuildEngine-Admin` verteilt werden, wenn die Bewertung fuer die gemeinsame Buildkonfiguration belastbar ist.
   - Beispiel: Eine curl-Schwachstelle betrifft ausschliesslich HTTP/2, waehrend die BuildEngine-Konfiguration `USE_NGHTTP2=OFF` setzt.

3. **Anwendungsbewertung (spaeter, nicht hier)**
   - Bewertung der tatsaechlichen Nutzung innerhalb einer konkreten Anwendung.
   - Liegt spaeter unter dem lokalen Build-/Arbeitsbereich und wird **nicht** automatisch ueber `BuildEngine-Admin` verteilt.
   - Beispiel: Eine betroffene API ist zwar Bestandteil der Bibliothek, wird von Anwendung X aber nicht aufgerufen.

## Verzeichnisvertrag

Die synchronisierte Admin-Kopie liegt im Production Root unter:

```text
<ProductionRoot>\admin\security\
```

Die VCL-Review-Anwendung arbeitet nicht direkt auf dieser synchronisierten Kopie, sondern mit einer Arbeitsdatenbank unter:

```text
<BuildRoot>\security\shared\build-assessments.sqlite
```

Die ueber das Admin-Repository verteilte Referenzdatenbank ist vorgesehen als:

```text
admin\security\build-assessments.sqlite
```

Beim ersten Start kann die Arbeitsdatenbank aus dieser Referenzdatenbank kopiert werden. Neue Provider-Findings werden in der Arbeitsdatenbank erfasst und bleiben zunaechst `local`.

Eine Bewertung wird erst dann Teil des gemeinsamen BuildEngine-Vertrags, wenn sie explizit als `shared` markiert und ueber die Funktion **Fuer Admin-Verteilung bereitstellen** publiziert wird.

Die Publikation soll in den Checkout des Admin-Repositories schreiben, nicht still in die synchronisierte Laufzeitkopie. Der Checkout ergibt sich aus dem konfigurierten Repository mit `id="admin"`, typischerweise:

```text
<RepositoriesRoot>\BuildEngine-Admin\admin\security\build-assessments.sqlite
```

Danach erfolgt ein normaler Git-Review/Commit. Damit ist eine lokale Bewertung niemals automatisch eine globale Aussage.

## Versionierte Begleitdateien

- `schema.sql` — kanonisches Schema der SQLite-Datenbank.
- `reference-data.sql` — vorgegebene Werte fuer Anwendbarkeit, Exposition, effektives Risiko, Entscheidung und Begruendung.
- `build-assessments.sqlite` — spaeter die versionierte gemeinsame Referenzdatenbank.

Schema und Referenzwerte bleiben auch dann im Repository, wenn die binare SQLite-Datei verteilt wird. So sind Aufbau, Migration und Review nachvollziehbar.

## Grundsaetze

- Provider-Severity wird nie ueberschrieben.
- `no finding` bedeutet nicht `safe`.
- Eine Buildbewertung ist an Bibliothek, Version, Upstream-Identitaet/Commit und Build-Evidence zu binden.
- Aendert sich die Evidence, muss eine alte Bewertung als veraltet erkennbar sein.
- Bewertungen werden revisionsorientiert gespeichert; alte Entscheidungen werden nicht destruktiv ueberschrieben.
- Unbekannte Findings werden automatisch in die Review-Queue aufgenommen.
- `not applicable` ist nicht dasselbe wie `false positive`.
- Nur Buildkonfigurationsaussagen duerfen als `shared` in diesen Datenbestand gelangen.
- Anwendungsspezifische Aussagen duerfen nicht in diese Datenbank publiziert werden.
