#include <stddef.h>
#include <stdio.h>
#include <jpeglib.h>

int main(void)
{
   struct jpeg_error_mgr theError;
   struct jpeg_compress_struct theCompressor;

   theCompressor.err = jpeg_std_error(&theError);
   jpeg_create_compress(&theCompressor);

   if (JPEG_LIB_VERSION <= 0)
   {
      jpeg_destroy_compress(&theCompressor);
      printf("SMOKE|CHECK|libjpeg-turbo-version|FAIL|JPEG_LIB_VERSION is not positive\n");
      printf("SMOKE|RESULT|FAIL|libjpeg-turbo consumer validation failed\n");
      return 2;
   }

   jpeg_destroy_compress(&theCompressor);
   printf("SMOKE|CHECK|libjpeg-turbo-version|PASS|JPEG_LIB_VERSION=%d\n", JPEG_LIB_VERSION);
   printf("SMOKE|RESULT|PASS|libjpeg-turbo consumer linked and executed successfully\n");
   return 0;
}
