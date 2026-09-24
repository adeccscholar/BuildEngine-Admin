/*
 * BCC64X compatibility for the Windows socket symbol that is declared by the
 * selected headers but not provided with the linkage expected by PostgreSQL.
 */

#if defined(__clang__) && defined(__BORLANDC__)

#include <winsock2.h>

const IN_ADDR in4addr_any = { 0 };

#endif
