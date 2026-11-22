#include <GDeflate.h>
#include <array>
#include <cstdint>
#include <print>
#include <vector>
#include <algorithm>

int main() {
   std::array<std::uint8_t, 131072> arrInput {};
   for(std::size_t i = 0; i < arrInput.size(); ++i) {
      arrInput[i] = static_cast<std::uint8_t>((i / 17) % 251);
   }
   std::vector<std::uint8_t> vecCompressed(GDeflate::CompressBound(arrInput.size()));
   std::size_t uCompressedSize = vecCompressed.size();
   bool const bCompressed = GDeflate::Compress(vecCompressed.data(), &uCompressedSize,
      arrInput.data(), arrInput.size(), GDeflate::MinimumCompressionLevel,
      GDeflate::COMPRESS_SINGLE_THREAD);
   std::array<std::uint8_t, 131072> arrResult {};
   bool const bDecompressed = bCompressed && GDeflate::Decompress(arrResult.data(),
      arrResult.size(), vecCompressed.data(), uCompressedSize, 1u);
   bool const bOK = bDecompressed && arrResult == arrInput;
   std::println("SMOKE|CHECK|gdeflate-roundtrip|{}|Native 128 KiB lossless data roundtrip",
      bOK ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|DirectStorage open GDeflate reference",
      bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
