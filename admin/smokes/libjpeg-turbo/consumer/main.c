#include <jpeglib.h>
#include <stdio.h>

int main(void)
{
   struct jpeg_error_mgr theError;
   struct jpeg_compress_struct theCompressor;

   theCompressor.err = jpeg_std_error(&theError);
   jpeg_create_compress(&theCompressor);

   if (JPEG_LIB_VERSION <= 0)
   {
      jpeg_destroy_compress(&theCompressor);
      return 2;
   }

   jpeg_destroy_compress(&theCompressor);
   printf("CHECK libjpeg-turbo consumer PASS\n");
   return 0;
}
