#include <ft2build.h>
#include FT_FREETYPE_H
#include <stdio.h>

int main(void)
{
   FT_Library theLibrary = NULL;
   if (FT_Init_FreeType(&theLibrary) != 0)
   {
      return 2;
   }

   FT_Done_FreeType(theLibrary);
   printf("CHECK freetype consumer PASS\n");
   return 0;
}
