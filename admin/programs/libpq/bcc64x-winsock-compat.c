/*
 * BCC64X compatibility for Windows socket symbols that the Windows SDK
 * declares as extern inline for non-MSVC compilers.  BCC64X does not emit
 * the external definitions required by PostgreSQL.
 *
 * Deliberately do not include ws2ipdef.h or ws2tcpip.h here: those headers
 * contain the colliding extern-inline definitions.  in6addr.h provides the
 * IPv6 address type only.  The local socket-address view below matches the
 * Windows SOCKADDR_IN6 ABI layout used by the callers.
 */

#if defined(__clang__) && defined(__BORLANDC__)

#include <winsock2.h>
#include <in6addr.h>

typedef struct Bcc64xSockaddrIn6 {
   ADDRESS_FAMILY sin6_family;
   USHORT   sin6_port;
   ULONG    sin6_flowinfo;
   IN6_ADDR sin6_addr;
   ULONG    sin6_scope_id;
} Bcc64xSockaddrIn6;

const IN_ADDR in4addr_any = { 0 };

BOOLEAN
IN6_IS_ADDR_V4TRANSLATED(CONST IN6_ADDR *a)
{
   return (BOOLEAN) ((a->u.Word[0] == 0) &&
                     (a->u.Word[1] == 0) &&
                     (a->u.Word[2] == 0) &&
                     (a->u.Word[3] == 0) &&
                     (a->u.Word[4] == 0xffff) &&
                     (a->u.Word[5] == 0));
}

BOOLEAN
IN6ADDR_ISEQUAL(CONST Bcc64xSockaddrIn6 *a, CONST Bcc64xSockaddrIn6 *b)
{
   return (BOOLEAN) ((a->sin6_scope_id == b->sin6_scope_id) &&
                     (a->sin6_addr.u.Word[0] == b->sin6_addr.u.Word[0]) &&
                     (a->sin6_addr.u.Word[1] == b->sin6_addr.u.Word[1]) &&
                     (a->sin6_addr.u.Word[2] == b->sin6_addr.u.Word[2]) &&
                     (a->sin6_addr.u.Word[3] == b->sin6_addr.u.Word[3]) &&
                     (a->sin6_addr.u.Word[4] == b->sin6_addr.u.Word[4]) &&
                     (a->sin6_addr.u.Word[5] == b->sin6_addr.u.Word[5]) &&
                     (a->sin6_addr.u.Word[6] == b->sin6_addr.u.Word[6]) &&
                     (a->sin6_addr.u.Word[7] == b->sin6_addr.u.Word[7]));
}

BOOLEAN
IN6ADDR_ISUNSPECIFIED(CONST Bcc64xSockaddrIn6 *a)
{
   return (BOOLEAN) ((a->sin6_scope_id == 0) &&
                     (a->sin6_addr.u.Word[0] == 0) &&
                     (a->sin6_addr.u.Word[1] == 0) &&
                     (a->sin6_addr.u.Word[2] == 0) &&
                     (a->sin6_addr.u.Word[3] == 0) &&
                     (a->sin6_addr.u.Word[4] == 0) &&
                     (a->sin6_addr.u.Word[5] == 0) &&
                     (a->sin6_addr.u.Word[6] == 0) &&
                     (a->sin6_addr.u.Word[7] == 0));
}

#endif
