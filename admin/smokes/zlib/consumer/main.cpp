#include <zlib.h>

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
   char const* const szVersion = zlibVersion();
   bSuccess = Check("runtime", szVersion != nullptr && *szVersion != '\0', "zlibVersion available") && bSuccess;
   constexpr std::string_view svText { "BuildEngine zlib consumer smoke" };
   std::array<unsigned char, 256> aCompressed {};
   uLongf uCompressed = aCompressed.size();
   int const iCompressed = compress2(aCompressed.data(), &uCompressed,
      reinterpret_cast<Bytef const*>(svText.data()), static_cast<uLong>(svText.size()), Z_BEST_SPEED);
   bool const bCompressed = iCompressed == Z_OK;
   bSuccess = Check("compress", bCompressed, "compress2 succeeded") && bSuccess;
   std::array<unsigned char, 256> aPlain {};
   uLongf uPlain = aPlain.size();
   bool bRoundtrip = false;
   if(bCompressed) {
      int const iPlain = uncompress(aPlain.data(), &uPlain, aCompressed.data(), uCompressed);
      bRoundtrip = iPlain == Z_OK && uPlain == svText.size() &&
         std::memcmp(aPlain.data(), svText.data(), svText.size()) == 0;
   }
   bSuccess = Check("roundtrip", bRoundtrip, "compress/uncompress roundtrip") && bSuccess;
   std::println("SMOKE|RESULT|{}|zlib consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
