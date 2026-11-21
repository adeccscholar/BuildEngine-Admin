#include <xtl/xoptional.hpp>
#include <print>

int main() {
   xtl::xoptional<int> const theValue { 7 };
   bool const bHasValue = theValue.has_value();
   std::println("SMOKE|CHECK|has-value|{}|xtl optional reports a stored value", bHasValue ? "PASS" : "FAIL");

   bool const bValue = bHasValue && theValue.value() == 7;
   std::println("SMOKE|CHECK|stored-value|{}|xtl optional preserves the integer value", bValue ? "PASS" : "FAIL");

   bool const bPassed = bHasValue && bValue;
   std::println("SMOKE|RESULT|{}|xtl optional", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
