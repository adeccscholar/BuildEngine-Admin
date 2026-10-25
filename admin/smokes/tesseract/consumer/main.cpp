#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <print>
#include <string>
int main() {
   tesseract::TessBaseAPI theApi;
   auto const strVersion=std::string{theApi.Version()};
   bool const bVersion=!strVersion.empty();
   std::println("SMOKE|CHECK|api-version|{}|Tesseract {}",bVersion?"PASS":"FAIL",strVersion);
   PIX* pPix=pixCreate(160,48,8);
   bool const bImage=pPix!=nullptr;
   if(pPix) {
      pixSetAll(pPix);
      pixDestroy(&pPix);
   }
   std::println("SMOKE|CHECK|leptonica-interop|{}|PIX allocation and cleanup",bImage?"PASS":"FAIL");
   // Model-free ABI test. A separate OCR-data smoke will validate traineddata.
   std::println("SMOKE|RESULT|{}|Tesseract dynamic C++ API consumer",
      bVersion && bImage?"PASS":"FAIL");
   return bVersion && bImage?0:1;
}
