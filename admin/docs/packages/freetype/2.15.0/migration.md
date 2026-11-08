# freetype 2.15.0 — BCC64X integration

Upstream source is the official FreeType 2.15.0 tar.xz, SHA-256 `93cd47eb719c325107f2d3cf087d06299e944c3b48a40b55537302304dea1dd5`. HarfBuzz dependency is now 15.1.0. zlib, bzip2, Brotli and PNG remain managed dependencies.

The previous library version remains available and is marked `supersededBy` in the BuildEngine XML catalog. The new version has a full producer, installation, publication and consumer contract.

**Verification boundary:** Source and patch compatibility checks do not establish a successful build. Native Windows BCC64X Release/Debug builds, upstream tests, installed consumer smoke, SBOM and Doxygen are **[not verified]** until the complete target-machine FSM has run.
