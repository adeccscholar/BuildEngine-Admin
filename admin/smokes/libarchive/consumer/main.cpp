#include <archive.h>

#include <print>
#include <string_view>

namespace {
bool Check(std::string_view const svId, bool const bPassed, std::string_view const svDetail) {
   std::println("SMOKE|CHECK|{}|{}|{}", svId, bPassed ? "PASS" : "FAIL", svDetail);
   return bPassed;
}
}

int main() {
   bool bSuccess = true;
   bSuccess = Check("runtime", archive_version_number() > 0, "libarchive runtime version available") && bSuccess;
   archive* const pArchive = archive_read_new();
   bSuccess = Check("create", pArchive != nullptr, "archive_read_new succeeded") && bSuccess;
   if(pArchive != nullptr) {
      int const iSupport = archive_read_support_format_tar(pArchive);
      bSuccess = Check("tar", iSupport == ARCHIVE_OK, "TAR reader enabled") && bSuccess;
      archive_read_free(pArchive);
   }
   std::println("SMOKE|RESULT|{}|libarchive consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
