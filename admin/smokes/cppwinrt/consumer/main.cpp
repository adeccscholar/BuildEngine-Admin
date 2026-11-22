#include <filesystem>
#include <print>
int main() {
   std::filesystem::path const thePackage {CPPWINRT_PACKAGE_ROOT};
   bool const bOK = std::filesystem::exists(thePackage / "bin" / "win64" / "Release" / "cppwinrt.exe");
   std::println("SMOKE|CHECK|cppwinrt-generator|{}|Native generator exists", bOK ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|C++/WinRT", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
