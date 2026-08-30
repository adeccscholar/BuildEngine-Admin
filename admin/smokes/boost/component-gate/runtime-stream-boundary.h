#pragma once

#include <ostream>

#if defined(_WIN32)
#  if defined(ADECC_STREAM_BOUNDARY_EXPORTS)
#    define ADECC_STREAM_BOUNDARY_API __declspec(dllexport)
#  else
#    define ADECC_STREAM_BOUNDARY_API __declspec(dllimport)
#  endif
#else
#  define ADECC_STREAM_BOUNDARY_API
#endif

extern "C" {

ADECC_STREAM_BOUNDARY_API int AdeccBoundaryOstreamPut(std::ostream& theStream) noexcept;
ADECC_STREAM_BOUNDARY_API int AdeccBoundaryOstreamFlush(std::ostream& theStream) noexcept;
ADECC_STREAM_BOUNDARY_API int AdeccBoundaryOstreamInsertChar(std::ostream& theStream) noexcept;
ADECC_STREAM_BOUNDARY_API int AdeccBoundaryOstreamEndl(std::ostream& theStream) noexcept;

}
