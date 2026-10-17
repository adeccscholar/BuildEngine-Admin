// SPDX-License-Identifier: MIT
#include <fpdfview.h>
#include <windows.h>
#include <array>
#include <format>
#include <print>
#include <string>
#include <string_view>
int main() {
   constexpr std::array<std::string_view, 4> arrObjects {{
      "1 0 obj\n<< /Type /Catalog /Pages 2 0 R >>\nendobj\n",
      "2 0 obj\n<< /Type /Pages /Kids [3 0 R] /Count 1 >>\nendobj\n",
      "3 0 obj\n<< /Type /Page /Parent 2 0 R /MediaBox [0 0 72 72] /Resources << >> /Contents 4 0 R >>\nendobj\n",
      "4 0 obj\n<< /Length 0 >>\nstream\n\nendstream\nendobj\n"
   }};
   std::string strPdf = "%PDF-1.4\n";
   std::array<std::size_t, 5> arrOffsets {};
   for(std::size_t uIndex = 0; uIndex < arrObjects.size(); ++uIndex) {
      arrOffsets[uIndex + 1] = strPdf.size();
      strPdf.append(arrObjects[uIndex]);
   }
   std::size_t const uXref = strPdf.size();
   strPdf += "xref\n0 5\n0000000000 65535 f \n";
   for(std::size_t uIndex = 1; uIndex < arrOffsets.size(); ++uIndex)
      strPdf += std::format("{:010} 00000 n \n", arrOffsets[uIndex]);
   strPdf += std::format("trailer\n<< /Size 5 /Root 1 0 R >>\nstartxref\n{}\n%%EOF\n", uXref);
   FPDF_LIBRARY_CONFIG aConfig {};
   aConfig.version = 2;
   FPDF_InitLibraryWithConfig(&aConfig);
   FPDF_DOCUMENT const theDocument = FPDF_LoadMemDocument(strPdf.data(), static_cast<int>(strPdf.size()), nullptr);
   bool bOk = theDocument && FPDF_GetPageCount(theDocument) == 1;
   if(bOk) {
      FPDF_PAGE const thePage = FPDF_LoadPage(theDocument, 0);
      bOk = thePage && FPDF_GetPageWidthF(thePage) > 0.0f;
      if(thePage) {
         FPDF_BITMAP const theBitmap = FPDFBitmap_Create(72, 72, 1);
         bOk = bOk && theBitmap;
         if(theBitmap) {
            FPDFBitmap_FillRect(theBitmap, 0, 0, 72, 72, 0xFFFFFFFF);
            FPDF_RenderPageBitmap(theBitmap, thePage, 0, 0, 72, 72, 0, 0);
            bOk = bOk && FPDFBitmap_GetBuffer(theBitmap) != nullptr;
            FPDFBitmap_Destroy(theBitmap);
         }
         FPDF_ClosePage(thePage);
      }
   }
   if(theDocument) FPDF_CloseDocument(theDocument);
   FPDF_DestroyLibrary();
   HMODULE const hPdfium = ::GetModuleHandleW(L"pdfium.dll");
   bool const bDynamic = hPdfium != nullptr &&
      ::GetProcAddress(hPdfium, "FPDF_InitLibraryWithConfig") != nullptr &&
      ::GetProcAddress(hPdfium, "FPDF_RenderPageBitmap") != nullptr;
   std::println("SMOKE|CHECK|dll-loading|{}|installed PDFium DLL and public exports", bDynamic ? "PASS" : "FAIL");
   bOk = bOk && bDynamic;
   std::println("SMOKE|CHECK|memory-pdf-render|{}|installed PDFium", bOk ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|PDFium in-memory render", bOk ? "PASS" : "FAIL");
   return bOk ? 0 : 1;
}
