#include <boost/config.hpp>
#include <boost/version.hpp>

#if defined(BOOST_EMBTC)
#  error "Managed Boost 1.92.0 consumer must bypass the historical Boost CodeGear layer"
#endif
#if defined(BOOST_NO_CXX11_NOEXCEPT)
#  error "Managed Boost 1.92.0 consumer requires native noexcept"
#endif
#if !defined(BOOST_CLANG)
#  error "Managed Boost 1.92.0 consumer expects native-Clang Boost.Config routing"
#endif

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
   bSuccess = Check("version", BOOST_VERSION == 109200, "Boost 1.92.0 selected") && bSuccess;
   bSuccess = Check("native-clang", true, "Boost.Config routes BCC64X through upstream Clang configuration") && bSuccess;
   bSuccess = Check("runtime-evidence", true, ADECC_BOOST_GATE_ID) && bSuccess;
   std::println("SMOKE|RESULT|{}|Boost {} consumer gate", bSuccess ? "PASS" : "FAIL", ADECC_BOOST_GATE_ID);
   return bSuccess ? 0 : 1;
}
