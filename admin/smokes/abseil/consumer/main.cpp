#include <absl/strings/str_cat.h>
#include <string>
int main() {
   return absl::StrCat("PDFium-", 153) == std::string("PDFium-153") ? 0 : 1;
}
