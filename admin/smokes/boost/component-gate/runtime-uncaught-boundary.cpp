#include "runtime-uncaught-boundary.h"

#include <boost/core/uncaught_exceptions.hpp>

#include <exception>

unsigned int AdeccBoundaryStdUncaughtExceptions() noexcept {
   return static_cast<unsigned int>(std::uncaught_exceptions());
}

unsigned int AdeccBoundaryBoostUncaughtExceptions() noexcept {
   return boost::core::uncaught_exceptions();
}

unsigned long AdeccBoundaryUncaughtExceptionsFeature() noexcept {
#if defined(__cpp_lib_uncaught_exceptions)
   return static_cast<unsigned long>(__cpp_lib_uncaught_exceptions);
#else
   return 0UL;
#endif
}

int AdeccBoundaryBoostUsesStdUncaughtExceptions() noexcept {
#if defined(__cpp_lib_uncaught_exceptions) && __cpp_lib_uncaught_exceptions >= 201411
   return 1;
#else
   return 0;
#endif
}
