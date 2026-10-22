# BCC64X OpenMP capability smoke

This is a **zero-download consumer capability probe**, not a build of the
LLVM OpenMP runtime. The BCC64X 20.1.7 toolchain compiles with `-fopenmp`;
the link currently resolves against the runtime already accessible to the
compiler environment. That runtime is **not yet version-pinned, installed
as a BuildEngine package, or proven reproducible from source**.

The smoke checks parallel regions, runtime schedules (static/dynamic/guided),
taskgroups, critical/atomic/barrier/single synchronization,
firstprivate/shared data sharing, `omp simd` and repeated team creation.
Every check emits BuildEngine `SMOKE|CHECK|name|PASS/FAIL|detail` and the
overall result is a failure if any test fails.

The probe also reports the path of a loaded `libomp.dll` or
`libiomp5md.dll` where possible; absence of those names is only
diagnostic. The DLL actually loaded must be identified via the Windows
loader or the executable imports before certifying a particular runtime.

Next integration milestone: pin LLVM openmp source to a verified revision,
build an external `libomp` DLL using BCC64X, install DLL and import
library, and link consumer tests explicitly to that package (with DLL
staging). Never silently fall back to the vendor runtime in that test.
Keep runtime ownership and dependency manifests separate from this
toolchain capability probe. Debug and Release must both pass.
