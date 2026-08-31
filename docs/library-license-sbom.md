# Kumulierte Lizenzinformationen und SBOM

## Ziel

BuildEngine erzeugt für jede Bibliothek nach erfolgreicher Installation zwei paketbezogene Metadatenartefakte:

- `install/packages/<LibraryId>/<LibraryVersion>/LICENSE-INFO.txt`
- `install/packages/<LibraryId>/<LibraryVersion>/sbom.cdx.json`

Beide Dateien werden aus demselben, von BuildEngine aufgelösten Abhängigkeitsgraphen erzeugt. Dadurch verwenden Lizenzbericht und SBOM dieselben konkreten Bibliotheksversionen und dieselben direkten bzw. transitiven Abhängigkeiten.

Die Lizenzinformationen werden dabei grundsätzlich aus dem tatsächlich entpackten und verifizierten Upstream-Quellstand ermittelt. Der Buildvertrag ist keine Lizenzdatenbank und soll im Normalfall keine Lizenzdaten enthalten müssen.

Die Original-Lizenzdateien bleiben Bestandteil der jeweiligen installierten Bibliothek. `LICENSE-INFO.txt` ist eine kumulierte Übersicht und ersetzt keine Original-Lizenztexte.

## Automatische Ermittlung

Der Metadata-Job analysiert den durch `<source>/<extract root="...">` bestimmten Upstream-Quellbaum erst zur Laufzeit, nachdem Source-, Build- und Installationsjobs abgeschlossen sind. Damit funktioniert die Ermittlung auch in einem Clean-Room, in dem beim Erstellen des Jobplans noch kein Quellbaum vorhanden ist.

Die Quellen werden in folgender Reihenfolge verwendet:

1. vorhandene standardisierte Upstream-SBOM-Dateien im JSON-Format,
2. Original-Lizenzdateien des Upstream-Releases,
3. optionale explizite `metadata`-Angaben des Buildvertrags als Override.

Unterstützt werden derzeit:

- CycloneDX-JSON,
- SPDX-JSON,
- typische Lizenzdateien wie `LICENSE`, `LICENSE.txt`, `LICENSE.md`, `COPYING`, `COPYRIGHT`, `NOTICE` und `UNLICENSE`,
- automatische Erkennung verbreiteter Lizenztexte anhand charakteristischer Textmerkmale.

Bei der automatischen Suche werden typische eingebettete Fremdquellverzeichnisse wie `vendor`, `third_party`, `external`, `deps` und vergleichbare Verzeichnisse nicht als Lizenzquelle der Wurzelbibliothek verwendet. Dadurch soll eine Lizenz eines mitgelieferten Fremdpakets nicht fälschlich der eigentlichen Bibliothek zugeordnet werden.

Kann eine Lizenzdatei gefunden, aber nicht eindeutig einem SPDX-Identifier zugeordnet werden, wird sie als unklassifizierte Upstream-Lizenz gekennzeichnet. Kann keine belastbare Lizenzinformation ermittelt werden, bleibt das Feld ausdrücklich als nicht automatisch ermittelt sichtbar. BuildEngine erfindet keine Lizenzangaben.

## Vorhandene Upstream-SBOM

Wird im Upstream-Quellbaum eine CycloneDX- oder SPDX-JSON-SBOM gefunden, verwendet BuildEngine deren belastbare Komponenten-, Supplier-, Homepage- und Lizenzinformationen als Metadatenquelle.

Die erste erkannte Upstream-SBOM der Wurzelbibliothek wird unverändert zusätzlich als

```text
install/packages/<LibraryId>/<LibraryVersion>/sbom.upstream.json
```

erhalten. Die von BuildEngine erzeugte kumulierte Datei `sbom.cdx.json` bleibt davon getrennt. Ihr Dependency-Graph wird weiterhin ausschließlich aus dem tatsächlich durch BuildEngine aufgelösten Buildvertrag erzeugt. Dadurch werden optionale, Test-, Tool- oder sonstige Upstream-Abhängigkeiten einer gelieferten SBOM nicht ungeprüft als tatsächlich gebaute Abhängigkeiten übernommen.

## Optionale Metadaten im Bibliotheksvertrag

Schema 13 kennt weiterhin einen optionalen `metadata`-Knoten:

```xml
<metadata name="Xerces-C++"
          supplier="Apache Software Foundation"
          homepage="https://xerces.apache.org/xerces-c/">
   <license name="Apache License 2.0"
            spdx="Apache-2.0"
            licensor="Apache Software Foundation and contributors"
            file="LICENSE">
      <summary>Kurze, sachliche Zusammenfassung der wesentlichen Lizenzbedingungen. Diese Zusammenfassung ersetzt nicht den Original-Lizenztext.</summary>
   </license>
</metadata>
```

Dieser Knoten ist kein regulärer Pflegeort für Lizenzinformationen, sondern ein expliziter Override für Sonderfälle, in denen die automatische Upstream-Auswertung unvollständig oder nicht eindeutig ist.

Bedeutung:

- `name`: optionaler Override des Produkt- bzw. Bibliotheksnamens,
- `supplier`: optionaler Override des Herausgebers bzw. Lieferanten,
- `homepage`: optionaler Override der Projekt- oder Produktseite,
- `license/@name`: explizite Lizenzbezeichnung,
- `license/@spdx`: explizite SPDX-Lizenzkennung,
- `license/@licensor`: expliziter Lizenzgeber bzw. Lizenzgebergruppe,
- `license/@file`: relative, installierte Original-Lizenzdatei; BuildEngine prüft deren Vorhandensein,
- `license/summary`: explizite sachliche Lizenzzusammenfassung.

Sind im Override ein oder mehrere `<license>`-Elemente vorhanden, ersetzen sie die automatisch ermittelten Lizenzangaben dieser Bibliothek. Einzelne `name`-, `supplier`- oder `homepage`-Attribute überschreiben nur das jeweilige automatisch ermittelte Feld.

## Kumulierte Lizenzdatei

`LICENSE-INFO.txt` enthält für die Bibliothek selbst sowie für alle transitiv verwendeten Bibliotheken mindestens:

- Bibliotheks-ID und konkrete Version,
- Beziehung zur Wurzelbibliothek (Bibliothek, direkte Abhängigkeit, transitive Abhängigkeit),
- automatisch erkannte oder explizit überschriebene Lizenzbezeichnung und SPDX-Kennung,
- Lizenzgeber, soweit belastbar ermittelt,
- Upstream-Lizenzdatei bzw. installierte Lizenzdatei, soweit vorhanden,
- Herkunft/Nachweis der ermittelten Information,
- eine technische Zusammenfassung des Erkennungsergebnisses.

Die Transitivität wird nicht aus bereits erzeugten Textdateien rekonstruiert, sondern direkt aus dem von BuildEngine validierten Abhängigkeitsgraphen. Dadurch kann eine Änderung einer konkreten Dependency-Version nicht unbemerkt an Lizenzbericht oder SBOM vorbeigehen.

## SBOM

Der standardisierte BuildEngine-Export ist CycloneDX 1.7 JSON. Die JSON-Serialisierung erfolgt mit `nlohmann::json`; das interne Metadaten- und Abhängigkeitsmodell ist davon getrennt.

Soweit aus Upstream-Quellen oder einem expliziten Override belastbar verfügbar, enthält die SBOM:

- Komponente und Version,
- `bom-ref` und generische Package URL,
- direkte und transitive Dependency-Beziehungen,
- Supplier,
- Lizenz bzw. SPDX-ID,
- Projekt-Homepage,
- verwendete Download-URL,
- SHA-256 des heruntergeladenen Quellarchivs,
- BuildEngine-Bibliotheks-ID und Vertrags-Timestamp,
- Evidenz der Lizenzermittlung als BuildEngine-Properties,
- Anzahl erkannter Upstream-SBOM-Dateien.

Die Wurzelbibliothek steht als `metadata.component` in der SBOM; alle transitiven Abhängigkeiten werden als Komponenten und im Dependency-Graph aufgeführt. Auch Komponenten ohne eigene Abhängigkeiten erhalten einen Dependency-Eintrag mit leerer Abhängigkeitsliste, damit ihre Abhängigkeitssituation nicht als unbekannt interpretiert werden muss.

## Inkrementeller State

Der Metadata-Job lautet:

```text
library:<LibraryId>:metadata
```

Er hängt von der erfolgreichen Installation der jeweiligen Bibliothek ab. Der State befindet sich unter:

```text
install/packages/<LibraryId>/<LibraryVersion>/.buildengine/metadata.state
```

Der State berücksichtigt die Wurzelkomponente, alle transitiven Komponenten, deren konkrete Versionen und Vertrags-Timestamps, die Dependency-Kanten sowie die aus Upstream-SBOM, Lizenzdateien oder Overrides ermittelten Lizenz-, Supplier-, Homepage-, Download- und Hash-Metadaten.

Ein Metadata-Job wird erneut ausgeführt, wenn beispielsweise:

- `LICENSE-INFO.txt` oder `sbom.cdx.json` fehlt,
- eine konkrete Dependency-Version geändert wurde,
- sich der transitive Dependency-Graph geändert hat,
- sich der Vertrags-Timestamp einer beteiligten Bibliothek geändert hat,
- sich die automatisch erkannten oder explizit überschriebenen Metadaten geändert haben.

Metadatenjobs laufen im normalen Scheduler. Sie blockieren nicht die Verwendung eines bereits fertigen SDKs als Build-Abhängigkeit einer anderen Bibliothek; der gesamte Buildlauf ist jedoch erst erfolgreich, wenn auch alle eingeplanten Metadatenjobs erfolgreich abgeschlossen sind.

## Pflegeprinzip

Lizenzinformationen werden soweit möglich automatisch aus belastbaren Primärquellen des konkreten Upstream-Releases ermittelt. Manuelle Lizenzpflege im Buildvertrag ist ausdrücklich nicht der Normalfall. Explizite Metadaten werden nur für belegbare Sonderfälle oder zur Korrektur einer nicht eindeutigen automatischen Erkennung eingesetzt.
