#include <qpdf/QPDF.hh>

#include <exception>
#include <print>

int main() {
   try {
      QPDF theDocument;
      theDocument.emptyPDF();

      std::println("SMOKE|CHECK|qpdf-empty-document|PASS|QPDF created an empty PDF document");
      std::println("SMOKE|RESULT|PASS|QPDF consumer linked and executed successfully");
      return 0;
      }
   catch (std::exception const& theException) {
      std::println("SMOKE|CHECK|qpdf-empty-document|FAIL|{}", theException.what());
      std::println("SMOKE|RESULT|FAIL|QPDF consumer validation failed");
      return 2;
      }
   }
