#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CONFIG_OPTIONS_H
#include <stdio.h>

#ifndef FT_CONFIG_OPTION_USE_HARFBUZZ
#error "FreeType was built without HarfBuzz support"
#endif

#ifdef FT_CONFIG_OPTION_USE_HARFBUZZ_DYNAMIC
#error "FreeType HarfBuzz support must be linked explicitly, not loaded dynamically"
#endif

int main(void)
{
   FT_Library theLibrary = NULL;
   if (FT_Init_FreeType(&theLibrary) != 0)
   {
      printf("SMOKE|CHECK|freetype-init|FAIL|FT_Init_FreeType failed\n");
      printf("SMOKE|RESULT|FAIL|FreeType consumer validation failed\n");
      return 2;
   }

   FT_Done_FreeType(theLibrary);
   printf("SMOKE|CHECK|freetype-harfbuzz|PASS|FreeType was built with linked HarfBuzz support\n");
   printf("SMOKE|CHECK|freetype-init|PASS|FT_Init_FreeType and FT_Done_FreeType succeeded\n");
   printf("SMOKE|RESULT|PASS|FreeType consumer linked and executed successfully\n");
   return 0;
}
