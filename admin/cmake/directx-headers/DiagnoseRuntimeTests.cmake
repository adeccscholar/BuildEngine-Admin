# SPDX-License-Identifier: MIT
# Version-bound runtime diagnostics for Microsoft DirectX-Headers v1.619.1.
# Preserve original upstream test flow and exit codes.
cmake_minimum_required(VERSION 3.31)
if(NOT DEFINED SOURCE_ROOT OR NOT IS_DIRECTORY "${SOURCE_ROOT}/test")
   message(FATAL_ERROR "SOURCE_ROOT must contain the original DirectX-Headers tests")
endif()
function(adecc_replace_once strFile strBefore strAfter)
   file(READ "${strFile}" strOriginal)
   # The Microsoft test.cpp file is CRLF in the pinned archive.
   # Normalize line endings in the isolated versioned build workspace only.
   string(REPLACE "\r\n" "\n" strOriginal "${strOriginal}")
   string(FIND "${strOriginal}" "${strAfter}" iAlready)
   if(NOT iAlready EQUAL -1)
      return()
   endif()
   string(FIND "${strOriginal}" "${strBefore}" iFound)
   if(iFound EQUAL -1)
      message(FATAL_ERROR "Unexpected upstream DirectX-Headers test source: ${strFile}; pattern: ${strBefore}")
   endif()
   string(REPLACE "${strBefore}" "${strAfter}" strModified "${strOriginal}")
   file(WRITE "${strFile}" "${strModified}")
endfunction()
set(strSimple "${SOURCE_ROOT}/test/test.cpp")
set(strFeatures "${SOURCE_ROOT}/test/feature_check_test.cpp")
adecc_replace_once("${strSimple}"
   "#include <wsl/winadapter.h>"
   "#include <wsl/winadapter.h>\n#include <cstdio> // BuildEngine: report the failing native DirectX API HRESULT")
adecc_replace_once("${strSimple}"
   "int main()\n{"
   "int main()\n{\n    std::fprintf(stderr, \"DirectX-Headers-Test: entered main\\n\");\n    std::fflush(stderr);")
adecc_replace_once("${strFeatures}"
   "int main()\n{"
   "int main()\n{\n    std::cerr << \"DirectX-Headers-Check-Feature-Support-Test: entered main\" << std::endl;")
adecc_replace_once("${strSimple}"
   "if (FAILED(DXCoreCreateAdapterFactory(&factory)))\n            return -1;"
   "if (HRESULT const hr = DXCoreCreateAdapterFactory(&factory); FAILED(hr)) {\n            std::fprintf(stderr, \"DXCoreCreateAdapterFactory: HRESULT=0x%08lX\\n\", static_cast<unsigned long>(hr));\n            return -1;\n        }")
adecc_replace_once("${strSimple}"
   "if (FAILED(factory->CreateAdapterList(1, &DXCORE_ADAPTER_ATTRIBUTE_D3D12_CORE_COMPUTE, &list)))\n            return -1;"
   "if (HRESULT const hr = factory->CreateAdapterList(1, &DXCORE_ADAPTER_ATTRIBUTE_D3D12_CORE_COMPUTE, &list); FAILED(hr)) {\n            std::fprintf(stderr, \"IDXCoreAdapterFactory::CreateAdapterList: HRESULT=0x%08lX\\n\", static_cast<unsigned long>(hr));\n            return -1;\n        }")
adecc_replace_once("${strSimple}"
   "if (FAILED(list->GetAdapter(0, &adapter)))\n            return -1;"
   "if (HRESULT const hr = list->GetAdapter(0, &adapter); FAILED(hr)) {\n            std::fprintf(stderr, \"IDXCoreAdapterList::GetAdapter: HRESULT=0x%08lX\\n\", static_cast<unsigned long>(hr));\n            return -1;\n        }")
adecc_replace_once("${strSimple}"
   "return D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));"
   "HRESULT const hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));\n    if (FAILED(hr)) {\n        std::fprintf(stderr, \"D3D12CreateDevice: HRESULT=0x%08lX\\n\", static_cast<unsigned long>(hr));\n    }\n    return hr;")
adecc_replace_once("${strFeatures}"
   "if(FAILED(DXCoreCreateAdapterFactory(IID_PPV_ARGS(&adapter_factory))))\n    {\n        return -1;\n    }"
   "if(HRESULT const hr = DXCoreCreateAdapterFactory(IID_PPV_ARGS(&adapter_factory)); FAILED(hr))\n    {\n        std::cerr << \"DXCoreCreateAdapterFactory: HRESULT=0x\" << std::hex << static_cast<unsigned long>(hr) << std::endl;\n        return -1;\n    }")
adecc_replace_once("${strFeatures}"
   "if(FAILED(adapter_factory->CreateAdapterList(_countof(dx_must_attr), dx_must_attr, IID_PPV_ARGS(&adapter_list))))\n    {\n        return -1;\n    }"
   "if(HRESULT const hr = adapter_factory->CreateAdapterList(_countof(dx_must_attr), dx_must_attr, IID_PPV_ARGS(&adapter_list)); FAILED(hr))\n    {\n        std::cerr << \"IDXCoreAdapterFactory::CreateAdapterList: HRESULT=0x\" << std::hex << static_cast<unsigned long>(hr) << std::endl;\n        return -1;\n    }")
# Checkpoints before the calls, not only in their FAILED() branches:
# a crash during COM argument preparation or DXCore factory entry must be visible.
adecc_replace_once("${strSimple}"
   "    check_uuid_linkage();"
   "    check_uuid_linkage();\n    std::fprintf(stderr, \"DirectX-Headers-Test: UUID linkage verified\\n\");\n    std::fflush(stderr);")
adecc_replace_once("${strSimple}"
   "        if (HRESULT const hr = DXCoreCreateAdapterFactory(&factory); FAILED(hr))"
   "        std::fprintf(stderr, \"DirectX-Headers-Test: entering DXCoreCreateAdapterFactory\\n\");\n        std::fflush(stderr);\n        if (HRESULT const hr = DXCoreCreateAdapterFactory(&factory); FAILED(hr))")
adecc_replace_once("${strSimple}"
   "        IDXCoreAdapterList *list = nullptr;"
   "        std::fprintf(stderr, \"DirectX-Headers-Test: factory created\\n\");\n        std::fflush(stderr);\n        IDXCoreAdapterList *list = nullptr;")
adecc_replace_once("${strSimple}"
   "        if (HRESULT const hr = factory->CreateAdapterList"
   "        std::fprintf(stderr, \"DirectX-Headers-Test: entering CreateAdapterList\\n\");\n        std::fflush(stderr);\n        if (HRESULT const hr = factory->CreateAdapterList")
adecc_replace_once("${strSimple}"
   "        if (HRESULT const hr = list->GetAdapter(0, &adapter); FAILED(hr))"
   "        std::fprintf(stderr, \"DirectX-Headers-Test: entering GetAdapter\\n\");\n        std::fflush(stderr);\n        if (HRESULT const hr = list->GetAdapter(0, &adapter); FAILED(hr))")
adecc_replace_once("${strSimple}"
   "    HRESULT const hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));"
   "    std::fprintf(stderr, \"DirectX-Headers-Test: entering D3D12CreateDevice\\n\");\n    std::fflush(stderr);\n    HRESULT const hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));")
adecc_replace_once("${strFeatures}"
   "    if(HRESULT const hr = DXCoreCreateAdapterFactory(IID_PPV_ARGS(&adapter_factory)); FAILED(hr))"
   "    std::cerr << \"Feature-Support-Test: entering DXCoreCreateAdapterFactory\" << std::endl;\n    if(HRESULT const hr = DXCoreCreateAdapterFactory(IID_PPV_ARGS(&adapter_factory)); FAILED(hr))")
adecc_replace_once("${strFeatures}"
   "    if(HRESULT const hr = adapter_factory->CreateAdapterList"
   "    std::cerr << \"Feature-Support-Test: factory created, entering CreateAdapterList\" << std::endl;\n    if(HRESULT const hr = adapter_factory->CreateAdapterList")
adecc_replace_once("${strFeatures}"
   "    for (uint32_t i = 0; i < adapter_list->GetAdapterCount(); i++)"
   "    std::cerr << \"Feature-Support-Test: adapter list created\" << std::endl;\n    for (uint32_t i = 0; i < adapter_list->GetAdapterCount(); i++)")
# Factory pointer/vtable snapshots and actual loaded DLL paths before unsafe calls.
# These diagnostics do not invoke additional COM methods or suppress failures.
adecc_replace_once("${strSimple}"
   "#include <cstdio> // BuildEngine: report the failing native DirectX API HRESULT"
   "#include <cstdio> // BuildEngine: report the failing native DirectX API HRESULT\n#include \"adecc_dxcore_diagnostics.h\"")
adecc_replace_once("${strFeatures}"
   "#include <wsl/winadapter.h>"
   "#include <wsl/winadapter.h>\n#include \"adecc_dxcore_diagnostics.h\"")
adecc_replace_once("${strSimple}"
   "        std::fprintf(stderr, \"DirectX-Headers-Test: entering CreateAdapterList\\n\");"
   "        adecc::dxcore_diagnostics::PrintStage(\"before CreateAdapterList\", factory);\n        std::fprintf(stderr, \"DirectX-Headers-Test: entering CreateAdapterList\\n\");")
adecc_replace_once("${strFeatures}"
   "    std::cerr << \"Feature-Support-Test: factory created, entering CreateAdapterList\" << std::endl;"
   "    adecc::dxcore_diagnostics::PrintStage(\"before CreateAdapterList\", adapter_factory.Get());\n    std::cerr << \"Feature-Support-Test: factory created, entering CreateAdapterList\" << std::endl;")
message(STATUS "Applied idempotent DirectX-Headers runtime HRESULT diagnostics")
