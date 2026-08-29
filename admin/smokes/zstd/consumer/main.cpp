#include <zstd.h>

#include <array>
#include <cstring>
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
   bSuccess = Check("runtime", ZSTD_versionNumber() != 0U, "ZSTD runtime version available") && bSuccess;
   constexpr std::string_view svText { "BuildEngine Zstd consumer smoke" };
   std::array<char, 256> aCompressed {};
   std::size_t const uCompressed = ZSTD_compress(aCompressed.data(), aCompressed.size(),
      svText.data(), svText.size(), 1);
   bool const bCompressed = ZSTD_isError(uCompressed) == 0U;
   bSuccess = Check("compress", bCompressed, "ZSTD_compress succeeded") && bSuccess;
   std::array<char, 256> aPlain {};
   std::size_t uPlain {};
   bool bRoundtrip = false;
   if(bCompressed) {
      uPlain = ZSTD_decompress(aPlain.data(), aPlain.size(), aCompressed.data(), uCompressed);
      bRoundtrip = ZSTD_isError(uPlain) == 0U && uPlain == svText.size() &&
         std::memcmp(aPlain.data(), svText.data(), svText.size()) == 0;
   }
   bSuccess = Check("roundtrip", bRoundtrip, "Zstd compress/decompress roundtrip") && bSuccess;
   std::println("SMOKE|RESULT|{}|Zstd consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
