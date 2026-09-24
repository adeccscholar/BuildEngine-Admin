/*
 * BCC64X compatibility for Windows socket symbols that are declared as
 * extern inline by the Windows SDK for non-MSVC compilers, but for which
 * BCC64X does not emit the external definitions required by PostgreSQL.
 *
 * Do not include ws2tcpip.h here. Its extern-inline definitions would
 * collide with the explicit external definitions below.
 */

#if defined(__clang__) && defined(__BORLANDC__)

#include <winsock2.h>

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

BOOLEAN
IN6ADDR_ISEQUAL(CONST SOCKADDR_IN6 *a, CONST SOCKADDR_IN6 *b)
{
   return (BOOLEAN) ((a->sin6_scope_id == b->sin6_scope_id) &&
                     (a->sin6_addr.s6_words[0] == b->sin6_addr.s6_words[0]) &&
                     (a->sin6_addr.s6_words[1] == b->sin6_addr.s6_words[1]) &&
                     (a->sin6_addr.s6_words[2] == b->sin6_addr.s6_words[2]) &&
                     (a->sin6_addr.s6_words[3] == b->sin6_addr.s6_words[3]) &&
                     (a->sin6_addr.s6_words[4] == b->sin6_addr.s6_words[4]) &&
                     (a->sin6_addr.s6_words[5] == b->sin6_addr.s6_words[5]) &&
                     (a->sin6_addr.s6_words[6] == b->sin6_addr.s6_words[6]) &&
                     (a->sin6_addr.s6_words[7] == b->sin6_addr.s6_words[7]));
}

BOOLEAN
IN6ADDR_ISUNSPECIFIED(CONST SOCKADDR_IN6 *a)
{
   return (BOOLEAN) ((a->sin6_scope_id == 0) &&
                     (a->sin6_addr.s6_words[0] == 0) &&
                     (a->sin6_addr.s6_words[1] == 0) &&
                     (a->sin6_addr.s6_words[2] == 0) &&
                     (a->sin6_addr.s6_words[3] == 0) &&
                     (a->sin6_addr.s6_words[4] == 0) &&
                     (a->sin6_addr.s6_words[5] == 0) &&
                     (a->sin6_addr.s6_words[6] == 0) &&
                     (a->sin6_addr.s6_words[7] == 0));
}

#endif
