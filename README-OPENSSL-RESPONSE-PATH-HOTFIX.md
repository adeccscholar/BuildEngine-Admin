# OpenSSL 3.5.8 BCC64X response-file external-library path hotfix

## Ursache

Der bestehende Patch `bcc64x-linker-response-paths.patch` normalisierte bereits
OpenSSL-interne Objekt-, Ressourcen-, DEF- und Outputpfade fuer die von BCC64X/
`ld.lld` verwendeten GNU/Clang-Response-Dateien.

Die neu aktivierten externen Kompressionsbibliotheken werden von OpenSSL jedoch
ueber `LIB_EX_LIBS`, `DSO_EX_LIBS` und `BIN_EX_LIBS` in dieselben Response-Dateien
geschrieben. Diese Variablen wurden bisher nicht normalisiert. Dadurch wurde z. B.

    D:\local\embarcadero\test_v2\install\packages\zlib\1.3.2\lib\win64\Release\libz.lib

von `ld.lld` als

    D:localembarcaderotest_v2installpackageszlib1.3.2libwin64Releaselibz.lib

interpretiert.

## Korrektur

Der bestehende Patch wird ersetzt. Er erzeugt nun zusaetzlich die nur fuer
Linker-Response-Dateien verwendeten Variablen:

    EX_LIBS_RESP
    CNF_EX_LIBS_RESP

und verwendet diese fuer:

    LIB_EX_LIBS
    DSO_EX_LIBS
    BIN_EX_LIBS

`ld_resp_path()` ersetzt dabei nur bei `ld_resp_forward_slashes` Backslashes durch
Forward Slashes. Die normalen Windows-/Make-Pfade bleiben unveraendert.

Damit gilt die Korrektur nicht nur fuer zlib, sondern ebenso fuer Brotli, Zstd und
weitere spaetere externe OpenSSL-Linkbibliotheken.

## Integration in BuildEngine-Admin

1. Diese Datei ersetzen:

   admin/patches/openssl/3.5.8/bcc64x-linker-response-paths.patch

2. In `admin/build-libraries.xml` AUSSCHLIESSLICH den Timestamp von OpenSSL 3.5.8
   anheben:

   vorher:

       <library id="openssl" version="3.5.8" timestamp="2026-08-27T20:30:00Z">

   nachher:

       <library id="openssl" version="3.5.8" timestamp="2026-08-27T23:15:00Z">

3. Die Timestamps von zlib 1.3.2, Brotli 1.2.0 und Zstd 1.5.7 NICHT aendern.

Der OpenSSL-Timestamp muss steigen, weil der bestehende `library:openssl:source`-
State sonst die geaenderte Patch-Datei nicht erneut anwendet. Die Producer werden
dadurch nicht invalidiert; nur OpenSSL wird neu geplant.

## Erwartung fuer den naechsten Lauf

- zlib: current
- Brotli: current
- Zstd: current
- OpenSSL source/build: rebuild
- Linker-Response-Pfade der externen Bibliotheken enthalten `/` statt `\`

Status: statisch analysiert; realer BCC64X-Lauf noch nicht verifiziert.
