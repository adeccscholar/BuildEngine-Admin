#include <xtl/xoptional.hpp>
#include <print>
int main() {
   xtl::xoptional<int> const theValue { 7 };
   bool const bPassed=theValue.has_value() && theValue.value()==7;
   std::println("SMOKE|RESULT|{}|xtl optional",bPassed?"PASS":"FAIL");
   return bPassed?0:1;
}
