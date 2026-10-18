// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkSurface.h"
#include "include/ports/SkFontMgr_empty.h"
#include "include/pathops/SkPathOps.h"
#include "include/core/SkPath.h"

#include <cstdio>

int main()
{
   auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(16, 16));
   auto fontManager = SkFontMgr_New_Custom_Empty();

   SkPath first = SkPath::Rect(SkRect::MakeXYWH(0, 0, 8, 8));
   SkPath second = SkPath::Rect(SkRect::MakeXYWH(4, 4, 8, 8));
   auto combined = Op(first, second, kUnion_SkPathOp);

   bool const success = surface != nullptr && fontManager != nullptr &&
                        combined.has_value();

   std::printf("SMOKE|CHECK|surface|%s|Skia 144 raster surface\n",
               surface ? "PASS" : "FAIL");
   std::printf("SMOKE|CHECK|fontmgr-empty|%s|Skia 144 FreeType empty font manager\n",
               fontManager ? "PASS" : "FAIL");
   std::printf("SMOKE|CHECK|pathops|%s|Skia 144 union operation\n",
               combined ? "PASS" : "FAIL");
   std::printf("SMOKE|RESULT|%s|Skia 144 installed consumer\n",
               success ? "PASS" : "FAIL");
   return success ? 0 : 1;
}
