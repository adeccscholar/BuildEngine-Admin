# DirectX-Headers 1.619.1 — native D3D12/DXCore integration

[TOC|Content]

## Purpose and package identity

DirectX-Headers provides modern Microsoft D3D12 and DXCore API declarations, associated native compatibility components and build-system exports. It is the **API foundation**, not a renderer, graphics driver or independent proof that a physical GPU was selected.

The authoritative source pin, direct dependencies, BCC64X build flags, installed package structure and consumer smoke are in `admin/build-libraries.xml`. The package is built exclusively with C++Builder 13/BCC64X and the managed toolchain. `-tR` remains mandatory for native executable and DLL products. The original upstream package and testing gates are preserved.

## Integration decisions and defects (22–28 November 2026)

The native build first revealed MSVC/WRL header assumptions, notably `wrl/client.h`, which is not part of the managed BCC64X header contract. The project uses explicit compatibility where necessary rather than silently injecting Visual Studio or unrelated Windows Kits headers. An earlier DXCore smoke produced an invalid success path: a stub returned success without an actual `IDXCoreAdapterFactory` pointer. The correct integration must call the real DXCore factory and validate its COM identity and usable pointer, not substitute a success-returning stub.

The distinction is particularly relevant to GPU selection. DXCore adapter discovery, device creation and `GL_RENDERER`/D3D12-device evidence must be recorded separately. Linking DirectX-Headers or generating a shader through DXC is never enough to claim that NVIDIA or Intel hardware executed work.

## Native consumer acceptance

The package contract defines native build and consumer checks. Successful library FSM completion across both the full workstation and the reduced Windows Server 2025 environment was **operator-reported** on 28 November 2026. This document does not independently rerun that acceptance. The future DirectX showrooms will carry additional native GPU/render acceptance checks.

## Showcase relationships

DirectX-Headers underpins D3D12-capable graphics experiments and may be combined with DirectXMath, DirectXMesh, DirectXTex, DXC, DirectXTK12 and WIL, each retaining its own independent library identity and runtime requirements. Mesa's experimental Gallium D3D12 OpenGL path is a separate layered implementation and must report its actual renderer.

[WRL compatibility details](/package/directx-headers/1.619.1/wrl-compatibility) · [Engineering retrospective](/news/2026-11-28/) · [Library catalog](/manual/libraries.md)
