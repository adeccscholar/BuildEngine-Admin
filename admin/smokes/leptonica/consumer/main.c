#include <leptonica/allheaders.h>
#include <stdio.h>
int main(void) {
   PIX* p=pixCreate(160,48,8);
   if(!p) {
      puts("SMOKE|RESULT|FAIL|Leptonica pixCreate");
      return 1;
   }
   pixSetAll(p);
   int w=0,h=0;
   pixGetDimensions(p,&w,&h,NULL);
   PIX* q=pixScale(p,0.5f,0.5f);
   int ok=q && w==160 && h==48 && pixGetWidth(q)==80;
   printf("SMOKE|CHECK|image-transform|%s|8-bit image create and scale\n",ok?"PASS":"FAIL");
   printf("SMOKE|RESULT|%s|Leptonica DLL consumer\n",ok?"PASS":"FAIL");
   pixDestroy(&q);
   pixDestroy(&p);
   return ok?0:1;
}
