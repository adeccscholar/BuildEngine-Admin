#include <iconv.h>

#include <stdio.h>

int main(void)
{
   iconv_t theConverter = iconv_open("UTF-8", "UTF-16LE");
   if (theConverter == (iconv_t)-1)
   {
      printf("SMOKE|CHECK|win-iconv-open|FAIL|iconv_open failed\n");
      printf("SMOKE|RESULT|FAIL|win-iconv consumer validation failed\n");
      return 2;
   }

   iconv_close(theConverter);
   printf("SMOKE|CHECK|win-iconv-open|PASS|iconv_open/iconv_close succeeded\n");
   printf("SMOKE|RESULT|PASS|win-iconv consumer linked and executed successfully\n");
   return 0;
}
