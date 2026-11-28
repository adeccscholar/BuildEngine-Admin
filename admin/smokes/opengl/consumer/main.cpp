#include <windows.h>
#include <GL/gl.h>

#include <array>
#include <cstdint>
#include <filesystem>
#include <print>
#include <string>
#include <string_view>

namespace {

/**
\brief Returns the native filesystem path of a loaded module.
\details The executable and the managed Mesa modules must be app-local.
\param theModule Loaded module handle, or nullptr for the executable.
\returns Absolute normalized module path, or an empty path on failure.
\throw Nothing intentionally.
*/
[[nodiscard]] std::filesystem::path ModulePath(HMODULE const theModule) {
   std::array<wchar_t,32768> arrPath {};
   DWORD const uLength=::GetModuleFileNameW(theModule,arrPath.data(),
      static_cast<DWORD>(arrPath.size()));
   if(uLength==0U||uLength>=arrPath.size()) return {};
   return std::filesystem::path {
      std::wstring_view {arrPath.data(),uLength}
   }.lexically_normal();
}

/**
\brief Compares two Windows paths without depending on casing.
\details Only compares paths already resolved from executable/module handles.
\param theLeft First absolute path.
\param theRight Second absolute path.
\returns True when Windows ordinal path comparison succeeds.
\throw std::filesystem::filesystem_error If absolute resolution fails.
*/
[[nodiscard]] bool SamePath(std::filesystem::path const& theLeft,
   std::filesystem::path const& theRight) {
   std::wstring const strLeft=
      std::filesystem::absolute(theLeft).lexically_normal().wstring();
   std::wstring const strRight=
      std::filesystem::absolute(theRight).lexically_normal().wstring();
   return ::CompareStringOrdinal(strLeft.c_str(),-1,strRight.c_str(),-1,TRUE)
      ==CSTR_EQUAL;
}

/**
\brief Confirms the actual DLL has a required public entrypoint.
\details No WGL context, DXGI adapter or Vulkan ICD is created.
\param theModule The already loaded library.
\param szExport Export name encoded as a zero-terminated C string.
\returns True if the entrypoint exists.
\throw Nothing.
*/
[[nodiscard]] bool HasExport(HMODULE const theModule,char const* const szExport) noexcept {
   return theModule!=nullptr&&::GetProcAddress(theModule,szExport)!=nullptr;
}

}

/**
\brief Verifies published BCC64X Mesa binaries without requiring GPU hardware.
\details This smoke checks the compiled consumer, application-local DLLs and
   their entrypoints only. Runtime Zink/D3D12 device capabilities belong to
   optional, separately supervised desktop diagnostics. Do not call WGL,
   glGetString, DXGI, Vulkan or any graphics driver during this smoke.
\returns Zero when native DLL publication and export contracts are usable.
\throw Nothing intentionally.
*/
int main() {
   std::filesystem::path const theExecutable=ModulePath(nullptr);
   std::filesystem::path const theDirectory=theExecutable.parent_path();
   std::filesystem::path const theOpenGLPath=theDirectory/L"opengl32.dll";
   std::filesystem::path const theGalliumPath=theDirectory/L"libgallium_wgl.dll";

   // Link against the actual managed OpenGL import library without executing
   // any OpenGL operation. This volatile pointer preserves the PE import.
   auto volatile pLinkedGetError=&::glGetError;
   std::uintptr_t const uLinkedAddress=
      reinterpret_cast<std::uintptr_t>(pLinkedGetError);

   HMODULE const hOpenGL=::GetModuleHandleW(L"opengl32.dll");
   // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR restricts dependency resolution to the
   // staged module directory plus the OS DLL search policy, never a GPU SDK.
   // Load the exact published DLL, retaining Windows' normal dependency
   // resolution for the managed runtime closure staged beside this consumer.
   HMODULE const hGallium=::LoadLibraryW(theGalliumPath.c_str());

   bool const bOpenGLFile=std::filesystem::is_regular_file(theOpenGLPath);
   bool const bGalliumFile=std::filesystem::is_regular_file(theGalliumPath);
   bool const bOpenGLAppLocal=hOpenGL!=nullptr&&!theExecutable.empty()&&
      SamePath(ModulePath(hOpenGL),theOpenGLPath);
   bool const bGalliumAppLocal=hGallium!=nullptr&&!theExecutable.empty()&&
      SamePath(ModulePath(hGallium),theGalliumPath);
   bool const bImports=uLinkedAddress!=0U&&
      HasExport(hOpenGL,"glGetError")&&
      HasExport(hOpenGL,"glGetString")&&
      HasExport(hOpenGL,"wglCreateContext")&&
      HasExport(hOpenGL,"wglMakeCurrent");

   std::println("SMOKE|CHECK|opengl-package-file|{}",
      bOpenGLFile?"PASS":"FAIL");
   std::println("SMOKE|CHECK|gallium-package-file|{}",
      bGalliumFile?"PASS":"FAIL");
   std::println("SMOKE|CHECK|opengl-app-local-import|{}",
      bOpenGLAppLocal?"PASS":"FAIL");
   std::println("SMOKE|CHECK|gallium-app-local-load|{}|win32_error={}",
      bGalliumAppLocal?"PASS":"FAIL",
      bGalliumAppLocal?0UL:static_cast<unsigned long>(::GetLastError()));
   std::println("SMOKE|CHECK|opengl-link-and-exports|{}",
      bImports?"PASS":"FAIL");
   std::println("SMOKE|INFO|gpu-runtime|SKIPPED|"
      "hardware is not required to build or publish Mesa");

   bool const bSuccess=bOpenGLFile&&bGalliumFile&&
      bOpenGLAppLocal&&bGalliumAppLocal&&bImports;
   std::println("SMOKE|RESULT|{}|native Mesa package and imports",
      bSuccess?"PASS":"FAIL");

   if(hGallium) ::FreeLibrary(hGallium);
   return bSuccess?0:1;
}
