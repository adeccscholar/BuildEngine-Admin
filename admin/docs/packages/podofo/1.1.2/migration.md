# podofo 1.1.2 — BCC64X integration

Source pin `e91d8b727763724f3efd96c589c8033f4333c68c`. Patch chain has nine rebased BCC64X source patches. `TestUtils` interface differences and DLL-only XMP test helpers have been ported. The `extern/resources` submodule is pinned to `f3e6e289db3cb7fe0a1a21e65430f221fe598882` and FreeType 2.15.0 is a direct dependency.

The previous library version remains available and is marked `supersededBy` in the BuildEngine XML catalog. The new version has a full producer, installation, publication and consumer contract.

**Verification boundary:** Source and patch compatibility checks do not establish a successful build. Native Windows BCC64X Release/Debug builds, upstream tests, installed consumer smoke, SBOM and Doxygen are **[not verified]** until the complete target-machine FSM has run.
