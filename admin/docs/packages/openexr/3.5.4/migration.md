# OpenEXR 3.5.4: BCC64X integration

Status: registered, native build and consumer smoke pending (21 November 2026).

External dependencies: Imath 3.2.3, OpenJPH 0.33.0, libdeflate 1.26. The initial build disables tools, examples, Python and upstream tests. OpenEXR's internal-dependency forcing options are disabled and explicit external package paths are supplied.

Warning: disabling FORCE_INTERNAL does not prevent an automatic vendored fallback when an external library is not discovered. A native configure audit must verify the resulting CMake targets. OpenEXR 3.5.4 has specific handling for OpenJPH 0.33.0 package-version and Threads behavior. Validate DLL imports, installed CMake consumption, HDR round-trip, adversarial image inputs and SBOM evidence.
