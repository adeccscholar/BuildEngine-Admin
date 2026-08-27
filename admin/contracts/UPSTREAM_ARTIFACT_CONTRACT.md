# BuildEngine-Admin – Upstream Artifact Contract

**Contract-ID:** `BE-ADMIN-UPSTREAM-ARTIFACTS-001`  
**Status:** verbindlich  
**Version:** 1  
**Datum:** 27. August 2026  
**Geltungsbereich:** Third-Party-Bibliotheken in `admin/build-libraries.xml`, Windows x64, C++Builder 13 / BCC64X

## 1. Zweck

BuildEngine soll nachweisen, wie weit sich aktuelle Third-Party-Projekte mit BCC64X in ihrem originalen Ökosystem bauen und verwenden lassen. Deshalb ist nicht eine hausinterne Vereinheitlichung von Dateinamen das Ziel, sondern die möglichst unveränderte Nutzung von Upstream-Quelle, Upstream-Buildsystem und dessen Artefaktvertrag.

Der Normalpfad lautet:

```text
Originalquelle -> originales Buildsystem -> BCC64X-Build -> Install -> Require -> Evidence
```

## 2. Autoritative Artefaktnamen

1. Die vom originalen Upstream-Buildsystem mit der aktiven BCC64X/CMake-Toolchain erzeugten Artefaktnamen sind autoritativ.
2. BuildEngine beschreibt und verifiziert diese Namen; BuildEngine führt keine eigene Namenskonvention für Third-Party-Artefakte ein.
3. Präfixe wie `lib`, Upstream-Postfixe, Versionsbestandteile und die Trennung von Shared-, Import- und Static-Library-Namen werden übernommen, sofern sie aus dem Upstream-Buildsystem und der aktiven Toolchain entstehen.
4. Release und Debug werden primär durch die Installationsverzeichnisse getrennt:

```text
bin/win64/Release
bin/win64/Debug
lib/win64/Release
lib/win64/Debug
```

Ein zusätzliches `d`, `debug` oder anderes Namenssuffix wird nicht von BuildEngine erfunden. Es wird nur verwendet, wenn Upstream es selbst vorgibt oder eine nachgewiesene technische Notwendigkeit besteht.

## 3. Producer vor Consumer

Ein nachgelagerter Consumer darf den Artefaktvertrag des Producers nicht ohne technischen Zwang verändern.

Beispiel: Wenn eine Bibliothek mit BCC64X als `libbrotlicommon.lib` entsteht, wird Brotli nicht allein deshalb in `brotlicommon.lib` umbenannt, weil ein anderer Upstream-Consumer diesen Namen auf einer anderen Windows-Toolchain erwartet. Die Integration des Consumers muss die reale Producer-Ausgabe berücksichtigen und wird separat verifiziert.

## 4. Erst bauen, dann Abweichungen definieren

Für eine neue Bibliothek gilt diese Reihenfolge:

1. Offizielle, gepinnte Upstream-Quelle beschaffen.
2. Unverändertes Upstream-Buildsystem verwenden.
3. Mit BCC64X konfigurieren und bauen.
4. Tatsächlich erzeugte Artefakte und Tests auswerten.
5. Install-/`require`-Vertrag an den realen Upstream-/Toolchain-Ausgaben ausrichten.
6. Erst bei einem reproduzierbaren Fehler einen Patch, eine CMake-Anpassung oder eine andere Abweichung erwägen.

Bekannte Probleme älterer C++Builder-Toolchains gelten nicht automatisch als Befund für BCC64X. Sie müssen mit der aktuellen Toolchain reproduziert werden.

## 5. Zulässige Ausnahmen

Eine Abweichung vom Upstream-Artefaktvertrag ist nur zulässig, wenn mindestens einer der folgenden Gründe nachgewiesen ist:

- notwendige ABI-/COFF64-Anpassung für BCC64X;
- nachgewiesener Fehler oder fehlende Unterstützung im Upstream-Buildsystem;
- zwingender technischer Konflikt zwischen Shared-Import-Library und unabhängiger Static-Library;
- nachgewiesene Plattformanforderung, die Upstream für BCC64X nicht korrekt modelliert.

Jede Ausnahme muss:

- explizit im Library-Vertrag sichtbar sein;
- minimalen Umfang haben;
- ihren technischen Grund dokumentieren;
- durch Build-/Test-Evidence belegt sein;
- eine spätere Rückkehr zum Upstream-Verhalten ermöglichen.

Eine reine hausinterne Namenspräferenz ist kein zulässiger Grund.

## 6. Quellarchiv und Transportformat

Das Transportformat eines offiziellen Upstream-Source-Artefakts ist nicht Teil des Artefaktnamensvertrags. Zwischen offiziellen Archivformen desselben Tags/Commits darf gewechselt werden, wenn:

- die Quelle weiterhin eindeutig auf denselben Upstream-Stand verweist;
- das verwendete Archiv einen festen SHA256 besitzt;
- keine Source-Datei inhaltlich verändert wird;
- der Wechsel ausschließlich der reproduzierbaren Bereitstellung dient.

Damit darf beispielsweise ein offizielles GitHub-Tag-ZIP anstelle eines TAR-Archivs verwendet werden, wenn der aktuelle Extraktionspfad spezielle TAR-Einträge noch nicht unterstützt. Das ist kein Source-Patch.

## 7. Aktuelle Anwendung des Contracts

### Brotli 1.2.0

Der unveränderte Upstream-CMake-Build ist mit BCC64X bereits in Release und Debug erfolgreich durchgelaufen. Die erzeugten Shared-Artefakte verwenden im aktuellen BCC64X/CMake-Profil den `lib`-Präfix, beispielsweise `libbrotlicommon.dll`. Der Install-/Require-Vertrag übernimmt deshalb diese realen Namen und erzwingt keine Umbenennung für OpenSSL oder einen anderen Consumer.

### Zstd 1.5.7

Der erste Lauf erreichte den Compiler noch nicht, weil der TAR-Extraktionspfad an einer nicht regulären Archive-Entry stoppte. Der Library-Vertrag wechselt deshalb auf das offizielle GitHub-Tag-ZIP desselben v1.5.7-Stands mit festem SHA256. Zstd selbst bleibt ungepatcht; insbesondere wird `lib/common/mem.h` nicht verändert. Die nächste Evidence ist der reale BCC64X-CMake-Build.

### zlib 1.3.2

Die bestehende CMake-Sonderbehandlung für den Namen der unabhängigen statischen Bibliothek entstand vor diesem Contract. Sie wird während des laufenden Brotli/Zstd-Nachweises nicht verändert, damit die bereits verifizierte zlib-/libarchive-Kette nicht unnötig invalidiert wird. Sie ist als bestehende Ausnahme separat gegen den offiziellen Upstream-Namen `zs`/Debug-Postfix zu prüfen und danach entweder zu entfernen oder mit einem nachgewiesenen technischen Grund zu dokumentieren.

## 8. Abnahmekriterium

Eine Third-Party-Integration erfüllt diesen Contract, wenn der dokumentierte Installationsbestand die vom Upstream-Buildsystem mit BCC64X tatsächlich erzeugten Artefakte unverändert übernimmt oder jede Abweichung nach Abschnitt 5 nachweisbar begründet ist.
