#include <pugixml.hpp>

#include <cmath>
#include <print>
#include <string_view>

namespace {
bool Check(std::string_view const svId, bool const bPassed, std::string_view const svDetail) {
   std::println("SMOKE|CHECK|{}|{}|{}", svId, bPassed ? "PASS" : "FAIL", svDetail);
   return bPassed;
}
}

int main() {
   bool bSuccess = true;
#ifdef PUGIXML_CHARCONV_FLOAT
   bSuccess = Check("package", true, "PUGIXML_CHARCONV_FLOAT propagated") && bSuccess;
#else
   bSuccess = Check("package", false, "PUGIXML_CHARCONV_FLOAT missing") && bSuccess;
#endif
   pugi::xml_document theDocument;
   pugi::xml_node theRoot = theDocument.append_child("root");
   pugi::xml_node theValue = theRoot.append_child("value");
   constexpr double flExpected = 1234.125;
   bool const bSet = theValue.text().set(flExpected);
   bSuccess = Check("write", bSet, "double value written") && bSuccess;
   pugi::xpath_node const theSelected = theDocument.select_node("/root/value");
   bool const bRoundtrip = theSelected && std::abs(theSelected.node().text().as_double() - flExpected) < 1.0e-12;
   bSuccess = Check("runtime", bRoundtrip, "XPath and double roundtrip") && bSuccess;
   std::println("SMOKE|RESULT|{}|pugixml consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
