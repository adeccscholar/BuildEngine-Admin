#include <curl/curl.h>

#include <print>
#include <string_view>

namespace {
bool Check(std::string_view const svId, bool const bPassed, std::string_view const svDetail) {
   std::println("SMOKE|CHECK|{}|{}|{}", svId, bPassed ? "PASS" : "FAIL", svDetail);
   return bPassed;
}

bool HasProtocol(curl_version_info_data const& theInfo, std::string_view const svProtocol) {
   if(theInfo.protocols == nullptr) return false;
   for(char const* const* pszProtocol = theInfo.protocols; *pszProtocol != nullptr; ++pszProtocol)
      if(svProtocol == *pszProtocol) return true;
   return false;
}
}

int main() {
   bool bSuccess = true;
   CURLcode const theInit = curl_global_init(CURL_GLOBAL_DEFAULT);
   bSuccess = Check("init", theInit == CURLE_OK, "curl_global_init succeeded") && bSuccess;
   curl_version_info_data const* const pInfo = curl_version_info(CURLVERSION_NOW);
   bSuccess = Check("runtime", pInfo != nullptr, "curl_version_info available") && bSuccess;
   if(pInfo != nullptr) {
      bSuccess = Check("https", HasProtocol(*pInfo, "https"), "HTTPS protocol enabled") && bSuccess;
      bool const bSsl = (pInfo->features & CURL_VERSION_SSL) != 0;
      bSuccess = Check("ssl", bSsl, "SSL/TLS feature enabled") && bSuccess;
      std::string_view const svSsl = pInfo->ssl_version != nullptr ? pInfo->ssl_version : "";
      bSuccess = Check("openssl", svSsl.find("OpenSSL") != std::string_view::npos, "OpenSSL TLS backend active") && bSuccess;
      bool const bZlib = (pInfo->features & CURL_VERSION_LIBZ) != 0;
      bSuccess = Check("zlib", bZlib, "zlib feature enabled") && bSuccess;
   }
   curl_global_cleanup();
   std::println("SMOKE|RESULT|{}|curl consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
