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

#ifndef ADECC_BOOST_RUNTIME_CASE
#  define ADECC_BOOST_RUNTIME_CASE 0
#endif

#if ADECC_BOOST_GATE_CODE == 1
#  include <boost/asio.hpp>
#  include <boost/beast/http.hpp>
#  include <boost/filesystem.hpp>
#  include <boost/json.hpp>
#  include <boost/program_options.hpp>
#  include <boost/regex.hpp>
#  include <boost/thread.hpp>
#elif ADECC_BOOST_GATE_CODE == 2
#  include <boost/geometry.hpp>
#  include <boost/multiprecision/cpp_int.hpp>
#  include <boost/numeric/ublas/vector.hpp>
#  include <boost/rational.hpp>
#elif ADECC_BOOST_GATE_CODE == 3
#  include <boost/mp11.hpp>
#  include <boost/spirit/home/x3.hpp>
#elif ADECC_BOOST_GATE_CODE == 4
#  include <boost/graph/adjacency_list.hpp>
#  include <boost/multi_index/hashed_index.hpp>
#  include <boost/multi_index/member.hpp>
#  include <boost/multi_index_container.hpp>
#  include <boost/uuid/string_generator.hpp>
#elif ADECC_BOOST_GATE_CODE == 5
#  include <boost/interprocess/shared_memory_object.hpp>
#  include <boost/lockfree/queue.hpp>
#elif ADECC_BOOST_GATE_CODE == 6
#  if ADECC_BOOST_RUNTIME_CASE == 1
#    include <boost/charconv.hpp>
#  elif ADECC_BOOST_RUNTIME_CASE == 2
#    include <boost/archive/basic_streambuf_locale_saver.hpp>
#    include <boost/archive/binary_iarchive.hpp>
#    include <boost/archive/binary_oarchive.hpp>
#    include <boost/archive/codecvt_null.hpp>
#    include <boost/archive/text_iarchive.hpp>
#    include <boost/archive/text_oarchive.hpp>
#    include <boost/serialization/string.hpp>
#  elif ADECC_BOOST_RUNTIME_CASE == 3
#    include <boost/url.hpp>
#  else
#    error "system-io-runtime requires an explicit runtime case"
#  endif
#elif ADECC_BOOST_GATE_CODE == 7
#  include <boost/dynamic_bitset.hpp>
#  include <boost/optional.hpp>
#  include <boost/signals2.hpp>
#  include <boost/variant2/variant.hpp>
#endif

#include <array>
#include <cwchar>
#include <exception>
#include <iostream>
#include <locale>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {

#if ADECC_BOOST_GATE_CODE == 6 && ADECC_BOOST_RUNTIME_CASE == 2

template<typename function_ty>
bool RunSerializationDiagnosticStage(char const* const pName, function_ty theFunction) {
   std::cout << "[SERIALIZATION-DIAG] BEGIN " << pName << std::endl;
   try {
      bool const bSuccess = theFunction();
      std::cout << "[SERIALIZATION-DIAG] " << (bSuccess ? "PASS " : "FAIL ") << pName << std::endl;
      return bSuccess;
   }
   catch(std::exception const& theException) {
      std::cout << "[SERIALIZATION-DIAG] EXCEPTION " << pName << ": " << theException.what() << std::endl;
      return false;
   }
   catch(...) {
      std::cout << "[SERIALIZATION-DIAG] EXCEPTION " << pName << ": unknown" << std::endl;
      return false;
   }
}

void SerializationCheckpoint(char const* const pName) {
   std::cout << "[SERIALIZATION-DIAG] CHECKPOINT " << pName << std::endl;
}

bool CheckStdCodecvtChar() {
   std::locale const theLocale = std::locale::classic();
   using facet_ty = std::codecvt<char, char, std::mbstate_t>;
   if(!std::has_facet<facet_ty>(theLocale)) return false;

   facet_ty const& theFacet = std::use_facet<facet_ty>(theLocale);
   return theFacet.always_noconv();
}

bool CheckStdCodecvtWChar() {
   std::locale const theLocale = std::locale::classic();
   using facet_ty = std::codecvt<wchar_t, char, std::mbstate_t>;
   if(!std::has_facet<facet_ty>(theLocale)) return false;

   facet_ty const& theFacet = std::use_facet<facet_ty>(theLocale);
   std::wstring_view const svOriginal { L"boost" };

   std::mbstate_t theState {};
   wchar_t const* pWideNext = svOriginal.data();
   std::array<char, 32> arrNarrow {};
   char* pNarrowNext = arrNarrow.data();
   std::codecvt_base::result const theOutResult = theFacet.out(
      theState,
      svOriginal.data(), svOriginal.data() + svOriginal.size(), pWideNext,
      arrNarrow.data(), arrNarrow.data() + arrNarrow.size(), pNarrowNext);
   if(theOutResult != std::codecvt_base::ok || pWideNext != svOriginal.data() + svOriginal.size()) return false;

   std::string_view const svNarrow {
      arrNarrow.data(), static_cast<std::size_t>(pNarrowNext - arrNarrow.data()) };
   if(svNarrow != "boost") return false;

   theState = std::mbstate_t {};
   char const* pNarrowReadNext = arrNarrow.data();
   std::array<wchar_t, 32> arrWide {};
   wchar_t* pWideOutputNext = arrWide.data();
   std::codecvt_base::result const theInResult = theFacet.in(
      theState,
      arrNarrow.data(), pNarrowNext, pNarrowReadNext,
      arrWide.data(), arrWide.data() + arrWide.size(), pWideOutputNext);
   if(theInResult != std::codecvt_base::ok || pNarrowReadNext != pNarrowNext) return false;

   std::wstring_view const svRoundTrip {
      arrWide.data(), static_cast<std::size_t>(pWideOutputNext - arrWide.data()) };
   return svRoundTrip == svOriginal;
}

bool CheckStdStreamImbueChar() {
   std::stringstream theStream;
   theStream.imbue(std::locale::classic());
   theStream << "locale-char";
   theStream.seekg(0);
   std::string strValue;
   theStream >> strValue;
   return strValue == "locale-char";
}

bool CheckStdStreamImbueWChar() {
   std::wstringstream theStream;
   theStream.imbue(std::locale::classic());
   theStream << L"locale-wchar";
   theStream.seekg(0);
   std::wstring strValue;
   theStream >> strValue;
   return strValue == L"locale-wchar";
}

bool CheckBoostCodecvtNullChar() {
   boost::archive::codecvt_null<char> theFacet { 1U };
   if(!theFacet.always_noconv()) return false;

   std::stringstream theStream;
   std::locale const theArchiveLocale { theStream.getloc(), &theFacet };
   theStream.imbue(theArchiveLocale);
   theStream << "boost-codecvt-char";
   return theStream.str() == "boost-codecvt-char";
}

bool CheckBoostCodecvtNullWChar() {
   boost::archive::codecvt_null<wchar_t> theFacet { 1U };
   std::wstring_view const svOriginal { L"boost" };

   std::mbstate_t theState {};
   wchar_t const* pWideNext = svOriginal.data();
   std::array<wchar_t, 32> arrEncodedStorage {};
   char* const pEncodedBegin = reinterpret_cast<char*>(arrEncodedStorage.data());
   char* pEncodedNext = pEncodedBegin;
   char* const pEncodedEnd = pEncodedBegin + sizeof(arrEncodedStorage);
   std::codecvt_base::result const theOutResult = theFacet.out(
      theState,
      svOriginal.data(), svOriginal.data() + svOriginal.size(), pWideNext,
      pEncodedBegin, pEncodedEnd, pEncodedNext);
   if(theOutResult != std::codecvt_base::ok || pWideNext != svOriginal.data() + svOriginal.size()) return false;
   if(pEncodedNext - pEncodedBegin != static_cast<std::ptrdiff_t>(svOriginal.size() * sizeof(wchar_t))) return false;

   theState = std::mbstate_t {};
   char const* pEncodedReadNext = pEncodedBegin;
   std::array<wchar_t, 32> arrRoundTrip {};
   wchar_t* pWideOutputNext = arrRoundTrip.data();
   std::codecvt_base::result const theInResult = theFacet.in(
      theState,
      pEncodedBegin, pEncodedNext, pEncodedReadNext,
      arrRoundTrip.data(), arrRoundTrip.data() + arrRoundTrip.size(), pWideOutputNext);
   if(theInResult != std::codecvt_base::ok || pEncodedReadNext != pEncodedNext) return false;

   std::wstring_view const svRoundTrip {
      arrRoundTrip.data(), static_cast<std::size_t>(pWideOutputNext - arrRoundTrip.data()) };
   return svRoundTrip == svOriginal;
}

bool CheckBoostOstreamLocaleSaverChar() {
   std::stringstream theStream;
   {
      boost::archive::basic_ostream_locale_saver<char, std::char_traits<char>> theSaver { theStream };
      theStream << "boost-locale-saver-char";
   }
   return theStream.str() == "boost-locale-saver-char";
}

bool CheckBinaryArchive() {
   std::stringstream theStream;
   std::string const strOriginal { "boost-serialization-binary" };
   {
      boost::archive::binary_oarchive theArchive(theStream);
      theArchive << strOriginal;
   }
   theStream.seekg(0);
   std::string strRoundTrip;
   {
      boost::archive::binary_iarchive theArchive(theStream);
      theArchive >> strRoundTrip;
   }
   return strRoundTrip == strOriginal;
}

bool CheckTextArchiveNoHeaderNoCodecvtConstruct() {
   std::stringstream theStream;
   SerializationCheckpoint("text-noheader-nocodecvt before-ctor");
   {
      boost::archive::text_oarchive theArchive(
         theStream, boost::archive::no_header | boost::archive::no_codecvt);
      SerializationCheckpoint("text-noheader-nocodecvt after-ctor");
   }
   SerializationCheckpoint("text-noheader-nocodecvt after-dtor");
   return true;
}

bool CheckTextArchiveNoHeaderDefaultConstruct() {
   std::stringstream theStream;
   SerializationCheckpoint("text-noheader-default before-ctor");
   {
      boost::archive::text_oarchive theArchive(theStream, boost::archive::no_header);
      SerializationCheckpoint("text-noheader-default after-ctor");
   }
   SerializationCheckpoint("text-noheader-default after-dtor");
   return true;
}

bool CheckTextArchiveHeaderNoCodecvtConstruct() {
   std::stringstream theStream;
   SerializationCheckpoint("text-header-nocodecvt before-ctor");
   {
      boost::archive::text_oarchive theArchive(theStream, boost::archive::no_codecvt);
      SerializationCheckpoint("text-header-nocodecvt after-ctor");
   }
   SerializationCheckpoint("text-header-nocodecvt after-dtor");
   return !theStream.str().empty();
}

bool CheckTextArchiveNoHeaderNoCodecvtOutput() {
   std::stringstream theStream;
   std::string const strOriginal { "boost-serialization-output" };
   SerializationCheckpoint("text-noheader-nocodecvt-output before-ctor");
   {
      boost::archive::text_oarchive theArchive(
         theStream, boost::archive::no_header | boost::archive::no_codecvt);
      SerializationCheckpoint("text-noheader-nocodecvt-output after-ctor");
      theArchive << strOriginal;
      SerializationCheckpoint("text-noheader-nocodecvt-output after-save");
   }
   SerializationCheckpoint("text-noheader-nocodecvt-output after-dtor");
   return !theStream.str().empty();
}

bool CheckTextArchiveNoCodecvt() {
   std::stringstream theStream;
   std::string const strOriginal { "boost-serialization-no-codecvt" };
   SerializationCheckpoint("text-roundtrip-nocodecvt before-oarchive");
   {
      boost::archive::text_oarchive theArchive(theStream, boost::archive::no_codecvt);
      SerializationCheckpoint("text-roundtrip-nocodecvt after-oarchive-ctor");
      theArchive << strOriginal;
      SerializationCheckpoint("text-roundtrip-nocodecvt after-save");
   }
   SerializationCheckpoint("text-roundtrip-nocodecvt after-oarchive-dtor");
   theStream.seekg(0);
   std::string strRoundTrip;
   SerializationCheckpoint("text-roundtrip-nocodecvt before-iarchive");
   {
      boost::archive::text_iarchive theArchive(theStream, boost::archive::no_codecvt);
      SerializationCheckpoint("text-roundtrip-nocodecvt after-iarchive-ctor");
      theArchive >> strRoundTrip;
      SerializationCheckpoint("text-roundtrip-nocodecvt after-load");
   }
   SerializationCheckpoint("text-roundtrip-nocodecvt after-iarchive-dtor");
   return strRoundTrip == strOriginal;
}

bool CheckTextArchiveDefault() {
   std::stringstream theStream;
   std::string const strOriginal { "boost-serialization" };
   {
      boost::archive::text_oarchive theArchive(theStream);
      theArchive << strOriginal;
   }
   theStream.seekg(0);
   std::string strRoundTrip;
   {
      boost::archive::text_iarchive theArchive(theStream);
      theArchive >> strRoundTrip;
   }
   return strRoundTrip == strOriginal;
}

bool RunSerializationDiagnostics() {
   std::cout << "[SERIALIZATION-DIAG] ABI sizeof(char)=" << sizeof(char)
             << " sizeof(wchar_t)=" << sizeof(wchar_t)
             << " sizeof(mbstate_t)=" << sizeof(std::mbstate_t) << std::endl;

   if(!RunSerializationDiagnosticStage("std-codecvt-char", CheckStdCodecvtChar)) return false;
   if(!RunSerializationDiagnosticStage("std-codecvt-wchar", CheckStdCodecvtWChar)) return false;
   if(!RunSerializationDiagnosticStage("std-stream-imbue-char", CheckStdStreamImbueChar)) return false;
   if(!RunSerializationDiagnosticStage("std-stream-imbue-wchar", CheckStdStreamImbueWChar)) return false;
   if(!RunSerializationDiagnosticStage("boost-codecvt-null-char", CheckBoostCodecvtNullChar)) return false;
   if(!RunSerializationDiagnosticStage("boost-codecvt-null-wchar", CheckBoostCodecvtNullWChar)) return false;
   if(!RunSerializationDiagnosticStage("boost-ostream-locale-saver-char", CheckBoostOstreamLocaleSaverChar)) return false;
   if(!RunSerializationDiagnosticStage("boost-binary-archive", CheckBinaryArchive)) return false;
   if(!RunSerializationDiagnosticStage("boost-text-noheader-nocodecvt-construct", CheckTextArchiveNoHeaderNoCodecvtConstruct)) return false;
   if(!RunSerializationDiagnosticStage("boost-text-noheader-default-construct", CheckTextArchiveNoHeaderDefaultConstruct)) return false;
   if(!RunSerializationDiagnosticStage("boost-text-header-nocodecvt-construct", CheckTextArchiveHeaderNoCodecvtConstruct)) return false;
   if(!RunSerializationDiagnosticStage("boost-text-noheader-nocodecvt-output", CheckTextArchiveNoHeaderNoCodecvtOutput)) return false;
   if(!RunSerializationDiagnosticStage("boost-text-archive-no-codecvt", CheckTextArchiveNoCodecvt)) return false;
   return RunSerializationDiagnosticStage("boost-text-archive-default", CheckTextArchiveDefault);
}

#endif

bool RunGate() {
#if ADECC_BOOST_GATE_CODE == 1
   namespace po = boost::program_options;
   po::options_description theOptions("boost-gate");
   theOptions.add_options()("value", po::value<int>(), "value");
   std::vector<std::string> const vecArguments { "--value", "42" };
   po::variables_map theVariables;
   po::store(po::command_line_parser(vecArguments).options(theOptions).run(), theVariables);
   po::notify(theVariables);

   boost::regex const theRegex("boost-[0-9]+");
   int iThreadValue = 0;
   boost::thread theThread([&iThreadValue]() { iThreadValue = 92; });
   theThread.join();
   boost::json::value const theJson = boost::json::parse(R"({"boost":192})");
   boost::asio::io_context theIo;
   int iAsioValue = 0;
   boost::asio::post(theIo, [&iAsioValue]() { iAsioValue = 1; });
   theIo.run();
   namespace http = boost::beast::http;
   http::request<http::empty_body> const theRequest { http::verb::get, "/boost", 11 };
   return !boost::filesystem::temp_directory_path().empty() &&
      theVariables["value"].as<int>() == 42 &&
      boost::regex_match(std::string("boost-192"), theRegex) &&
      iThreadValue == 92 && theJson.as_object().at("boost").as_int64() == 192 &&
      iAsioValue == 1 && theRequest.target() == "/boost";
#elif ADECC_BOOST_GATE_CODE == 2
   boost::multiprecision::cpp_int iBig = 1;
   iBig <<= 100;
   boost::numeric::ublas::vector<int> vecValues(3);
   vecValues(0) = 1; vecValues(1) = 2; vecValues(2) = 3;
   boost::rational<int> const theRatio { 1, 2 };
   using point_ty = boost::geometry::model::point<double, 2, boost::geometry::cs::cartesian>;
   point_ty theA {};
   point_ty theB {};
   boost::geometry::set<0>(theA, 0.0);
   boost::geometry::set<1>(theA, 0.0);
   boost::geometry::set<0>(theB, 3.0);
   boost::geometry::set<1>(theB, 4.0);
   return (iBig >> 100) == 1 && vecValues(2) == 3 && theRatio.numerator() == 1 &&
      boost::geometry::distance(theA, theB) == 5.0;
#elif ADECC_BOOST_GATE_CODE == 3
   std::string const strInput { "123" };
   auto theIterator = strInput.begin();
   int iValue = 0;
   bool const bParsed = boost::spirit::x3::parse(theIterator, strInput.end(), boost::spirit::x3::int_, iValue);
   using list_ty = boost::mp11::mp_list<int, double, char>;
   return bParsed && theIterator == strInput.end() && iValue == 123 && boost::mp11::mp_size<list_ty>::value == 3;
#elif ADECC_BOOST_GATE_CODE == 4
   struct value_ty { int iId; };
   using namespace boost::multi_index;
   using container_ty = multi_index_container<value_ty, indexed_by<hashed_unique<member<value_ty, int, &value_ty::iId>>>>;
   container_ty theValues;
   theValues.insert(value_ty { 7 });
   boost::adjacency_list<> theGraph(3);
   add_edge(0, 1, theGraph);
   boost::uuids::string_generator theGenerator;
   auto const theUuid = theGenerator("01234567-89ab-cdef-0123-456789abcdef");
   return theValues.find(7) != theValues.end() && num_edges(theGraph) == 1 && !theUuid.is_nil();
#elif ADECC_BOOST_GATE_CODE == 5
   boost::lockfree::queue<int> theQueue(8);
   theQueue.push(23);
   int iValue = 0;
   bool const bPopped = theQueue.pop(iValue);
   boost::interprocess::shared_memory_object::remove("BuildEngineBoostEvidenceUnused");
   return bPopped && iValue == 23;
#elif ADECC_BOOST_GATE_CODE == 6
#  if ADECC_BOOST_RUNTIME_CASE == 1
   int iValue = 0;
   std::string_view const svNumber { "192" };
   auto const theParse = boost::charconv::from_chars(svNumber.data(), svNumber.data() + svNumber.size(), iValue);
   return theParse.ec == std::errc {} && iValue == 192;
#  elif ADECC_BOOST_RUNTIME_CASE == 2
   return RunSerializationDiagnostics();
#  elif ADECC_BOOST_RUNTIME_CASE == 3
   auto const theUrl = boost::urls::parse_uri("https://example.invalid/boost");
   return theUrl.has_value() && theUrl->scheme() == "https";
#  else
   return false;
#  endif
#elif ADECC_BOOST_GATE_CODE == 7
   boost::dynamic_bitset<> theBits(8);
   theBits.set(3);
   boost::optional<int> const theOptional { 23 };
   boost::signals2::signal<int(int)> theSignal;
   theSignal.connect([](int const iValue) { return iValue + 1; });
   boost::variant2::variant<int, std::string> const theVariant { std::string { "boost" } };
   auto const theResult = theSignal(41);
   return theBits.test(3) && theOptional && *theOptional == 23 && theResult && *theResult == 42 &&
      boost::variant2::get<std::string>(theVariant) == "boost";
#else
   return false;
#endif
}
}

int main() {
   return RunGate() ? 0 : 1;
}
