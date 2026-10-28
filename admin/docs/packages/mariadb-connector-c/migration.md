# MariaDB Connector/C 3.4.11 — BCC64X migration

The BuildEngine source of truth is `admin/build-libraries.xml`. The upstream project is built using CMake + Ninja + BCC64X only, with Schannel TLS and native Windows certificate verification. The Windows SDK resource compiler is registered explicitly through `CB_WINDOWS_RC={Tool:windows-rc}`, which the shared BCC64X toolchain maps to `CMAKE_RC_COMPILER`. This applies to `dialog.rc` and all other dynamically built authentication plugins; the plugins are not disabled. `--fresh` invalidates the previous configure cache so no incorrectly detected resource compiler is reused. External installed zlib and zstd are declared dependencies. A version-bound patch disables the unconditional upstream static mariadbclient and static install, leaving the shared libmariadb DLL/import library.

Source: official v3.4.11 GitHub tag. License: LGPL-2.1-or-later; copy upstream `COPYING.LIB` and preserve notices. The serverless consumer smoke checks the DLL through the C API (`mysql_get_client_version`, `mysql_init`, `mysql_close`). It does **not** verify connection to a MariaDB server.

The user-reported BuildEngine run on 2026-10-28 confirmed **build Release PASS** and **build Debug PASS** (approximately 25 seconds each) after Windows SDK RC provisioning. Install failed only because the expected canonical `libmariadb.lib` was not supplied at the declared path. Import-library generation, install, smoke execution, and MariaDB 11 connectivity remain **not verified**. Validate those before describing the package as PASS. Patches must be rebuilt against exact upstream revision, never applied unchecked. No compiler fallback, no static OSS libraries, and no branch or GitHub Actions.

## Build issue: `dialog.rc.res`

On 2026-10-28 both Debug and Release originally failed during compilation of `CMakeFiles/dialog.dir/win/dialog.rc.res`. The upstream `dialog` target is an authentication plugin created by `REGISTER_PLUGIN` and has a generated version-info `.rc` file. The original XML omitted `CB_WINDOWS_RC`, even though BuildEngine's BCC64X toolchain only registers the SDK RC compiler when that environment variable is supplied. The change provides it for both producer and consumer configurations and uses a fresh configure. **Expected fix, not confirmed PASS:** the original log excerpt did not contain the RC process's stderr; inspect `02-cmake-build.log` if the same target continues to fail. Do not disable the plugin to hide a resource compiler failure.

## Install issue: missing `libmariadb.lib` (2026-10-28)

The user-reported BuildEngine FSM confirms both DLL build variants PASS. The former install contract required `lib/win64/{Configuration}/libmariadb.lib` without ensuring that upstream CMake generates and installs that exact filename. This is a naming/staging contract error, not a reason to recompile the client DLL.

The corrected **XML-only** install path first stages and verifies `bin/win64/{Configuration}/libmariadb.dll`, then invokes the already-proven BCC64X import generation workflow from libpq:

```text
{Tool:tdump} -def <installed libmariadb.dll> <variant build directory>/libmariadb.def
{Tool:ld-lld} -m i386pep --out-implib <installed libmariadb.lib> <variant build directory>/libmariadb.def
```

This creates only a COFF import library (no static object code). It uses the actual DLL export table rather than assuming an upstream CMake import target filename. It works independently for Release and Debug and preserves the BuildEngine's published `libmariadb.lib` consumer contract. The initial Windows release/debug execution of this change is pending; only then mark install/smoke PASS. Do not create ad hoc scripts or change the generic BuildEngine executor.
