/*
 * BCC64X compatibility for Windows socket symbols that are declared by the
 * selected headers but not provided with the linkage expected by PostgreSQL.
 */

#if defined(__clang__) && defined(__BORLANDC__)

#include <winsock2.h>
#include <ws2tcpip.h>

const struct in_addr in4addr_any = { 0 };

int
IN6_IS_ADDR_V4TRANSLATED(const struct in6_addr *a)
{
    return (a->s6_words[0] == 0) &&
           (a->s6_words[1] == 0) &&
           (a->s6_words[2] == 0) &&
           (a->s6_words[3] == 0) &&
           (a->s6_words[4] == 0xffff) &&
           (a->s6_words[5] == 0);
}

#endif
