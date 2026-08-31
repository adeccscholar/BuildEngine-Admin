# Kumulierte Lizenzinformationen und SBOM

## Ziel

BuildEngine erzeugt für jede Bibliothek nach erfolgreicher Installation zwei paketbezogene Metadatenartefakte:

- `install/packages/<LibraryId>/<LibraryVersion>/LICENSE-INFO.txt`
- `install/packages/<LibraryId>/<LibraryVersion>/sbom.cdx.json`

Beide Dateien werden aus demselben, von BuildEngine aufgelösten Abhängigkeitsgraphen erzeugt. Dadurch verwenden Lizenzbericht und SBOM dieselben konkreten Bibliotheksversionen und dieselben direkten bzw. transitiven Abhängigkeiten.

Die Original-Lizenzdateien bleiben Bestandteil der jeweiligen installierten Bibliothek. `LICENSE-INFO.txt` ist eine kumulierte Übersicht und ersetzt keine Original-Lizenztexte.

## Lizenzmetadaten im Bibliotheksvertrag

Schema 13 ergänzt einen optionalen `metadata`-Knoten einer Bibliothek:

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

Bedeutung:

- `name`: menschenlesbarer Produkt- bzw. Bibliotheksname.
- `supplier`: Herausgeber bzw. Lieferant der Bibliothek, soweit belastbar bekannt.
- `homepage`: Projekt- oder Produktseite.
- `license/@name`: menschenlesbare Lizenzbezeichnung.
- `license/@spdx`: SPDX-Lizenzkennung, soweit eindeutig zuordenbar.
- `license/@licensor`: Lizenzgeber bzw. Lizenzgebergruppe, soweit aus den Primärquellen belastbar ableitbar.
- `license/@file`: relative, installierte Original-Lizenzdatei. BuildEngine prüft deren Vorhandensein.
- `license/summary`: sachliche Lizenzzusammenfassung. Sie ist keine Rechtsberatung und ersetzt nicht den Lizenztext.

`metadata` bleibt optional. Fehlen einzelne Angaben, erfindet BuildEngine keine Werte. Der kumulierte Bericht kennzeichnet fehlende Lizenzdaten ausdrücklich als nicht im Buildvertrag hinterlegt; die SBOM enthält nur belastbar verfügbare Felder.

## Kumulierte Lizenzdatei

`LICENSE-INFO.txt` enthält für die Bibliothek selbst sowie für alle transitiv verwendeten Bibliotheken mindestens:

- Bibliotheks-ID und konkrete Version,
- Beziehung zur Wurzelbibliothek (Bibliothek, direkte Abhängigkeit, transitive Abhängigkeit),
- Lizenzbezeichnung und SPDX-Kennung, soweit vorhanden,
- Lizenzgeber, soweit vorhanden,
- Pfad zur Original-Lizenzdatei, soweit deklariert,
- die im Buildvertrag hinterlegte Lizenzzusammenfassung.

Die Transitivität wird nicht aus bereits erzeugten Textdateien rekonstruiert, sondern direkt aus dem von BuildEngine validierten Abhängigkeitsgraphen. Dadurch kann eine Änderung einer konkreten Dependency-Version nicht unbemerkt an Lizenzbericht oder SBOM vorbeigehen.

## SBOM

Der erste standardisierte Export ist CycloneDX 1.7 JSON. Die JSON-Serialisierung erfolgt mit `nlohmann::json`; das interne Metadaten- und Abhängigkeitsmodell ist davon getrennt.

Soweit aus dem Buildvertrag belastbar verfügbar, enthält die SBOM:

- Komponente und Version,
- `bom-ref` und generische Package URL,
- direkte und transitive Dependency-Beziehungen,
- Supplier,
- Lizenz bzw. SPDX-ID,
- Projekt-Homepage,
- verwendete Download-URL,
- SHA-256 des heruntergeladenen Quellarchivs,
- BuildEngine-Bibliotheks-ID und Vertrags-Timestamp,
- ergänzende Lizenzmetadaten als BuildEngine-Properties.

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

Der State berücksichtigt die Wurzelkomponente, alle transitiven Komponenten, deren konkrete Versionen und Vertrags-Timestamps, die Dependency-Kanten sowie die verfügbaren Lizenz-, Supplier-, Homepage-, Download- und Hash-Metadaten.

Ein Metadata-Job wird erneut ausgeführt, wenn beispielsweise:

- `LICENSE-INFO.txt` oder `sbom.cdx.json` fehlt,
- eine konkrete Dependency-Version geändert wurde,
- sich der transitive Dependency-Graph geändert hat,
- sich der Vertrags-Timestamp einer beteiligten Bibliothek geändert hat,
- Lizenz-, Supplier-, Homepage-, Download- oder Hash-Metadaten geändert wurden.

Metadatenjobs laufen im normalen Scheduler. Sie blockieren nicht die Verwendung eines bereits fertigen SDKs als Build-Abhängigkeit einer anderen Bibliothek; der gesamte Buildlauf ist jedoch erst erfolgreich, wenn auch alle eingeplanten Metadatenjobs erfolgreich abgeschlossen sind.

## Pflegeprinzip

Lizenzinformationen werden nur aus belastbaren Primärquellen übernommen, vorzugsweise aus den mit dem konkreten Upstream-Release gelieferten Dateien `LICENSE`, `COPYING`, `NOTICE` oder vergleichbaren Projektdateien. Unsichere Angaben werden nicht ergänzt, sondern bleiben sichtbar unvollständig, bis sie verifiziert sind.
