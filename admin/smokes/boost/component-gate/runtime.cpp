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
#  include <boost/archive/text_iarchive.hpp>
#  include <boost/archive/text_oarchive.hpp>
#  include <boost/charconv.hpp>
#  include <boost/serialization/string.hpp>
#  include <boost/url.hpp>
#elif ADECC_BOOST_GATE_CODE == 7
#  include <boost/dynamic_bitset.hpp>
#  include <boost/optional.hpp>
#  include <boost/signals2.hpp>
#  include <boost/variant2/variant.hpp>
#endif

#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace {
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
   int iValue = 0;
   std::string_view const svNumber { "192" };
   auto const theParse = boost::charconv::from_chars(svNumber.data(), svNumber.data() + svNumber.size(), iValue);
   std::stringstream theStream;
   std::string const strOriginal { "boost-serialization" };
   {
      boost::archive::text_oarchive theArchive(theStream);
      theArchive << strOriginal;
   }
   std::string strRoundTrip;
   {
      boost::archive::text_iarchive theArchive(theStream);
      theArchive >> strRoundTrip;
   }
   auto const theUrl = boost::urls::parse_uri("https://example.invalid/boost");
   return theParse.ec == std::errc {} && iValue == 192 && strRoundTrip == strOriginal &&
      theUrl.has_value() && theUrl->scheme() == "https";
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
