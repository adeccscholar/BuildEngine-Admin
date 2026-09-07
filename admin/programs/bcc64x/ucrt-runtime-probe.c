/* Copyright (c) 2026 adecc Systemhaus GmbH
 * SPDX-License-Identifier: MIT
 * Project: adecc Scholar
 *
 * Minimal BCC64X Win64 UCRT runtime/linker reproducer.
 */

#include <math.h>

float (* volatile g_fabsf)(float) = fabsf;
long double (* volatile g_nextafterl)(long double, long double) = nextafterl;
double (* volatile g_nexttoward)(double, long double) = nexttoward;
float (* volatile g_nexttowardf)(float, long double) = nexttowardf;
long double (* volatile g_nexttowardl)(long double, long double) = nexttowardl;

__declspec(dllexport) int Bcc64xUcrtRuntimeProbe(void)
{
   return g_fabsf != 0 &&
          g_nextafterl != 0 &&
          g_nexttoward != 0 &&
          g_nexttowardf != 0 &&
          g_nexttowardl != 0;
}
