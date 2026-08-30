# BuildEngine-Admin TODO

Stand: 2026-08-31

## Aktiver nächster Arbeitsblock

1. Xerces-C als eigenständiges BCC64X-Paket in `admin/build-libraries.xml` integrieren.
2. Release und Debug bauen, installieren, publizieren und mit einem kleinen Consumer-Smoke verifizieren.
3. Danach ACE 8.0.6 / TAO 4.0.6 aus dem bereits erfolgreichen Evidenzstand in den aktuellen BuildEngine-Vertrag übernehmen.
4. Xerces-C in den ACE/TAO-MPC-/Feature-Vertrag aufnehmen, damit insbesondere `ACE_XML_Utils` nicht mehr wegen `requires xerces` ausgelassen wird.
5. Bestehende ACE/TAO-Patches und Gates übernehmen; keine neue Portierung beginnen, solange die historische Evidenz den benötigten Vertrag bereits belegt.

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
