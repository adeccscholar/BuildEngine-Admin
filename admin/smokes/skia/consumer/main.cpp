// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkData.h"
#include "include/core/SkDocument.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkRect.h"
#include "include/core/SkSize.h"
#include "include/core/SkStream.h"
#include "include/core/SkSurface.h"
#include "include/docs/SkPDFDocument.h"
#include "include/docs/SkPDFJpegHelpers.h"
#include "include/encode/SkJpegEncoder.h"
#include "include/encode/SkPngEncoder.h"
#include "include/encode/SkWebpEncoder.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/ports/SkTypeface_win.h"
#include "modules/skottie/include/Skottie.h"
#include "modules/skshaper/include/SkShaper.h"
#include "modules/skunicode/include/SkUnicode_icu.h"
#include "modules/svg/include/SkSVGDOM.h"

#include <cstdio>
#include <cstring>

namespace {

bool Check(bool condition, char const* id, char const* detail)
{
   std::printf("SMOKE|CHECK|%s|%s|%s\n", id, condition ? "PASS" : "FAIL", detail);
   return condition;
}

}

int main()
{
   bool result = true;

   auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(64, 64));
   result &= Check(surface != nullptr, "surface", "CPU raster surface created");
   if(!surface)
   {
      std::printf("SMOKE|RESULT|FAIL|Skia desktop consumer\n");
      return 1;
   }

   SkCanvas* canvas = surface->getCanvas();
   canvas->clear(SK_ColorWHITE);

   SkPaint paint;
   paint.setColor(SK_ColorBLUE);
   canvas->drawRect(SkRect::MakeWH(32.0f, 32.0f), paint);
   result &= Check(surface->width() == 64 && surface->height() == 64,
                   "dimensions", "64x64 raster surface retained");

   SkPixmap pixmap;
   bool const havePixels = surface->peekPixels(&pixmap);
   result &= Check(havePixels, "pixels", "raster surface exposes pixels");

   if(havePixels)
   {
      auto png = SkPngEncoder::Encode(pixmap, {});
      auto jpeg = SkJpegEncoder::Encode(pixmap, {});
      auto webp = SkWebpEncoder::Encode(pixmap, {});

      result &= Check(png != nullptr, "png-encode", "PNG encoder produced data");
      result &= Check(jpeg != nullptr, "jpeg-encode", "JPEG encoder produced data");
      result &= Check(webp != nullptr, "webp-encode", "WebP encoder produced data");

      result &= Check(png && SkImages::DeferredFromEncodedData(png) != nullptr,
                      "png-decode", "PNG data accepted by Skia decoder");
      result &= Check(jpeg && SkImages::DeferredFromEncodedData(jpeg) != nullptr,
                      "jpeg-decode", "JPEG data accepted by Skia decoder");
      result &= Check(webp && SkImages::DeferredFromEncodedData(webp) != nullptr,
                      "webp-decode", "WebP data accepted by Skia decoder");
   }

   auto fontManager = SkFontMgr_New_DirectWrite();
   result &= Check(fontManager != nullptr, "directwrite", "DirectWrite font manager created");

   auto unicode = SkUnicodes::ICU::Make();
   result &= Check(unicode != nullptr, "unicode-icu", "ICU-backed SkUnicode created");

   auto shaper = SkShaper::Make(fontManager);
   result &= Check(shaper != nullptr, "harfbuzz-shaper", "SkShaper created with HarfBuzz/Unicode support");

   char const svg[] =
      "<svg xmlns='http://www.w3.org/2000/svg' width='16' height='16'>"
      "<rect width='16' height='16' fill='red'/></svg>";
   SkMemoryStream svgStream(svg, std::strlen(svg));
   auto svgDom = SkSVGDOM::MakeFromStream(svgStream);
   result &= Check(svgDom != nullptr, "svg", "SVG DOM parsed through Expat-backed module");
   if(svgDom)
   {
      svgDom->setContainerSize(SkSize::Make(16.0f, 16.0f));
      svgDom->render(canvas);
   }

   SkDynamicMemoryWStream pdfStream;
   auto pdf = SkPDF::MakeDocument(&pdfStream, SkPDF::JPEG::MetadataWithCallbacks());
   result &= Check(pdf != nullptr, "pdf", "PDF document backend created");
   if(pdf)
   {
      SkCanvas* pdfCanvas = pdf->beginPage(32.0f, 32.0f);
      pdfCanvas->clear(SK_ColorWHITE);
      pdf->endPage();
      pdf->close();
      result &= Check(pdfStream.bytesWritten() != 0, "pdf-output", "PDF backend produced bytes");
   }

   char const lottie[] =
      "{\"v\":\"5.7.4\",\"fr\":30,\"ip\":0,\"op\":1,\"w\":16,\"h\":16,\"layers\":[]}";
   SkMemoryStream lottieStream(lottie, std::strlen(lottie));
   auto animation = skottie::Animation::Make(&lottieStream);
   result &= Check(animation != nullptr, "skottie", "minimal Lottie animation parsed");
   if(animation)
   {
      animation->render(canvas);
   }

   // No WGL context is created by this package smoke. Calling the factory still
   // exercises the exported Ganesh/OpenGL consumer ABI and the OpenGL runtime load path.
   auto glContext = GrDirectContexts::MakeGL();
   result &= Check(true, "ganesh-opengl", glContext ? "Ganesh GL context created" :
                                             "Ganesh GL factory invoked without a current WGL context");

   std::printf("SMOKE|RESULT|%s|Skia desktop consumer\n", result ? "PASS" : "FAIL");
   return result ? 0 : 2;
}
