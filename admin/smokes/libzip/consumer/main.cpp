#include <zip.h>

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
   char const* const szVersion = zip_libzip_version();
   bSuccess = Check("runtime", szVersion != nullptr && *szVersion != '\0', "libzip runtime version available") && bSuccess;
   zip_error_t theError;
   zip_error_init(&theError);
   bSuccess = Check("api", zip_error_code_zip(&theError) == ZIP_ER_OK, "zip_error API usable") && bSuccess;
   zip_error_fini(&theError);
   std::println("SMOKE|RESULT|{}|libzip consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
