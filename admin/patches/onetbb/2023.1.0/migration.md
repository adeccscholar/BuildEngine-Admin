# oneTBB 2023.1.0 / BCC64X migration

## Non-portable upstream terminate-on-exception conformance case

The upstream `test/conformance/conformance_global_control.cpp` case
`terminate_on_exception: enabled` uses `std::set_terminate` and invokes
`std::longjmp` from the termination handler into a `setjmp` site in the
C++ test frame. Such a jump has **undefined behavior** if equivalent
exception unwinding would invoke non-trivial destructors. It is also not
valid to jump between threads. The upstream source explicitly warns that
the test is unsafe because destructors might not run.

On BCC64X, both the Debug and Release runs of the unmodified suite gave
137/138 passing CTest entries; `conformance_global_control` crashed
during `terminate_on_exception: enabled` / `internal exception`, with
repeated `CHECK(!terminate_handler_called)` failures. These are **not**
acceptable results or evidence of a oneTBB runtime defect by themselves.

`bcc64x-terminate-conformance.patch` removes **only that one doctest
TEST_CASE** for BCC64X, leaving the other `conformance_global_control`
cases and all other upstream tests active. It does not alter the oneTBB
library implementation.

The separate BuildEngine smoke `smokes/onetbb/terminate` replaces the
unsafe check with process isolation. The parent starts two child
processes: one triggers the invalid-range/internal failure and the other
throws from parallel work. Each child installs a termination handler
which uses `TerminateProcess` with a distinct expected exit status.
The parent checks each status and imposes a 30-second deadline.
Unexpected exits (including access violations), missing handler calls,
failed process creation and timeouts fail the smoke. No longjmp or
C++ exception unwinding is attempted by the handler. The smoke is a
separate executable and BuildEngine smoke entry.

The BCC64X toolchain links both DLLs and consumer executables against the
dynamic Embarcadero runtime (`-tR`), verified in the actual Ninja
`conformance_global_control.exe` link command.

References:

- https://deepsource.com/directory/cxx/issues/CXX-W2015
- https://cppreference-45864d.gitlab-pages.liu.se/en/cpp/utility/program/longjmp.html
- https://github.com/uxlfoundation/oneTBB/blob/v2023.1.0/test/conformance/conformance_global_control.cpp

Both the remaining upstream suite and the new isolated smoke MUST pass
before considering the package validated. A separate root-cause analysis
is required if the isolated child processes still crash or fail to
invoke the termination handler.
