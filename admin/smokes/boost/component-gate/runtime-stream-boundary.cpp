#include "runtime-stream-boundary.h"

int AdeccBoundaryOstreamPut(std::ostream& theStream) noexcept {
   try {
      theStream.put('\n');
      return theStream.good() ? 0 : 1;
   }
   catch(...) {
      return 2;
   }
}

int AdeccBoundaryOstreamFlush(std::ostream& theStream) noexcept {
   try {
      theStream.flush();
      return theStream.good() ? 0 : 1;
   }
   catch(...) {
      return 2;
   }
}

int AdeccBoundaryOstreamInsertChar(std::ostream& theStream) noexcept {
   try {
      theStream << '\n';
      return theStream.good() ? 0 : 1;
   }
   catch(...) {
      return 2;
   }
}

int AdeccBoundaryOstreamEndl(std::ostream& theStream) noexcept {
   try {
      theStream << std::endl;
      return theStream.good() ? 0 : 1;
   }
   catch(...) {
      return 2;
   }
}
