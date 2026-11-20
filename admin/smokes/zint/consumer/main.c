#include <zint.h>
#include <stdio.h>
#include <string.h>

static int Check(char const *szId, int bPassed, char const *szDetail) {
   printf("SMOKE|CHECK|%s|%s|%s\n", szId, bPassed ? "PASS" : "FAIL", szDetail);
   return bPassed;
}

int main(void) {
   struct zint_symbol *pSymbol = ZBarcode_Create();
   if(pSymbol == NULL) {
      Check("allocation", 0, "ZBarcode_Create returned null");
      puts("SMOKE|RESULT|FAIL|Zint allocation");
      return 1;
   }
   pSymbol->symbology = BARCODE_QRCODE;
   unsigned char const szData[] = "BUILDENGINE-ZINT-2.16";
   int const iResult = ZBarcode_Encode(pSymbol, szData, (int)strlen((char const *)szData));
   int const bEncoded = iResult < ZINT_ERROR && pSymbol->rows > 0 && pSymbol->width > 0;
   Check("qr-encode", bEncoded, "Native Zint DLL generates QR modules");
   int const iVectorResult = bEncoded ? ZBarcode_Buffer_Vector(pSymbol, 0) : ZINT_ERROR_INVALID_DATA;
   int const bVector = iVectorResult < ZINT_ERROR &&
      pSymbol->vector != NULL && pSymbol->vector->width > 0;
   Check("vector", bVector, "Native vector barcode representation is available");
   ZBarcode_Delete(pSymbol);
   printf("SMOKE|RESULT|%s|Zint QR creation and vector output\n",
          bEncoded && bVector ? "PASS" : "FAIL");
   return bEncoded && bVector ? 0 : 1;
}
