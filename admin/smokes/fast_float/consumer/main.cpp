#include <fast_float/fast_float.h>
#include <string_view>
#include <system_error>
int main() {
   constexpr std::string_view input = "123.125";
   double value = 0.0;
   auto const parsed = fast_float::from_chars(input.data(), input.data()+input.size(), value);
   return parsed.ec == std::errc{} && parsed.ptr == input.data()+input.size() && value == 123.125 ? 0 : 1;
}
