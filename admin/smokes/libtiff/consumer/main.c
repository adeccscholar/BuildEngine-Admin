#include <tiffio.h>
#include <stdio.h>

int main(void)
{
   char const* szVersion = TIFFGetVersion();
   if (szVersion == NULL || *szVersion == '\0')
   {
      printf("SMOKE|CHECK|libtiff-version|FAIL|TIFFGetVersion returned no version text\n");
      printf("SMOKE|RESULT|FAIL|libTIFF consumer validation failed\n");
      return 2;
   }

   printf("SMOKE|CHECK|libtiff-version|PASS|TIFFGetVersion returned version text\n");
   printf("SMOKE|RESULT|PASS|libTIFF consumer linked and executed successfully\n");
   return 0;
}
