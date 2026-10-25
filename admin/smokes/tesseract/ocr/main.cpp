// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include "ocr_paths.h"
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>

/** \brief Recognize the pinned synthetic OCR sample.
    \details Reports model initialization, image loading, recognition and exact text.
    \returns Zero on success; nonzero on any failed assertion.
    \throw Nothing.
*/
int main() {
   std::filesystem::path const aModel=std::filesystem::path{szOcrTessdataDirectory}/"eng.traineddata";
   bool const bModel=std::filesystem::is_regular_file(aModel);
   std::cout<<"SMOKE|CHECK|model-present|"<<(bModel?"PASS":"FAIL")<<"|pinned English model\n";
   if(!bModel) return 2;
   std::filesystem::path const aImage{szOcrImagePath};
   bool const bFixture=std::filesystem::is_regular_file(aImage);
   std::cout<<"SMOKE|CHECK|fixture-present|"<<(bFixture?"PASS":"FAIL")<<"|synthetic PNG\n";
   if(!bFixture) return 3;
   tesseract::TessBaseAPI aApi;
   bool const bInit=aApi.Init(szOcrTessdataDirectory,"eng",tesseract::OEM_LSTM_ONLY)==0;
   std::cout<<"SMOKE|CHECK|model-init|"<<(bInit?"PASS":"FAIL")<<"|English LSTM model\n";
   if(!bInit) return 4;
   aApi.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
   using PixOwner=std::unique_ptr<PIX,void (*)(PIX*)>;
   std::string const strImage=aImage.string();
   PixOwner const theImage(pixRead(strImage.c_str()),[](PIX* pPix){pixDestroy(&pPix);});
   bool const bImage=static_cast<bool>(theImage);
   std::cout<<"SMOKE|CHECK|image-read|"<<(bImage?"PASS":"FAIL")<<"|Leptonica PNG\n";
   if(!bImage) return 5;
   aApi.SetImage(theImage.get());
   bool const bRecognition=aApi.Recognize(nullptr)==0;
   std::cout<<"SMOKE|CHECK|recognize|"<<(bRecognition?"PASS":"FAIL")<<"|OCR engine\n";
   if(!bRecognition) return 6;
   std::unique_ptr<char[]> const pText(aApi.GetUTF8Text());
   bool const bText=static_cast<bool>(pText);
   std::cout<<"SMOKE|CHECK|text-present|"<<(bText?"PASS":"FAIL")<<"|UTF-8 output\n";
   if(!bText) return 7;
   std::string_view svText(pText.get());
   while(!svText.empty()&&(svText.back()=='\n'||svText.back()=='\r'))
      svText.remove_suffix(1);
   bool const bExact=svText=="OCR TEST 4711";
   std::cout<<"SMOKE|CHECK|ocr-exact|"<<(bExact?"PASS":"FAIL")<<"|expected=OCR TEST 4711; actual="<<svText<<"\n";
   std::cout<<"SMOKE|RESULT|"<<(bExact?"PASS":"FAIL")<<"|Tesseract OCR recognition\n";
   return bExact?0:8;
}
