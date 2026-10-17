# tao 4.0.6: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `tao@4.0.6`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- No TAO download/extract or patching here: ACE owns and prepares the complete ACE_wrappers source tree, including TAO source repairs, in Source.

## Abhängigkeiten

- Nicht explizit im lokalen Vertrag aufgeführt.

## Quellbeschaffung

- Nicht explizit im lokalen Vertrag aufgeführt.

## Versionsgebundene Patches und Adapter

- Nicht explizit im lokalen Vertrag aufgeführt.

## Build-Parameter und Einstellungen

- Nicht explizit im lokalen Vertrag aufgeführt.

### Build-Umgebung

- `ACE_ROOT={ExtensionProducerRoot}`
- `TAO_ROOT={ExtensionProducerRoot}\TAO`
- `SSL_ROOT={DependencyRoot:openssl}`
- `SSL_INCDIR={DependencyRoot:openssl}\include`
- `SSL_LIBDIR={DependencyRoot:openssl}\lib\win64\{Configuration}`
- `XERCESC_INCDIR={InstallRoot}\packages\xerces-c\3.3.0\include`
- `XERCESC_LIBDIR={InstallRoot}\packages\xerces-c\3.3.0\lib\win64\{Configuration}`
- `XERCESLIB={XercesLibrary}`
- `ZLIB_ROOT={InstallRoot}\packages\zlib\1.3.2`
- `ZLIB_INCDIR={InstallRoot}\packages\zlib\1.3.2\include`
- `ZLIB_LIBDIR={InstallRoot}\packages\zlib\1.3.2\lib\win64\{Configuration}`
- `BCC32C=`
- `BCC64=`
- `BCC64X=1`
- `CODEGUARD=`
- `UNICODE=`
- `DEBUG={DebugFlag}`
- `RELEASE={ReleaseFlag}`
- `PATH={ExtensionProducerRoot}\lib;{ExtensionProducerRoot}\bin;{DependencyRoot:openssl}\bin\win64\{Configuration};{InstallRoot}\packages\xerces-c\3.3.0\bin\win64\{Configuration};{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{ToolDir:perl};{ToolDir:bcc64x};{ToolDir:embarcadero-make};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `PATH={ExtensionInstallRoot}\tools\bin\win64\Release;{ExtensionInstallRoot}\services\bin\win64\Release;{ExtensionInstallRoot}\bin\win64\Release;{ENV:PATH}`

### Technische Arbeitsschritte

- `generate-workspace`
- `build-workspace`
- `generate-naming-service`
- `build-naming-service`
- `generate-cos-event-service`
- `build-cos-event-service`
- `generate-rt-event-service`
- `build-rt-event-service`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**Gemeinsamer Producer:** ACE 8.0.6 und TAO 4.0.6 besitzen getrennte logische FSM-Identitäten, teilen jedoch den physischen ACE_wrappers-Baum. ACE_ROOT und TAO_ROOT sind auf diesen gemeinsamen Baum abzustimmen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
