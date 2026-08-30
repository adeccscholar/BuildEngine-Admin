#include <boost/config.hpp>
#include <boost/version.hpp>

#if defined(BOOST_EMBTC)
#  error "Managed Boost 1.92.0 consumer must bypass the historical Boost CodeGear layer"
#endif
#if defined(BOOST_NO_CXX11_NOEXCEPT)
#  error "Managed Boost 1.92.0 consumer requires native noexcept"
#endif
#if !defined(BOOST_CLANG)
#  error "Managed Boost 1.92.0 consumer expects native-Clang Boost.Config routing"
#endif

#include "runtime-stream-boundary.h"

#include <boost/archive/basic_streambuf_locale_saver.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/codecvt_null.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/io/ios_state.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/serialization/string.hpp>

#include <iostream>
#include <locale>
#include <sstream>
#include <string>

namespace {

template<typename function_ty>
bool RunStage(char const* const pName, function_ty theFunction) {
   std::cout << "[SERIALIZATION-BOUNDARY] BEGIN " << pName << std::endl;
   try {
      bool const bSuccess = theFunction();
      std::cout << "[SERIALIZATION-BOUNDARY] " << (bSuccess ? "PASS " : "FAIL ") << pName << std::endl;
      return bSuccess;
   }
   catch(std::exception const& theException) {
      std::cout << "[SERIALIZATION-BOUNDARY] EXCEPTION " << pName << ": " << theException.what() << std::endl;
      return false;
   }
   catch(...) {
      std::cout << "[SERIALIZATION-BOUNDARY] EXCEPTION " << pName << ": unknown" << std::endl;
      return false;
   }
}

void Checkpoint(char const* const pName) {
   std::cout << "[SERIALIZATION-BOUNDARY] CHECKPOINT " << pName << std::endl;
}

bool CheckLocalEndl() {
   std::stringstream theStream;
   theStream << std::endl;
   return theStream.good() && theStream.str() == "\n";
}

bool CheckDllOstreamPut() {
   std::stringstream theStream;
   return AdeccBoundaryOstreamPut(theStream) == 0 && theStream.str() == "\n";
}

bool CheckDllOstreamFlush() {
   std::stringstream theStream;
   theStream << "x";
   return AdeccBoundaryOstreamFlush(theStream) == 0 && theStream.str() == "x";
}

bool CheckDllOstreamInsertChar() {
   std::stringstream theStream;
   return AdeccBoundaryOstreamInsertChar(theStream) == 0 && theStream.str() == "\n";
}

bool CheckDllOstreamEndl() {
   std::stringstream theStream;
   return AdeccBoundaryOstreamEndl(theStream) == 0 && theStream.str() == "\n";
}

bool CheckLocalTextPrimitiveLifetimeStdStream() {
   std::stringstream theStream;
   {
      boost::io::ios_flags_saver theFlagsSaver { theStream };
      boost::io::ios_precision_saver thePrecisionSaver { theStream };
      boost::archive::codecvt_null<char> theFacet { 1U };
      std::locale const theArchiveLocale { theStream.getloc(), &theFacet };
      boost::archive::basic_ostream_locale_saver<char, std::char_traits<char>> theLocaleSaver { theStream };

      theStream << std::noboolalpha;
      theStream << std::endl;
   }
   return theStream.good();
}

using boost_sink_ty = boost::iostreams::back_insert_device<std::string>;
using boost_ostream_ty = boost::iostreams::stream<boost_sink_ty>;

bool CheckBoostIoStreamsPlainOutput() {
   std::string strBuffer;
   {
      boost_sink_ty theSink { strBuffer };
      boost_ostream_ty theStream { theSink };
      theStream << "boost-iostreams";
      theStream.flush();
      if(!theStream.good()) return false;
   }
   return strBuffer == "boost-iostreams";
}

bool CheckLocalTextPrimitiveLifetimeBoostStream() {
   std::string strBuffer;
   {
      boost_sink_ty theSink { strBuffer };
      boost_ostream_ty theStream { theSink };
      {
         boost::io::ios_flags_saver theFlagsSaver { theStream };
         boost::io::ios_precision_saver thePrecisionSaver { theStream };
         boost::archive::codecvt_null<char> theFacet { 1U };
         std::locale const theArchiveLocale { theStream.getloc(), &theFacet };
         boost::archive::basic_ostream_locale_saver<char, std::char_traits<char>> theLocaleSaver { theStream };

         theStream << std::noboolalpha;
         theStream << std::endl;
      }
      theStream.flush();
      if(!theStream.good()) return false;
   }
   return strBuffer == "\n";
}

bool CheckBinaryArchiveBoostStream() {
   std::string strBuffer;
   std::string const strOriginal { "boost-serialization-binary-iostreams" };
   {
      boost_sink_ty theSink { strBuffer };
      boost_ostream_ty theStream { theSink };
      {
         boost::archive::binary_oarchive theArchive { theStream };
         theArchive << strOriginal;
      }
      theStream.flush();
      if(!theStream.good()) return false;
   }

   boost::iostreams::array_source theSource { strBuffer.data(), strBuffer.size() };
   boost::iostreams::stream<boost::iostreams::array_source> theStream { theSource };
   std::string strRoundTrip;
   {
      boost::archive::binary_iarchive theArchive { theStream };
      theArchive >> strRoundTrip;
   }
   return strRoundTrip == strOriginal;
}

bool CheckTextArchiveNoHeaderNoCodecvtBoostStream() {
   std::string strBuffer;
   boost_sink_ty theSink { strBuffer };
   boost_ostream_ty theStream { theSink };

   Checkpoint("boost-iostreams text-noheader-nocodecvt before-ctor");
   {
      boost::archive::text_oarchive theArchive {
         theStream, boost::archive::no_header | boost::archive::no_codecvt };
      Checkpoint("boost-iostreams text-noheader-nocodecvt after-ctor");
   }
   Checkpoint("boost-iostreams text-noheader-nocodecvt after-dtor");

   theStream.flush();
   return theStream.good();
}

bool CheckTextArchiveNoHeaderNoCodecvtStdStream() {
   std::stringstream theStream;
   Checkpoint("std-stringstream text-noheader-nocodecvt before-ctor");
   {
      boost::archive::text_oarchive theArchive {
         theStream, boost::archive::no_header | boost::archive::no_codecvt };
      Checkpoint("std-stringstream text-noheader-nocodecvt after-ctor");
   }
   Checkpoint("std-stringstream text-noheader-nocodecvt after-dtor");
   return theStream.good();
}

bool RunDiagnostics() {
   std::cout << "[SERIALIZATION-BOUNDARY] ABI sizeof(char)=" << sizeof(char)
             << " sizeof(wchar_t)=" << sizeof(wchar_t)
             << " sizeof(mbstate_t)=" << sizeof(std::mbstate_t) << std::endl;

   if(!RunStage("local-ostream-endl", CheckLocalEndl)) return false;
   if(!RunStage("dll-ostream-put", CheckDllOstreamPut)) return false;
   if(!RunStage("dll-ostream-flush", CheckDllOstreamFlush)) return false;
   if(!RunStage("dll-ostream-insert-char", CheckDllOstreamInsertChar)) return false;
   if(!RunStage("dll-ostream-endl", CheckDllOstreamEndl)) return false;
   if(!RunStage("local-text-primitive-lifetime-std-stream", CheckLocalTextPrimitiveLifetimeStdStream)) return false;
   if(!RunStage("boost-iostreams-plain-output", CheckBoostIoStreamsPlainOutput)) return false;
   if(!RunStage("local-text-primitive-lifetime-boost-stream", CheckLocalTextPrimitiveLifetimeBoostStream)) return false;
   if(!RunStage("binary-archive-boost-stream", CheckBinaryArchiveBoostStream)) return false;
   if(!RunStage("text-oarchive-noheader-nocodecvt-boost-stream", CheckTextArchiveNoHeaderNoCodecvtBoostStream)) return false;
   return RunStage("text-oarchive-noheader-nocodecvt-std-stream", CheckTextArchiveNoHeaderNoCodecvtStdStream);
}

}

int main() {
   return RunDiagnostics() ? 0 : 1;
}
