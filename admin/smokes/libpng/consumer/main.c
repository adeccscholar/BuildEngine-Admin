#include <png.h>
#include <stdio.h>

int main(void)
{
   png_uint_32 const uVersion = png_access_version_number();
   if (uVersion == 0)
   {
      printf("SMOKE|CHECK|libpng-version|FAIL|png_access_version_number returned 0\n");
      printf("SMOKE|RESULT|FAIL|libpng consumer validation failed\n");
      return 2;
   }

   printf("SMOKE|CHECK|libpng-version|PASS|png_access_version_number returned %lu\n", (unsigned long)uVersion);
   printf("SMOKE|RESULT|PASS|libpng consumer linked and executed successfully\n");
   return 0;
}
