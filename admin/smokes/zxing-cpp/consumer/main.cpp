#include <ZXing/ReadBarcode.h>
#include <ZXing/CreateBarcode.h>
#include <ZXing/WriteBarcode.h>
#include <ZXing/ImageView.h>
#include <ZXing/ReaderOptions.h>
#include <zint.h>

#include <algorithm>
#include <array>
#include <exception>
#include <print>
#include <string_view>

int main() {
   try {
      std::array<unsigned char, 64> const arrBlank {};
      ZXing::ImageView const theBlank {arrBlank.data(), 8, 8, ZXing::ImageFormat::Lum};
      bool const bReader = ZXing::ReadBarcodes(theBlank).empty();
      std::println("SMOKE|CHECK|empty-image|{}|ZXing reader handles an empty luminance image",
         bReader ? "PASS" : "FAIL");

      bool const bZint = ZBarcode_ValidID(BARCODE_QRCODE) != 0;
      std::println("SMOKE|CHECK|external-zint|{}|Installed Zint DLL supports QR encoding",
         bZint ? "PASS" : "FAIL");

      std::string_view const svPayload = "BUILDENGINE-ZINT-2.16";
      ZXing::CreatorOptions const theCreate { ZXing::BarcodeFormat::QRCode };
      ZXing::Barcode const theBarcode = ZXing::CreateBarcodeFromText(svPayload, theCreate);
      bool const bCreated = theBarcode.isValid();
      std::println("SMOKE|CHECK|writer-create|{}|ZXing creates a QR barcode through the external Zint backend",
         bCreated ? "PASS" : "FAIL");

      bool bRoundtrip {};
      if(bCreated) {
         ZXing::WriterOptions theWriter;
         theWriter.scale(5);
         theWriter.addQuietZones(true);
         ZXing::Image const theImage = ZXing::WriteBarcodeToImage(theBarcode, theWriter);
         ZXing::Barcodes const vecDecoded = ZXing::ReadBarcodes(theImage);
         bRoundtrip = std::ranges::any_of(vecDecoded,
            [svPayload](ZXing::Barcode const& theDecoded) {
               return theDecoded.isValid() && theDecoded.text() == svPayload;
            });
      }
      std::println("SMOKE|CHECK|writer-roundtrip|{}|ZXing writer renders and reader decodes the original QR payload",
         bRoundtrip ? "PASS" : "FAIL");
      bool const bPassed = bReader && bZint && bCreated && bRoundtrip;
      std::println("SMOKE|RESULT|{}|ZXing native reader and externally linked Zint writer",
         bPassed ? "PASS" : "FAIL");
      return bPassed ? 0 : 1;
   }
   catch(std::exception const& theError) {
      std::println("SMOKE|CHECK|exception|FAIL|ZXing or Zint threw during native barcode creation");
      std::println("SMOKE|RESULT|FAIL|{}", theError.what());
      return 2;
   }
}
