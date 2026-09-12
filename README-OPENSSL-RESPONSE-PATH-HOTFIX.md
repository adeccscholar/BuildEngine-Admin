# OpenSSL 3.5.8 BCC64X response-file external-library path hotfix

## Cause

The existing patch `bcc64x-linker-response-paths.patch` already normalized OpenSSL-internal object, resource, DEF, and output paths for the GNU/Clang response files used by BCC64X/`ld.lld`.

The newly enabled external compression libraries, however, are written by OpenSSL through `LIB_EX_LIBS`, `DSO_EX_LIBS`, and `BIN_EX_LIBS` into the same response files. These variables had not been normalized. As a result, for example,

```text
D:\local\embarcadero\test_v2\install\packages\zlib\1.3.2\lib\win64\Release\libz.lib
```

was interpreted by `ld.lld` as

```text
D:localembarcaderotest_v2installpackageszlib1.3.2libwin64Releaselibz.lib
```

## Correction

The existing patch is replaced. It now additionally creates variables used only for linker response files:

```text
EX_LIBS_RESP
CNF_EX_LIBS_RESP
```

and uses them for:

```text
LIB_EX_LIBS
DSO_EX_LIBS
BIN_EX_LIBS
```

`ld_resp_path()` replaces backslashes with forward slashes only when `ld_resp_forward_slashes` is active. Normal Windows/Make paths remain unchanged.

The correction therefore applies not only to zlib but equally to Brotli, Zstd, and later external OpenSSL link libraries.

## Integration into BuildEngine-Admin

1. Replace this file:

   ```text
   admin/patches/openssl/3.5.8/bcc64x-linker-response-paths.patch
   ```

2. In `admin/build-libraries.xml`, raise **only** the timestamp of OpenSSL 3.5.8:

   before:

   ```xml
   <library id="openssl" version="3.5.8" timestamp="2026-08-27T20:30:00Z">
   ```

   after:

   ```xml
   <library id="openssl" version="3.5.8" timestamp="2026-08-27T23:15:00Z">
   ```

3. Do **not** change the timestamps of zlib 1.3.2, Brotli 1.2.0, or Zstd 1.5.7.

The OpenSSL timestamp must increase because otherwise the existing `library:openssl:source` state would not reapply the changed patch file. The producer libraries are not invalidated; only OpenSSL is replanned.

## Expected next run

- zlib: current
- Brotli: current
- Zstd: current
- OpenSSL source/build: rebuild
- linker-response paths for external libraries contain `/` instead of `\`

Status: statically analyzed; a real BCC64X run has not yet been verified.
