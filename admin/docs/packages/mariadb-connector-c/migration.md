# MariaDB Connector/C 3.4.11 — BCC64X migration

The BuildEngine source of truth is `admin/build-libraries.xml`. The upstream project is built using CMake + Ninja + BCC64X only, with Schannel TLS and native Windows certificate verification. External installed zlib and zstd are declared dependencies. A version-bound patch disables the unconditional upstream static mariadbclient and static install, leaving the shared libmariadb DLL/import library.

Source: official v3.4.11 GitHub tag. License: LGPL-2.1-or-later; copy upstream `COPYING.LIB` and preserve notices. The serverless consumer smoke checks the DLL through the C API (`mysql_get_client_version`, `mysql_init`, `mysql_close`). It does **not** verify connection to a MariaDB server.

Build, install, runtime loader, smoke execution, and MariaDB 11 connectivity on the user's Windows BCC64X machine remain **not verified**. Validate those before describing the package as PASS. Patches must be rebuilt against exact upstream revision, never applied unchecked. No compiler fallback, no static OSS libraries, and no branch or GitHub Actions.
