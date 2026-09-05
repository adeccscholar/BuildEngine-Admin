/* Copyright (c) 2026 adecc Systemhaus GmbH
 * SPDX-License-Identifier: MIT
 * Project: adecc Scholar
 *
 * BCC64X Win64 Modern Debug compatibility for the Windows UCRT math contract.
 *
 * On the verified target system, BCC64X Debug code can retain an external
 * __imp_fabsf reference even though Release optimization lowers fabsf to an
 * LLVM/CPU intrinsic.  The OS API-set contract resolves to ucrtbase.dll, but
 * the resolved host does not expose fabsf on that system.  Keep the workaround
 * local to the BCC64X-built Debug archive and implement fabsf through Clang's
 * builtin so this object has no UCRT math dependency of its own.
 */

#if !defined(__BORLANDC__) || !defined(__clang__) || !defined(_WIN64) || !defined(__MINGW64__)
#  error This compatibility object is only for the BCC64X Win64 Modern target.
#endif

typedef float (*adecc_fabsf_fn)(float);

float fabsf(float fValue)
{
   return __builtin_fabsf(fValue);
}

/* COFF dllimport call sites may reference the import pointer directly. */
adecc_fabsf_fn __imp_fabsf = &fabsf;
