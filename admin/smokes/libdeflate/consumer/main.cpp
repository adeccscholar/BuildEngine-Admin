#include <libdeflate.h>
#include <array>
#include <cstring>
#include <print>
#include <string_view>

int main() {
   constexpr std::string_view svInput = "BuildEngine libdeflate 1.26 native DLL roundtrip";
   std::array<unsigned char, 256> aCompressed {};
   std::array<char, 256> aDecoded {};

   libdeflate_compressor* pCompressor = libdeflate_alloc_compressor(6);
   libdeflate_decompressor* pDecompressor = libdeflate_alloc_decompressor();
   bool bSuccess = pCompressor != nullptr && pDecompressor != nullptr;

   if(bSuccess) {
      std::size_t const uPacked = libdeflate_zlib_compress(
         pCompressor, svInput.data(), svInput.size(), aCompressed.data(), aCompressed.size());
      std::size_t uDecoded = 0;
      enum libdeflate_result const eResult = uPacked != 0
         ? libdeflate_zlib_decompress(pDecompressor, aCompressed.data(), uPacked,
               aDecoded.data(), aDecoded.size(), &uDecoded)
         : LIBDEFLATE_BAD_DATA;
      bSuccess = uPacked != 0 && eResult == LIBDEFLATE_SUCCESS &&
         uDecoded == svInput.size() &&
         std::memcmp(aDecoded.data(), svInput.data(), svInput.size()) == 0;
   }

   libdeflate_free_compressor(pCompressor);
   libdeflate_free_decompressor(pDecompressor);
   std::println("SMOKE|CHECK|zlib-roundtrip|{}|native shared compressor/decompressor",
      bSuccess ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|libdeflate installed consumer",
      bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
