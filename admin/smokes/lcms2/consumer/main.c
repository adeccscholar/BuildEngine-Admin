#include <lcms2.h>
#include <stdio.h>
int main(void) {
   cmsHPROFILE profile = cmsCreate_sRGBProfile();
   int const ok = profile != NULL;
   if(profile != NULL) cmsCloseProfile(profile);
   printf("SMOKE|CHECK|srgb-profile|%s|in-memory ICC profile\n", ok ? "PASS" : "FAIL");
   printf("SMOKE|RESULT|%s|LittleCMS consumer\n", ok ? "PASS" : "FAIL");
   return ok ? 0 : 1;
}
