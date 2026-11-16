#include <print>
#include <string>
#define WIN_ENV 1
#define XMP_DynamicBuild 1
#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1
#include "XMP.hpp"
#include "XMP.incl_cpp"

int main() {
   bool const bMetaInitialized = SXMPMeta::Initialize();
   std::println("SMOKE|CHECK|meta-init|{}|XMPCore initialization",
                bMetaInitialized ? "PASS" : "FAIL");
   if (!bMetaInitialized) {
      std::println("SMOKE|RESULT|FAIL|XMP Toolkit DLL consumer");
      return 1;
   }

   bool const bFilesInitialized = SXMPFiles::Initialize();
   std::println("SMOKE|CHECK|files-init|{}|XMPFiles initialization",
                bFilesInitialized ? "PASS" : "FAIL");
   if (!bFilesInitialized) {
      SXMPMeta::Terminate();
      std::println("SMOKE|RESULT|FAIL|XMP Toolkit DLL consumer");
      return 3;
   }

   SXMPMeta theMetadata;
   theMetadata.SetProperty("http://purl.org/dc/elements/1.1/", "title", "BuildEngine XMP consumer");
   std::string strValue;
   bool const bRoundTrip =
      theMetadata.GetProperty("http://purl.org/dc/elements/1.1/", "title", &strValue, nullptr) &&
      strValue == "BuildEngine XMP consumer";
   std::println("SMOKE|CHECK|metadata-roundtrip|{}|XMP property round-trip",
                bRoundTrip ? "PASS" : "FAIL");

   SXMPFiles::Terminate();
   SXMPMeta::Terminate();
   std::println("SMOKE|RESULT|{}|XMP Toolkit DLL consumer", bRoundTrip ? "PASS" : "FAIL");
   return bRoundTrip ? 0 : 2;
}
