# Native BCC64X WRL compatibility for DirectX-Headers 1.619.1

**Status:** [not verified] until native BCC64X Release/Debug build, install and original upstream CTest execution.

## Rationale

The Windows SDK 10.0.26100.0 WRL implementation rejects BCC64X at `winrt/wrl/def.h:33` with `WRL requires compiler version 16.00 or greater`. Do not impersonate MSVC with `_MSC_VER` and do not disable upstream tests.

The exact Microsoft DirectX-Headers tag `v1.619.1` includes MIT-licensed sources:

- `include/wsl/stubs/wrl/client.h`: original forwarding header, includes `wsl/wrladapter.h`;
- `include/wsl/wrladapter.h`: Microsoft's `Microsoft::WRL::ComPtr` implementation;
- `include/wsl/winadapter.h`: includes native `unknwn.h`.

The compatibility adapter `admin/cmake/directx-headers/WindowsSdkWinRT.cmake` installs a copy of the upstream forwarding header into the build tree's separate include root and places that root before the Windows SDK for the original CTest targets. It also installs `include/wrl/client.h` in the versioned DirectX-Headers package. The upstream install already copies `include/wsl/`.

No other WSL stubs are added to the native Windows include path. Win32 types and COM interfaces still come from the real native Windows SDK. The selected WRL class implementation is the pinned Microsoft MIT source, not SDK WRL. This is not a claim of general SDK independence.

## Verification requirements

1. Re-run the actual original upstream `DirectX-Headers-Test`, `DirectX-Headers-Check-Feature-Support-Test`, and `Feature-Support-Test` for both configurations with BCC64X.
2. Inspect the compiler's include-resolution trace to ensure `wrl/client.h` comes from `adecc-microsoft-wrl-compat`, while `unknwn.h` comes from the native Windows toolchain.
3. Check that the versioned installed package contains `include/wrl/client.h`, `include/wsl/wrladapter.h`, `include/wsl/winadapter.h`, all original DirectX headers and unchanged upstream tests.
4. Run an independent consumer from the installed package (not the source tree), including basic `ComPtr` AddRef/Release and D3D12 state-object helper compilation.
5. Follow with DirectXMesh, DirectXTex and DirectXTK12 consumers. Do not promote unsupported WRL facilities (e.g. weak-reference semantics) as verified.

**Static evidence:** the pinned Microsoft sources and original CMake target structure were inspected. A separate CMake 3.31 mock check of include overlay and package install succeeded, but it is not a native BCC64X verification.

## Git discipline

The original `test-CMakeLists.txt` and `googletest-CMakeLists.txt` are unchanged by this update. The patch is only in the common CMake adapter and this documentation. Git's clean-worktree/fast-forward contract remains authoritative. Never run `git reset --hard`, `git clean` or disable dirty checking as a response to line-ending changes.


## Native BCC64X GUID deduction follow-up

The later native attempt passed the Windows SDK WRL compiler-version guard using the MIT compatibility header and reached `feature_check_test.cpp:696`. BCC64X then rejected `IID_PPV_ARGS(&adapter_factory)`: the Embarcadero `combaseapi.h` defines `_CG_GUID_PPT_Helper(T**)`, which does not accept the `Microsoft::WRL::Details::ComPtrRef<T>` proxy returned by `ComPtr::operator&`.

The isolated compatibility `wrl/client.h` now adds exactly one type-safe template overload of `_CG_GUID_PPT_Helper` for that Microsoft proxy, deriving the COM IID via `__uuidof(**aPtr)`. Microsoft's original forwarder and adapter source are retained, and the same adapted header is installed with the package. No `_MSC_VER` pretense, SDK modifications, feature removal or test-target disabling is involved.

**[not verified]**: BCC64X compilation and CTest results after this follow-up.

## HRESULT diagnostics for original native tests

The next native run compiled all original targets in both Release and Debug. `Feature-Support-Test` passed, but the two GPU-dependent original runtime tests failed without a printed HRESULT. This is insufficient evidence to attribute the failure to the WRL compatibility layer, GPU availability, or COM.

A version-bound idempotent source diagnostic stage now adds `stderr` HRESULT reporting at the existing `DXCoreCreateAdapterFactory`, `CreateAdapterList`, `GetAdapter` and `D3D12CreateDevice` failure paths in `test/test.cpp`, and at both initial DXCore failure paths in `test/feature_check_test.cpp`. The instrumentation does not change the test names, return codes, conditions or upstream test suite; it modifies only isolated extracted test-source copies, never the installed SDK. DirectX-Headers version timestamp was refreshed to ensure source execution.

The next CTest run should report a named failing API plus hexadecimal HRESULT if one of the instrumented branches fails. A crash before any instrumented branch remains possible and would require a debugger or dump. **[not verified]**.

## Native test entry confirmed, checkpoint instrumentation refined

The 22 November 2026 verbose CTest output confirms both GPU-dependent original executables enter `main()`, and all 74 cases of the independent GoogleTest `Feature-Support-Test` pass. The first GPU runtime test exits unsuccessfully and the second can crash with SegFault. Neither produces a failing HRESULT from the existing failure-only diagnostics. The cause is **not established** by these logs.

To isolate the location without changing assertions or disabling a test, the versioned diagnostic script now prints flushed checkpoints before and after factory creation, before adapter-list creation and adapter access, and before D3D12 device creation. The next full verbose CTest log should expose the last completed checkpoint even if the process crashes inside an API call. A native debugger/Minidump may be needed after that. **[not verified]** until rerun.
