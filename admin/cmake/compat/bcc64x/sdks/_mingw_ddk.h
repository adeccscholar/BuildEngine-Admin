#ifndef MINGW_DDK_H
#define MINGW_DDK_H

/*
 * RAD Studio 13 / BCC64X ships a mingw-w64 compatibility _mingw.h which
 * includes sdks/_mingw_ddk.h, while that generated compatibility stub is
 * absent from the installed header tree.  The upstream no-DDK generated
 * variant only leaves MINGW_HAS_DDK_H undefined.
 *
 * This overlay supplies exactly that missing no-DDK stub.  It does not select
 * a MinGW compiler or SDK and it does not modify the RAD Studio installation.
 */

#endif
