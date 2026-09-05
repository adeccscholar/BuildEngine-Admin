// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"

int main() {
    auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(64, 64));
    if (!surface) return 1;

    SkCanvas* canvas = surface->getCanvas();
    canvas->clear(SK_ColorWHITE);

    SkPaint paint;
    paint.setColor(SK_ColorBLUE);
    canvas->drawRect(SkRect::MakeWH(32.0f, 32.0f), paint);
    return surface->width() == 64 && surface->height() == 64 ? 0 : 2;
}
