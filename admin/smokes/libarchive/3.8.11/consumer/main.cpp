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
   int const iVersion = archive_version_number();
   bSuccess = Check("version", iVersion == ARCHIVE_VERSION_NUMBER && iVersion == 3008011,
      "installed headers and runtime are libarchive 3.8.11") && bSuccess;

   archive* const pReader = archive_read_new();
   bSuccess = Check("reader-create", pReader != nullptr, "archive_read_new succeeded") && bSuccess;
   if(pReader != nullptr) {
      bSuccess = Check("tar", archive_read_support_format_tar(pReader) == ARCHIVE_OK,
         "TAR format enabled") && bSuccess;
      bSuccess = Check("gzip", archive_read_support_filter_gzip(pReader) == ARCHIVE_OK,
         "in-process zlib/gzip reader enabled") && bSuccess;
      bSuccess = Check("lz4-reader", archive_read_support_filter_lz4(pReader) == ARCHIVE_OK,
         "in-process LZ4 reader enabled, not external fallback") && bSuccess;
      archive_read_free(pReader);
   }

   archive* const pWriter = archive_write_new();
   bSuccess = Check("writer-create", pWriter != nullptr, "archive_write_new succeeded") && bSuccess;
   if(pWriter != nullptr) {
      bSuccess = Check("lz4-writer", archive_write_add_filter_lz4(pWriter) == ARCHIVE_OK,
         "in-process LZ4 writer enabled, not external fallback") && bSuccess;
      archive_write_free(pWriter);
   }

   std::println("SMOKE|RESULT|{}|libarchive 3.8.11 zlib/LZ4 consumer",
      bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
