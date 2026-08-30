# Library Consumer Smoke Tests

These sources are part of the library build contract. They are intentionally small consumers of the published
`Win64x` tree and do not replace upstream regression or validation tests.

Every executable writes the same machine-readable protocol to stdout:

```text
SMOKE|CHECK|<id>|PASS|<detail>
SMOKE|CHECK|<id>|FAIL|<detail>
SMOKE|RESULT|PASS|<detail>
SMOKE|RESULT|FAIL|<detail>
```

The BuildEngine `SmokeOutputValidator` reads stdout and stderr without modifying the child process. A successful
smoke requires at least one CHECK, exactly one PASS RESULT, no failed checks, exit code zero, and no stderr output.
Lines not starting with `SMOKE|` are retained as informational stdout. Malformed `SMOKE|` records are protocol errors.

For each smoke, BuildEngine keeps the complete CMake configure/build logs, the complete raw process log, and a
second validation log containing every observed stdout/stderr line plus parsed CHECK/RESULT records and the final
validation decision.

Schema 11 additionally permits multiple smoke gates for one library and two consumer scopes. `published` is the
normal Release view through `install/Win64x`; `package` addresses the exact versioned package/configuration and is
used for Boost Debug evidence without creating a second global SDK tree. A smoke may declare one or more
`<run executable="..."/>` pre-runs. Those programs are built by the same consumer CMake project and must all exit
with code zero before the final protocol-emitting validator is executed.
