#include <openssl/crypto.h>
#include <openssl/ssl.h>

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
   char const* const szVersion = OpenSSL_version(OPENSSL_VERSION);
   bSuccess = Check("runtime", szVersion != nullptr && *szVersion != '\0', "OpenSSL runtime version available") && bSuccess;
   int const iInitialized = OPENSSL_init_ssl(0U, nullptr);
   bSuccess = Check("ssl", iInitialized == 1, "OPENSSL_init_ssl succeeded") && bSuccess;
   SSL_CTX* const pContext = SSL_CTX_new(TLS_method());
   bSuccess = Check("context", pContext != nullptr, "TLS context created") && bSuccess;
   if(pContext != nullptr) SSL_CTX_free(pContext);
   std::println("SMOKE|RESULT|{}|OpenSSL consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
