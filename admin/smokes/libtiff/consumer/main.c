#include <tiffio.h>
#include <stdio.h>

int main(void)
{
   char const* szVersion = TIFFGetVersion();
   if (szVersion == NULL || *szVersion == '\0')
   {
      return 2;
   }

   printf("CHECK libtiff consumer PASS\n");
   return 0;
}
