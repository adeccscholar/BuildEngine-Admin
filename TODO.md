# BuildEngine-Admin TODO

Stand: 2026-08-31

## Aktiver nächster Arbeitsblock

1. Xerces-C 3.3.0 ist als eigenständiger BCC64X-Vertrag unter `admin/build-libraries.d/xerces-c.xml` angelegt. Offizieller Source-ZIP-SHA256: `c35a6f04e853bde456c65ec38a4496c7ccf60b27c6989ff4e2149db9ea40648c`.
2. Offen ist jetzt der reale Zielmaschinenlauf: Release und Debug konfigurieren, bauen, Upstream-CTest ausführen, installieren, publizieren und den DOM-Consumer-Smoke verifizieren. Die erwarteten BCC64X-Artefaktnamen `libxerces-c.dll` / `libxerces-c.lib` sind bis zu diesem Lauf **[nicht verifiziert]**.
3. Der Windows-Xerces-Vertrag setzt `network-accessor=winsock`, `transcoder=windows` und `message-loader=inmemory` explizit, damit keine zufällig sichtbaren optionalen Backends den Build zwischen Maschinen verändern.
4. Danach ACE 8.0.6 / TAO 4.0.6 aus dem bereits erfolgreichen Evidenzstand in den aktuellen BuildEngine-Vertrag übernehmen.
5. Xerces-C in den ACE/TAO-MPC-/Feature-Vertrag aufnehmen, damit insbesondere `ACE_XML_Utils` nicht mehr wegen `requires xerces` ausgelassen wird.
6. Bestehende ACE/TAO-Patches und Gates übernehmen; keine neue Portierung beginnen, solange die historische Evidenz den benötigten Vertrag bereits belegt.

## BuildEngine-Vertragsorganisation

`admin/build-libraries.xml` bleibt der Hauptvertrag. Neue Bibliotheken können zusätzlich als vollständige Schema-11-Dokumente unter `admin/build-libraries.d/*.xml` liegen. BuildEngine führt diese Fragmente deterministisch vor der bestehenden Validierung zusammen. Damit bleiben Duplicate-ID-, Dependency-, Tool-, Variant-, Publish- und Smoke-Prüfungen unverändert zentral.

Erster Nutzer dieses Mechanismus ist Xerces-C. Der dafür erforderliche BuildEngine-Stand beginnt mit Commit `c9518bc1e69aef97ca52501e374417d3cc042a56`.

## Boost 1.92.0 – bekannte Einschränkung, nicht blockierend

Der Boost-Arbeitsblock ist für den aktuellen Third-Party-Fortschritt abgeschlossen.

Gesichert:

- Boost.Iostreams Plain Output/Flush funktioniert.
- Boost.Serialization Binary Archive funktioniert, auch mit Boost.Iostreams als Streamträger.
- Locale/codecvt- und `uncaught_exceptions()`-Hypothesen wurden isoliert geprüft und ausgeschlossen.
- Ein minimaler BCC64X-DLL-Reproducer zeigt die eigentliche Grenze:
  - DLL `std::ostream::put()` auf einem EXE-eigenen Stream: PASS
  - DLL `std::ostream::flush()`: PASS
  - DLL `operator<<('\n')`: reproduzierbare Access Violation `0xC0000005`
- Boost.Serialization Text Archives benutzen im Destruktionspfad denselben problematischen C++-Stream-Insertion-Pfad.

Entscheidung:

- Reproducer und Dokumentation bleiben erhalten.
- Der bekannte Crashpfad wird nicht mehr im normalen Acceptance-Gate ausgeführt.
- Das akzeptierte Boost-BCC64X-Profil umfasst die verifizierten Pfade und schließt Boost.Serialization Text Archives über diese DLL-/`std::ostream`-Grenze ausdrücklich aus.

Details: `BOOST_1_92_BCC64X_RUNTIME_STATUS.md`.

## ACE/TAO – wiederzuverwendende Evidenz

Bereits belegt und nicht neu zu erfinden:

- ACE 8.0.6 / TAO 4.0.6
- Release-Tag `ACE+TAO-8_0_6`
- Release-Archiv SHA256 `e741c8b0ec0c7d6747b184d674567b1e73a13bdf2c902485d1299bbf267b3fba`
- upstream MPC/BMake-Pfad
- BCC64X-Build mit `BCC64X=1`
- Buildreihenfolge ACE -> ace_gperf -> TAO_IDL -> TAO core
- build-lokales `tao_idl.exe` vor TAO-Core
- installierte `tao_idl.exe` und `ace_gperf.exe` unter `tools/bin`
- Win64-SSL/select-Korrekturen
- upstream SSLIOP-`params_dup.cpp`-Korrektur
- Naming Service, COS Event und RT Event als bereits erreichte Infrastrukturziele
- OpenSSL-Abhängigkeit aus dem historischen Evidenzstand; beim Import auf die aktuell verwaltete OpenSSL-Version im BuildEngine-Vertrag abgleichen statt blind den alten Pfad zu kopieren

Historische Revisionsnamen dienen nur als Provenienz und werden nicht in aktive Pfadnamen übernommen.
