# TODO – Boost Version Upgrade and Discovery

## Sequence

This item is implemented **only after completion of the current Boost 1.92.0 proof and after integration of the other planned third-party libraries**.

Current priority:

1. Complete Boost 1.92.0 through build, install, publish, and all Release/Debug evidence gates.
2. Add further libraries to BuildEngine and verify each through the same reproducible package/consumer contract.
3. Then automate Boost version discovery and upgrade preflight.

## Goal

Moving to a new Boost version must not be done by blindly copying the 1.92.0 profile. BuildEngine should inventory the new upstream version and make differences from the last accepted Boost profile visible.

## Planned upgrade preflight

For a new Boost version:

- pin the official upstream artifact and SHA-256 again;
- re-inventory logical Boost libraries and CMake source modules from the upstream metadata structure;
- generate a diff against the last accepted profile: added, removed, renamed, metadata/target changes;
- reassess external ecosystem dependencies such as OpenCL, MPI, and Python;
- redetermine or verify binary component families and CMake target types;
- make changes between SHARED/STATIC/INTERFACE targets visible in particular;
- rerun the Boost.Config/BCC64X native-Clang preflight without historical assumptions;
- check whether the local Boost.Config adapter is still required or upstream now classifies BCC64X correctly itself;
- create/accept the new canonical component contract only after a successful preflight;
- then execute the complete Release/Debug production graph and all component/runtime gates.

## Guiding principle

The generic Boost engine remains version-independent. Only the accepted component profile, expected artifacts/target types, and documented external exclusions are version-bound.

A new version therefore must not be considered fully supported merely because the old 1.92.0 component set still builds.
