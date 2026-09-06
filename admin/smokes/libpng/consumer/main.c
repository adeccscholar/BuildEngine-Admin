#include <png.h>
#include <stdio.h>

int main(void)
{
   png_uint_32 const uVersion = png_access_version_number();
   if (uVersion == 0)
   {
      return 2;
   }

   printf("CHECK libpng consumer PASS\n");
   return 0;
}
