#include <windows.h>
#include <GL/gl.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <print>
#include <string>
#include <string_view>

namespace {

[[nodiscard]] std::filesystem::path ModulePath(HMODULE const theModule) {
   std::array<wchar_t, 32768> arrPath {};
   DWORD const uLength = ::GetModuleFileNameW(theModule, arrPath.data(), static_cast<DWORD>(arrPath.size()));
   if(uLength == 0U || uLength >= arrPath.size()) return {};
   return std::filesystem::path { std::wstring_view { arrPath.data(), uLength } }.lexically_normal();
}

[[nodiscard]] bool SamePath(std::filesystem::path const& theLeft, std::filesystem::path const& theRight) {
   std::wstring const strLeft = std::filesystem::absolute(theLeft).lexically_normal().wstring();
   std::wstring const strRight = std::filesystem::absolute(theRight).lexically_normal().wstring();
   return ::CompareStringOrdinal(strLeft.c_str(), -1, strRight.c_str(), -1, TRUE) == CSTR_EQUAL;
}

[[nodiscard]] bool ContainsInsensitive(std::string strText, std::string_view const svNeedle) {
   std::ranges::transform(strText, strText.begin(),
      [](unsigned char const chValue) { return static_cast<char>(std::tolower(chValue)); });
   std::string strNeedle { svNeedle };
   std::ranges::transform(strNeedle, strNeedle.begin(),
      [](unsigned char const chValue) { return static_cast<char>(std::tolower(chValue)); });
   return strText.find(strNeedle) != std::string::npos;
}

}

int main() {
   HMODULE const theOpenGLModule = ::GetModuleHandleW(L"opengl32.dll");
   std::filesystem::path const theExecutable = ModulePath(nullptr);
   std::filesystem::path const theOpenGLPath = ModulePath(theOpenGLModule);
   bool const bAppLocalOpenGL = theOpenGLModule != nullptr && !theExecutable.empty() && !theOpenGLPath.empty() &&
      SamePath(theExecutable.parent_path(), theOpenGLPath.parent_path());

   HINSTANCE const theInstance = ::GetModuleHandleW(nullptr);
   wchar_t const* const szClassName = L"BuildEngineMesaOpenGLSmoke";
   WNDCLASSW const theClass {
      .style = CS_OWNDC,
      .lpfnWndProc = ::DefWindowProcW,
      .hInstance = theInstance,
      .lpszClassName = szClassName
   };
   ATOM const theClassAtom = ::RegisterClassW(&theClass);
   HWND const theWindow = theClassAtom == 0U ? nullptr :
      ::CreateWindowExW(0U, szClassName, L"BuildEngine Mesa OpenGL smoke", WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 64, 64, nullptr, nullptr, theInstance, nullptr);
   HDC const theDeviceContext = theWindow == nullptr ? nullptr : ::GetDC(theWindow);

   PIXELFORMATDESCRIPTOR thePixelFormatDescriptor {
      .nSize = sizeof(PIXELFORMATDESCRIPTOR),
      .nVersion = 1,
      .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      .iPixelType = PFD_TYPE_RGBA,
      .cColorBits = 24,
      .cDepthBits = 24,
      .iLayerType = PFD_MAIN_PLANE
   };

   int const iPixelFormat = theDeviceContext == nullptr ? 0 :
      ::ChoosePixelFormat(theDeviceContext, &thePixelFormatDescriptor);
   bool const bPixelFormat = iPixelFormat != 0 &&
      ::SetPixelFormat(theDeviceContext, iPixelFormat, &thePixelFormatDescriptor) != FALSE;
   HGLRC const theContext = bPixelFormat ? ::wglCreateContext(theDeviceContext) : nullptr;
   bool const bContext = theContext != nullptr && ::wglMakeCurrent(theDeviceContext, theContext) != FALSE;

   GLubyte const* const pVendor = bContext ? ::glGetString(GL_VENDOR) : nullptr;
   GLubyte const* const pRenderer = bContext ? ::glGetString(GL_RENDERER) : nullptr;
   std::string const strVendor = pVendor == nullptr ? std::string {} :
      reinterpret_cast<char const*>(pVendor);
   std::string const strRenderer = pRenderer == nullptr ? std::string {} :
      reinterpret_cast<char const*>(pRenderer);

   HMODULE const theGalliumModule = ::GetModuleHandleW(L"libgallium_wgl.dll");
   std::filesystem::path const theGalliumPath = ModulePath(theGalliumModule);
   bool const bAppLocalGallium = theGalliumModule != nullptr && !theExecutable.empty() && !theGalliumPath.empty() &&
      SamePath(theExecutable.parent_path(), theGalliumPath.parent_path());
   // The package builds both Mesa softpipe and the D3D12 Gallium driver.
   // Validate that the active renderer belongs to one of those configured
   // backends, rather than requiring the software fallback on every host.
   bool const bSoftpipe = ContainsInsensitive(strRenderer, "softpipe");
   bool const bD3D12 = ContainsInsensitive(strRenderer, "d3d12");
   bool const bConfiguredRenderer = !strRenderer.empty() && (bSoftpipe || bD3D12);

   // GL_VENDOR identifies the OpenGL implementation (often Mesa), not
   // necessarily the underlying GPU. Use the D3D12 renderer's adapter name
   // for informational vendor classification without affecting compatibility.
   std::string_view svAdapterVendor = "unknown";
   if(bSoftpipe) {
      svAdapterVendor = "software";
   }
   else if(bD3D12) {
      if(ContainsInsensitive(strRenderer, "nvidia") || ContainsInsensitive(strRenderer, "geforce") ||
         ContainsInsensitive(strRenderer, "quadro") || ContainsInsensitive(strRenderer, "rtx")) {
         svAdapterVendor = "NVIDIA";
      }
      else if(ContainsInsensitive(strRenderer, "amd") || ContainsInsensitive(strRenderer, "radeon") ||
              ContainsInsensitive(strRenderer, "advanced micro devices")) {
         svAdapterVendor = "AMD";
      }
      else if(ContainsInsensitive(strRenderer, "intel") || ContainsInsensitive(strRenderer, "iris") ||
              ContainsInsensitive(strRenderer, "arc graphics") || ContainsInsensitive(strRenderer, "uhd graphics")) {
         svAdapterVendor = "Intel";
      }
      else if(ContainsInsensitive(strRenderer, "qualcomm") || ContainsInsensitive(strRenderer, "adreno")) {
         svAdapterVendor = "Qualcomm";
      }
      else if(ContainsInsensitive(strRenderer, "microsoft") || ContainsInsensitive(strRenderer, "warp")) {
         svAdapterVendor = "Microsoft software adapter";
      }
      else {
         svAdapterVendor = "other D3D12 adapter";
      }
   }

   GLubyte const* const pVersion = bContext ? ::glGetString(GL_VERSION) : nullptr;
   GLubyte const* const pShadingLanguage = bContext ? ::glGetString(GL_SHADING_LANGUAGE_VERSION) : nullptr;
   std::string const strVersion = pVersion == nullptr ? std::string {} : reinterpret_cast<char const*>(pVersion);
   std::string const strShadingLanguage = pShadingLanguage == nullptr ? std::string {} :
      reinterpret_cast<char const*>(pShadingLanguage);

   std::println("SMOKE|CHECK|opengl-module|{}|app-local Mesa opengl32.dll loaded",
                bAppLocalOpenGL ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|wgl-context|{}|WGL context creation {}",
                bContext ? "PASS" : "FAIL", bContext ? "succeeded" : "failed");
   std::println("SMOKE|CHECK|gallium-module|{}|app-local libgallium_wgl.dll loaded",
                bAppLocalGallium ? "PASS" : "FAIL");
   std::println("SMOKE|CHECK|renderer|{}|vendor='{}', renderer='{}'",
                bConfiguredRenderer ? "PASS" : "FAIL", strVendor, strRenderer);
   std::println("SMOKE|INFO|gpu|backend='{}', adapter-vendor='{}', renderer='{}'",
                bD3D12 ? "D3D12" : (bSoftpipe ? "softpipe" : "unknown"),
                svAdapterVendor, strRenderer);
   std::println("SMOKE|INFO|api|opengl='{}', glsl='{}'", strVersion, strShadingLanguage);

   bool const bSuccess = bAppLocalOpenGL && bContext && bAppLocalGallium && bConfiguredRenderer;
   std::println("SMOKE|RESULT|{}|Mesa OpenGL configured Gallium consumer usable", bSuccess ? "PASS" : "FAIL");

   if(bContext) ::wglMakeCurrent(nullptr, nullptr);
   if(theContext != nullptr) ::wglDeleteContext(theContext);
   if(theDeviceContext != nullptr && theWindow != nullptr) ::ReleaseDC(theWindow, theDeviceContext);
   if(theWindow != nullptr) ::DestroyWindow(theWindow);
   if(theClassAtom != 0U) ::UnregisterClassW(szClassName, theInstance);

   return bSuccess ? 0 : 1;
}
