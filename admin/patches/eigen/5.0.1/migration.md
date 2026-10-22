# Eigen 5.0.1 migration

Eigen is a header-only C++ linear algebra library. It has no DLL, shared runtime, import library, or static archive of its own. Do not create a dummy compiled library merely to satisfy a binary-package rule.

- Source: official GitLab tag 5.0.1; version-pinned archive in BuildEngine XML.
- Install: copy the original `Eigen/` and `unsupported/` include trees under `include/eigen3`; retain upstream license notices and `COPYING.README`.
- Build: no build phase, no compiler replacement. Consumer smoke compiles C++23 using BCC64X and checks a dense LDLT solve and sparse matrix-vector multiplication.
- Publication: include headers only. No `.lib` or `.dll`.
- oneTBB and OpenMP: Eigen itself is not treated as requiring either runtime globally. Its documented OpenMP parallelization is optional and consumer/build dependent; enabling global OpenMP compiler flags for a header-only library would change all consumers and requires separate validation. Likewise, oneTBB is not a mandatory Eigen dependency.
- Validation: pending actual Debug/Release smoke and installation results.
