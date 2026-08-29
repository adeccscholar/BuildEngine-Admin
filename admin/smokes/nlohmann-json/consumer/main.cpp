#include <nlohmann/json.hpp>

#include <print>
#include <string>
#include <string_view>

namespace {
bool Check(std::string_view const svId, bool const bPassed, std::string_view const svDetail) {
   std::println("SMOKE|CHECK|{}|{}|{}", svId, bPassed ? "PASS" : "FAIL", svDetail);
   return bPassed;
}
}

int main() {
   bool bSuccess = true;
   nlohmann::json const theJson = nlohmann::json::parse(R"({"value":42,"name":"BuildEngine"})");
   bSuccess = Check("parse", theJson.at("value").get<int>() == 42, "JSON parse succeeded") && bSuccess;
   std::string const strSerialized = theJson.dump();
   bSuccess = Check("serialize", strSerialized.find("BuildEngine") != std::string::npos, "JSON serialization succeeded") && bSuccess;
   std::println("SMOKE|RESULT|{}|nlohmann-json consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
