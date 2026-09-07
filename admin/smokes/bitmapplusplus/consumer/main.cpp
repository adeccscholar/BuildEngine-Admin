#include <BitmapPlusPlus.hpp>

#include <filesystem>
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
   std::filesystem::path const theFile = "bitmapplusplus-smoke.bmp";

   bmp::Bitmap theImage(8, 8);
   theImage.clear(bmp::Black);
   theImage.set(2, 3, bmp::Red);
   theImage.fill_rect(4, 1, 2, 3, bmp::Green);
   theImage.save(theFile);

   bSuccess = Check("write", std::filesystem::is_regular_file(theFile), "BMP file written") && bSuccess;

   bmp::Bitmap const theReloaded(theFile.string());
   bool const bShape = theReloaded.width() == 8 && theReloaded.height() == 8;
   bSuccess = Check("shape", bShape, "8x8 image roundtrip") && bSuccess;

   bool const bPixel = theReloaded.get(2, 3) == bmp::Red && theReloaded.get(4, 1) == bmp::Green;
   bSuccess = Check("pixel", bPixel, "pixel values roundtrip") && bSuccess;

   std::error_code theError;
   std::filesystem::remove(theFile, theError);

   std::println("SMOKE|RESULT|{}|BitmapPlusPlus 1.1.1 consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
