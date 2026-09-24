/*
 * BCC64X compatibility for Windows socket symbols that are declared by the
 * selected headers but not provided with the linkage expected by PostgreSQL.
 */

#if defined(__clang__) && defined(__BORLANDC__)

#include <winsock2.h>
#include <ws2tcpip.h>

const IN_ADDR in4addr_any = { 0 };

BOOLEAN
IN6_IS_ADDR_V4TRANSLATED(CONST IN6_ADDR *a)
{
    return (BOOLEAN) ((a->s6_words[0] == 0) &&
                      (a->s6_words[1] == 0) &&
                      (a->s6_words[2] == 0) &&
                      (a->s6_words[3] == 0) &&
                      (a->s6_words[4] == 0xffff) &&
                      (a->s6_words[5] == 0));
}

#endif
