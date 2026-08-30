#pragma once

#if defined(_WIN32)
#  if defined(ADECC_UNCAUGHT_BOUNDARY_EXPORTS)
#    define ADECC_UNCAUGHT_BOUNDARY_API __declspec(dllexport)
#  else
#    define ADECC_UNCAUGHT_BOUNDARY_API __declspec(dllimport)
#  endif
#else
#  define ADECC_UNCAUGHT_BOUNDARY_API
#endif

extern "C" {

ADECC_UNCAUGHT_BOUNDARY_API unsigned int AdeccBoundaryStdUncaughtExceptions() noexcept;
ADECC_UNCAUGHT_BOUNDARY_API unsigned int AdeccBoundaryBoostUncaughtExceptions() noexcept;
ADECC_UNCAUGHT_BOUNDARY_API unsigned long AdeccBoundaryUncaughtExceptionsFeature() noexcept;
ADECC_UNCAUGHT_BOUNDARY_API int AdeccBoundaryBoostUsesStdUncaughtExceptions() noexcept;

}
