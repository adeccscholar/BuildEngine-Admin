#include <winmd_reader.h>
#include <print>
#include <type_traits>
int main() {
   bool const bOK = std::is_class_v<winmd::reader::database>;
   std::println("SMOKE|CHECK|winmd-reader|{}|Native reader API", bOK ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Microsoft WinMD Reader", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
