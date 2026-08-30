#include "runtime-stream-boundary.h"

#include <iostream>
#include <sstream>

namespace {

template<typename function_ty>
bool RunStage(char const* const pName, function_ty theFunction) {
   std::cout << "[STREAM-BOUNDARY] BEGIN " << pName << std::endl;
   bool const bSuccess = theFunction();
   std::cout << "[STREAM-BOUNDARY] " << (bSuccess ? "PASS " : "FAIL ") << pName << std::endl;
   return bSuccess;
}

bool CheckLocalEndl() {
   std::stringstream theStream;
   theStream << std::endl;
   return theStream.good() && theStream.str() == "\n";
}

bool CheckDllPut() {
   std::stringstream theStream;
   return AdeccBoundaryOstreamPut(theStream) == 0 && theStream.str() == "\n";
}

bool CheckDllFlush() {
   std::stringstream theStream;
   theStream << "x";
   return AdeccBoundaryOstreamFlush(theStream) == 0 && theStream.str() == "x";
}

bool CheckDllInsertChar() {
   std::stringstream theStream;
   return AdeccBoundaryOstreamInsertChar(theStream) == 0 && theStream.str() == "\n";
}

bool CheckDllEndl() {
   std::stringstream theStream;
   return AdeccBoundaryOstreamEndl(theStream) == 0 && theStream.str() == "\n";
}

}

int main() {
   if(!RunStage("local-endl", CheckLocalEndl)) return 1;
   if(!RunStage("dll-put", CheckDllPut)) return 1;
   if(!RunStage("dll-flush", CheckDllFlush)) return 1;
   if(!RunStage("dll-insert-char", CheckDllInsertChar)) return 1;
   if(!RunStage("dll-endl", CheckDllEndl)) return 1;
   return 0;
}
