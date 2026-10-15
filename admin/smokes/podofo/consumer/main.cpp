#include <podofo/podofo.h>
#include <exception>
#include <print>
int main() {
   try {
      PoDoFo::PdfMemDocument document;
      bool const empty = document.GetPages().GetCount() == 0U;
      std::println("SMOKE|CHECK|document-create|{}|empty document", empty ? "PASS" : "FAIL");
      if(!empty) {
         std::println("SMOKE|RESULT|FAIL|PoDoFo consumer");
         return 1;
      }
      document.GetPages().CreatePage(PoDoFo::PdfPageSize::A4);
      bool const ok = document.GetPages().GetCount() == 1U;
      std::println("SMOKE|CHECK|page-create|{}|A4 page created", ok ? "PASS" : "FAIL");
      std::println("SMOKE|RESULT|{}|PoDoFo consumer", ok ? "PASS" : "FAIL");
      return ok ? 0 : 1;
   } catch(std::exception const& error) {
      std::println("SMOKE|CHECK|document-create|FAIL|{}", error.what());
      std::println("SMOKE|RESULT|FAIL|PoDoFo exception");
      return 2;
   }
}
