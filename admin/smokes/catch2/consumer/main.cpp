#define CATCH_CONFIG_RUNNER

#include <catch2/catch_session.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <array>
#include <deque>
#include <print>
#include <string_view>
#include <vector>

TEST_CASE("Catch2 basic assertions", "[buildengine]") {
   std::array<int, 4> const aiValues{1, 2, 3, 4};
   REQUIRE(aiValues.size() == 4);
   REQUIRE(aiValues[2] == 3);
}

TEMPLATE_PRODUCT_TEST_CASE(
   "Catch2 Clang 20 template product path",
   "[buildengine][template-product]",
   (std::vector, std::deque),
   (int, double)) {
   TestType theValues{};
   REQUIRE(theValues.empty());
   theValues.push_back({});
   REQUIRE(theValues.size() == 1);
}

int main(int argc, char* argv[]) {
   std::println("SMOKE|CHECK|package|PASS|Catch2 3.16 package found");

   Catch::Session theSession;
   int const iResult = theSession.run(argc, argv);

   bool const bSuccess = iResult == 0;
   std::println("SMOKE|CHECK|runtime|{}|Catch2 test session executed", bSuccess ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Catch2 3.16 consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : iResult;
}
