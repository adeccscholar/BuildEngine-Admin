# ace 8.0.6: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `ace@8.0.6`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- Weitere historische Begründungen sind [nicht verifiziert] und müssen bei Bedarf anhand von Commit- und Buildlog-Evidence ergänzt werden.

## Abhängigkeiten

- `openssl 3.5.9`
- `xerces-c 3.3.0`
- `zlib 1.3.2`

## Quellbeschaffung

- `https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-8_0_6/ACE%2BTAO-{LibraryVersion}.zip`

## Versionsgebundene Patches und Adapter

- `ace-tao\8.0.6\bcc64x-win64-upstream-backports.patch`
- `{LibraryVersion}\bcc64x-win64-upstream-backports.patch`
- `ace-tao\8.0.6\bcc64x-xerces-modern-borland.patch`
- `{LibraryVersion}\bcc64x-xerces-modern-borland.patch`
- `ace-tao\8.0.6\bcc64x-zlib-library-names.patch`
- `{LibraryVersion}\bcc64x-zlib-library-names.patch`
- `ace-tao\8.0.6\upstream-ssliop-params-dup-cpp.patch`
- `{LibraryVersion}\upstream-ssliop-params-dup-cpp.patch`

## Build-Parameter und Einstellungen

- Nicht explizit im lokalen Vertrag aufgeführt.

### Build-Umgebung

- `ACE_ROOT={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers`
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
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers\lib;{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers\bin;{DependencyRoot:openssl}\bin\win64\{Configuration};{InstallRoot}\packages\xerces-c\3.3.0\bin\win64\{Configuration};{InstallRoot}\packages\zlib\1.3.2\bin\win64\{Configuration};{ToolDir:perl};{ToolDir:bcc64x};{ToolDir:embarcadero-make};{ENV:PATH}`

### Technische Arbeitsschritte

- `win64-backports-check`
- `win64-backports-apply`
- `xerces-modern-borland-check`
- `xerces-modern-borland-apply`
- `zlib-library-names-check`
- `zlib-library-names-apply`
- `ssliop-backport-check`
- `ssliop-backport-apply`
- `remove-build-tree`
- `copy-build-tree`
- `generate-workspace`
- `build-workspace`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**Gemeinsamer Producer:** ACE 8.0.6 und TAO 4.0.6 besitzen getrennte logische FSM-Identitäten, teilen jedoch den physischen ACE_wrappers-Baum. ACE_ROOT und TAO_ROOT sind auf diesen gemeinsamen Baum abzustimmen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
