#include <fast_float/fast_float.h>
#include <iostream>
#include <string_view>
#include <system_error>

int main() {
   constexpr std::string_view input = "123.125";
   double value = 0.0;
   auto const result = fast_float::from_chars(
      input.data(), input.data() + input.size(), value);
   bool const ok = result.ec == std::errc{} &&
      result.ptr == input.data() + input.size() && value == 123.125;
   std::cout << "SMOKE|CHECK|parse-decimal|" << (ok ? "PASS" : "FAIL")
             << "|from_chars exact roundtrip\n";
   std::cout << "SMOKE|RESULT|" << (ok ? "PASS" : "FAIL")
             << "|fast_float consumer\n";
   return ok ? 0 : 1;
}
