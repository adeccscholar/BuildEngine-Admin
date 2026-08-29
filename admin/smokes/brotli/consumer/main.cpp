#include <brotli/decode.h>
#include <brotli/encode.h>
#include <cstdint>

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
   constexpr std::string_view svText { "BuildEngine Brotli consumer smoke" };
   std::array<std::uint8_t, 256> aCompressed {};
   std::size_t uCompressed = aCompressed.size();
   BROTLI_BOOL const bEncoded = BrotliEncoderCompress(BROTLI_DEFAULT_QUALITY, BROTLI_DEFAULT_WINDOW,
      BROTLI_MODE_GENERIC, svText.size(), reinterpret_cast<std::uint8_t const*>(svText.data()),
      &uCompressed, aCompressed.data());
   bool const bCompressed = bEncoded == BROTLI_TRUE;
   bSuccess = Check("encode", bCompressed, "BrotliEncoderCompress succeeded") && bSuccess;
   std::array<std::uint8_t, 256> aPlain {};
   std::size_t uPlain = aPlain.size();
   bool bRoundtrip = false;
   if(bCompressed) {
      BrotliDecoderResult const theDecoded = BrotliDecoderDecompress(
         uCompressed, aCompressed.data(), &uPlain, aPlain.data());
      bRoundtrip = theDecoded == BROTLI_DECODER_RESULT_SUCCESS && uPlain == svText.size() &&
         std::memcmp(aPlain.data(), svText.data(), svText.size()) == 0;
   }
   bSuccess = Check("roundtrip", bRoundtrip, "Brotli encode/decode roundtrip") && bSuccess;
   std::println("SMOKE|RESULT|{}|Brotli consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
