#include <libpq-fe.h>

#include <stdio.h>

int main(void)
{
   int const iVersion = PQlibVersion();
   printf("libpq %d\n", iVersion);
   return iVersion > 0 ? 0 : 1;
}
