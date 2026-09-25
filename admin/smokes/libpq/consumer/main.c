#include <libpq-fe.h>

#include <stdio.h>

int main(void)
{
   int const iVersion = PQlibVersion();

   if(iVersion > 0)
      {
      printf("SMOKE|CHECK|libpq-version|PASS|PQlibVersion=%d\n", iVersion);
      printf("SMOKE|RESULT|PASS|libpq consumer smoke passed\n");
      return 0;
      }

   printf("SMOKE|CHECK|libpq-version|FAIL|PQlibVersion=%d\n", iVersion);
   printf("SMOKE|RESULT|FAIL|libpq consumer smoke failed\n");
   return 1;
}
