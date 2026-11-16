#include <string>
#define WIN_ENV 1
#define XMP_DynamicBuild 1
#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1
#include "XMP.hpp"
#include "XMP.incl_cpp"

int main() {
   if (!SXMPMeta::Initialize()) {
      return 1;
   }
   if (!SXMPFiles::Initialize()) {
      SXMPMeta::Terminate();
      return 3;
   }
   SXMPMeta theMetadata;
   theMetadata.SetProperty("http://purl.org/dc/elements/1.1/", "title", "BuildEngine XMP consumer");
   std::string strValue;
   if (!theMetadata.GetProperty("http://purl.org/dc/elements/1.1/", "title", &strValue, nullptr) ||
       strValue != "BuildEngine XMP consumer") {
      SXMPFiles::Terminate();
      SXMPMeta::Terminate();
      return 2;
   }
   SXMPFiles::Terminate();
   SXMPMeta::Terminate();
   return 0;
}
