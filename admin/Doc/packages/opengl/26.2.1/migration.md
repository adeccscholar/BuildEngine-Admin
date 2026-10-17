# opengl 26.2.1: Migrationsdokumentation

[TOC|Content]

**BuildEngine-Vertrag:** `admin/build-libraries.xml`, `opengl@26.2.1`. **Ziel:** Windows/BCC64X. Die Dokumentation beruht auf dem deklarativen Vertrag, nicht auf einem unabhängig verifizierten vollständigen Lauf.

## Entscheidungen und dokumentierte Workarounds

- Mesa mandatory Python configure-time modules. Keep them pinned and source-local so the build does not depend on global pip/site-packages.
- Native BCC64X Mesa build. No MSVC, clang-cl or MinGW fallback is permitted.
- Import libraries are derived only from the BCC64X-built Mesa DLLs.

## Abhängigkeiten

- Nicht explizit im lokalen Vertrag aufgeführt.

## Quellbeschaffung

- `https://archive.mesa3d.org/mesa-{LibraryVersion}.tar.xz`
- `https://files.pythonhosted.org/packages/source/m/meson/meson-1.12.0.tar.gz`
- `https://files.pythonhosted.org/packages/7d/fa/3944b40b07da9ce895c0e6303a5ab7d53da063554f534556b134a54d6093/packaging-26.3.tar.gz`
- `https://files.pythonhosted.org/packages/7e/99/7690b6d4034fffd95959cbe0c02de8deb3098cc577c67bb6a24fe5d7caa7/markupsafe-3.0.3.tar.gz`
- `https://files.pythonhosted.org/packages/2a/12/b5fa2353e2754cd67fb9f83793fa48ff42c213a5da7e719869d2301f6ab8/mako-1.4.1.tar.gz`
- `https://files.pythonhosted.org/packages/05/8e/961c0007c59b8dd7729d542c61a4d537767a59645b82a0b521206e1e25c2/pyyaml-6.0.3.tar.gz`

## Versionsgebundene Patches und Adapter

- Nicht explizit im lokalen Vertrag aufgeführt.

## Build-Parameter und Einstellungen

- `-Dplatforms=windows`
- `-Dgallium-drivers=softpipe`
- `-Dgallium-wgl-dll-name=libgallium_wgl`
- `-Dopengl=true`
- `-Dgles1=disabled`
- `-Dgles2=disabled`
- `-Degl=disabled`
- `-Dglx=disabled`
- `-Dgbm=disabled`
- `-Dvulkan-drivers`
- `-Dvulkan-layers`
- `-Dllvm=disabled`
- `-Dzlib=disabled`
- `-Dzstd=disabled`
- `-Dxmlconfig=disabled`
- `-Dbuild-tests=true`
- `-Dtools`
- `-Dgallium-rusticl=false`

### Build-Umgebung

- `CC={Tool:bcc64x}`
- `CXX={Tool:bcc64x}`
- `AR={Tool:llvm-ar}`
- `WINDRES={Tool:bcc64x-windres}`
- `PYTHONPATH={Workspace}\packaging-26.3\src;{Workspace}\markupsafe-3.0.3\src;{Workspace}\mako-1.4.1;{Workspace}\pyyaml-6.0.3\lib`
- `PATH={ToolDir:bcc64x};{ToolDir:llvm-ar};{ToolDir:bcc64x-windres};{ToolDir:ninja};{ToolDir:python};{ENV:PATH}`
- `CB_BCC64X={Tool:bcc64x}`
- `CB_BDS={BDS}`
- `PATH={InstallRoot}\Win64x\bin;{ENV:PATH}`

### Technische Arbeitsschritte

- `remove-build`
- `meson-setup`
- `meson-compile`
- `meson-test`
- `meson-install`
- `create-libdir`
- `opengl-def`
- `opengl-import`
- `wgl-def`
- `wgl-import`

## Installation und Consumer-Hinweise

BuildEngine-Pakete verwenden ihre deklarativen `{InstallRoot}`- und `{BuildRoot}`-Pfade sowie die Versions- und gegebenenfalls Debug/Release-Trennung. BCC64X Windows-Executables und DLLs verwenden die dynamische Runtime `-tR`. Die genaue Installationsstruktur und Verwendung sind anhand der XML-`install`-, `publish`- und `smoke`-Actions zu prüfen.

**[nicht verifiziert]** Ob und in welchem Umfang die konkrete Variante schon durch einen vollständigen Clean-Room-Test bestätigt wurde, ist anhand der FSM-Evidence zu bestimmen.

## Quellen

- `admin/build-libraries.xml`
- `admin/server-docs/libraries.md`
- `docs/bcc64x-library-integration-findings.md`
