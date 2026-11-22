#include <libdeflate.h>
#include <stdio.h>
int main(void) {
   struct libdeflate_gdeflate_compressor* pCompressor = libdeflate_alloc_gdeflate_compressor(6);
   struct libdeflate_gdeflate_decompressor* pDecompressor = libdeflate_alloc_gdeflate_decompressor();
   int const iOK = pCompressor != NULL && pDecompressor != NULL;
   if(pCompressor) {
      libdeflate_free_gdeflate_compressor(pCompressor);
   }
   if(pDecompressor) {
      libdeflate_free_gdeflate_decompressor(pDecompressor);
   }
   printf("SMOKE|CHECK|gdeflate-fork-api|%s|GDeflate allocation and disposal\n", iOK ? "PASS" : "FAIL");
   printf("SMOKE|RESULT|%s|Managed NVIDIA GDeflate-capable libdeflate\n", iOK ? "PASS" : "FAIL");
   return iOK ? 0 : 1;
}
