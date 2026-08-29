#include <lzma.h>
#include <cstdint>

#include <limits>
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
   bSuccess = Check("runtime", lzma_version_number() != 0U, "liblzma runtime version available") && bSuccess;
   lzma_stream theStream = LZMA_STREAM_INIT;
   lzma_ret const theResult = lzma_stream_decoder(&theStream, std::numeric_limits<std::uint64_t>::max(), 0U);
   bSuccess = Check("decoder", theResult == LZMA_OK, "lzma_stream_decoder initialized") && bSuccess;
   if(theResult == LZMA_OK) lzma_end(&theStream);
   std::println("SMOKE|RESULT|{}|XZ/liblzma consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
