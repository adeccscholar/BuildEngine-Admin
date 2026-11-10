## Updated BuildEngine contract (10 November 2026)

**Current XML contract; no new PASS evidence.** The previous migration notes below remain unchanged.

### Complete transitive dependency graph

```mermaid
flowchart LR
   N0["ace 8.0.8"]
   N1["openssl 3.5.9"]
   N2["xerces-c 3.3.0"]
   N3["zlib 1.4"]
   N4["brotli 1.2.1"]
   N5["zstd 1.5.7"]
   N0 --> N1
   N0 --> N2
   N0 --> N3
   N1 --> N3
   N1 --> N4
   N1 --> N5
```

**Direct dependencies:** `openssl@3.5.9`, `xerces-c@3.3.0`, `zlib@1.4`

### Configuration and patches

- `-D{BMakeConfiguration}`
- Patch: `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-xerces-modern-borland.patch`
- Patch: `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-zlib-library-names.patch`

---

## Previous migration notes

# ACE 8.0.8

[TOC|Content]

## Current BuildEngine contract (10 November 2026)

**Contract data, not confirmed build execution.** Earlier test results are tied to specific versions and builds; new runs remain **[not verified]**.

Adaptive Communication Environment providing portable networking, concurrency, IPC, and OS abstraction.

**Coordinate:** `ace@8.0.8` · **Category:** `middleware` · **License signal:** `green`

### Declared dependencies (complete transitive closure)

Diagram edges come from the exact pinned XML dependencies. Dashed edges identify extensions, solid edges identify package dependencies. Upstream-bundled components are not additional BuildEngine coordinates.

```mermaid
flowchart LR
   N0["ace 8.0.8"]
   N1["brotli 1.2.1"]
   N2["openssl 3.5.9"]
   N3["xerces-c 3.3.0"]
   N4["zlib 1.4"]
   N5["zstd 1.5.7"]
   N0 --> N2
   N0 --> N3
   N0 --> N4
   N2 --> N4
   N2 --> N1
   N2 --> N5
```

**Direct dependencies:** `openssl@3.5.9`, `xerces-c@3.3.0`, `zlib@1.4`

### Configuration and integration notes

**CMake-Argumente:**

- `-D{BMakeConfiguration}`

**Build-Umgebung:**

- `ACE_ROOT={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers`
- `SSL_ROOT={DependencyRoot:openssl}`
- `SSL_INCDIR={DependencyRoot:openssl}\include`
- `SSL_LIBDIR={DependencyRoot:openssl}\lib\win64\{Configuration}`
- `XERCESC_INCDIR={DependencyRoot:xerces-c}\include`
- `XERCESC_LIBDIR={DependencyRoot:xerces-c}\lib\win64\{Configuration}`
- `XERCESLIB={XercesLibrary}`
- `ZLIB_ROOT={DependencyRoot:zlib}`
- `ZLIB_INCDIR={DependencyRoot:zlib}\include`
- `ZLIB_LIBDIR={DependencyRoot:zlib}\lib\win64\{Configuration}`
- `BCC32C=`
- `BCC64=`
- `BCC64X=1`
- `MPC_NUMBER_OF_PROCESSORS={BuildJobs}`
- `CODEGUARD=`
- `UNICODE=`
- `DEBUG={DebugFlag}`
- `RELEASE={ReleaseFlag}`
- `PATH={BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers\lib;{BuildRoot}\packages\{LibraryId}\{LibraryVersion}\{Configuration}\ACE_wrappers\bin;{DependencyRoot:openssl}\bin\win64\{Configuration};{DependencyRoot:xerces-c}\bin\win64\{Configuration};{DependencyRoot:zlib}\bin\win64\{Configuration};{ToolDir:perl};{ToolDir:bcc64x};{ToolDir:embarcadero-make};{ENV:PATH}`

**Versionsgebundene Patches:**

- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-xerces-modern-borland.patch`
- `{Workspace}\.buildengine\patches\{LibraryVersion}\bcc64x-zlib-library-names.patch`
- `{BuildFileDir}\patches\ace-tao\8.0.8\bcc64x-xerces-modern-borland.patch`
- `{BuildFileDir}\patches\ace-tao\8.0.8\bcc64x-zlib-library-names.patch`

**Source-Downloads:**

- `https://github.com/DOCGroup/ACE_TAO/releases/download/ACE%2BTAO-8_0_8/ACE%2BTAO-{LibraryVersion}.zip` (SHA-256: `de45b9e50c29b378d3074f4dc0a5d24e0eaa445b44f39461c447aa83f1cc833a`)

**Explizit geforderte Installationsartefakte:**

- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\tools\bin\win64\{Configuration}\ace_gperf.exe`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\ace`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\include\ace\ACE.h`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\COPYING`
- `{InstallRoot}\packages\{LibraryId}\{LibraryVersion}\ACE_VERSION.txt`

**Authoritative sources:** `admin/build-libraries.xml` for configuration and version pins; FSM/build logs for actual execution. [Library catalog](/manual/libraries.md).
