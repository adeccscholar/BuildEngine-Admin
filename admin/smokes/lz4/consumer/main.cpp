#include <lz4.h>
#include <lz4frame.h>
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
   constexpr std::string_view svInput = "BuildEngine LZ4 native shared compression roundtrip";
   std::array<char, 256> aPacked {};
   std::array<char, 256> aPlain {};
   int const iPacked = LZ4_compress_default(svInput.data(), aPacked.data(),
      static_cast<int>(svInput.size()), static_cast<int>(aPacked.size()));
   bSuccess = Check("compress", iPacked > 0, "LZ4 block compressor") && bSuccess;
   int const iDecoded = iPacked > 0
      ? LZ4_decompress_safe(aPacked.data(), aPlain.data(), iPacked, static_cast<int>(aPlain.size()))
      : -1;
   bSuccess = Check("roundtrip", iDecoded == static_cast<int>(svInput.size()) &&
      std::memcmp(aPlain.data(), svInput.data(), svInput.size()) == 0,
      "LZ4 block decompress roundtrip") && bSuccess;

   std::array<char, 512> aFrame {};
   std::size_t const uFrame = LZ4F_compressFrame(
      aFrame.data(), aFrame.size(), svInput.data(), svInput.size(), nullptr);
   bSuccess = Check("frame", LZ4F_isError(uFrame) == 0U &&
      uFrame > 0U && uFrame <= aFrame.size(), "LZ4 frame encoder exported") && bSuccess;
   bSuccess = Check("version", LZ4_versionNumber() == LZ4_VERSION_NUMBER,
      "runtime and header version match") && bSuccess;
   std::println("SMOKE|RESULT|{}|LZ4 consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
