#include <ZXing/ReadBarcode.h>
#include <ZXing/ImageView.h>
#include <print>
#include <array>

int main() {
   std::array<unsigned char, 64> const arrImage{};
   ZXing::ImageView const theImage{arrImage.data(), 8, 8, ZXing::ImageFormat::Lum};
   ZXing::Barcodes const theCodes = ZXing::ReadBarcodes(theImage);
   bool const bPassed = theCodes.empty();
   std::println("SMOKE|CHECK|empty-image|{}|ZXing reader handles an empty luminance image", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|ZXing reader-only consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
