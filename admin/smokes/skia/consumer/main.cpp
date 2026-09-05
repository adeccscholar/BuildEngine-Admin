// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"

#include <cstdio>

int main() {
   auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(64, 64));
   if(!surface) {
      std::printf("SMOKE|CHECK|surface|FAIL|SkSurfaces::Raster returned null\n");
      std::printf("SMOKE|RESULT|FAIL|Skia CPU raster consumer\n");
      return 1;
   }

   std::printf("SMOKE|CHECK|surface|PASS|CPU raster surface created\n");

   SkCanvas* canvas = surface->getCanvas();
   canvas->clear(SK_ColorWHITE);

   SkPaint paint;
   paint.setColor(SK_ColorBLUE);
   canvas->drawRect(SkRect::MakeWH(32.0f, 32.0f), paint);

   bool const bDimensions = surface->width() == 64 && surface->height() == 64;
   std::printf("SMOKE|CHECK|dimensions|%s|64x64 raster surface retained\n",
               bDimensions ? "PASS" : "FAIL");
   std::printf("SMOKE|RESULT|%s|Skia CPU raster consumer\n",
               bDimensions ? "PASS" : "FAIL");
   return bDimensions ? 0 : 2;
}
