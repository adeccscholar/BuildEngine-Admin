# harfbuzz 15.1.0 — BCC64X integration

Source pin `3df40d9b98d6b32f0e4288d4ecfbb158ddb3a372`; official release tar.xz SHA-256 `0fc7c565443a5f4be7da65ac5ee5bd737d9b158dc908708b7689a0a5796b0c8d`. Canonical DLL names are patched for upstream CMake. FreeType support remains OFF to avoid a dependency cycle.

The previous library version remains available and is marked `supersededBy` in the BuildEngine XML catalog. The new version has a full producer, installation, publication and consumer contract.

**Verification boundary:** Source and patch compatibility checks do not establish a successful build. Native Windows BCC64X Release/Debug builds, upstream tests, installed consumer smoke, SBOM and Doxygen are **[not verified]** until the complete target-machine FSM has run.
