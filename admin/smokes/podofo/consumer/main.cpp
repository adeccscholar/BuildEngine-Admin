#include <podofo/podofo.h>

#include <exception>
#include <print>

int main() {
   try {
      PoDoFo::PdfMemDocument theDocument;

      std::println("SMOKE|CHECK|podofo-document|PASS|PoDoFo created an in-memory PDF document");
      std::println("SMOKE|RESULT|PASS|PoDoFo consumer linked and executed successfully");
      return 0;
      }
   catch (std::exception const& theException) {
      std::println("SMOKE|CHECK|podofo-document|FAIL|{}", theException.what());
      std::println("SMOKE|RESULT|FAIL|PoDoFo consumer validation failed");
      return 2;
      }
   }
