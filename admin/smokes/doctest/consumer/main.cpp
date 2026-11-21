#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
TEST_CASE("managed doctest arithmetic") {
   CHECK(6 * 7 == 42);
}
