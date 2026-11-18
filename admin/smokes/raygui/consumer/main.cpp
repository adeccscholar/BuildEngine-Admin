#define RAYGUI_IMPLEMENTATION
#include <raygui.h>
#include <print>
int main() {
   int const iTextSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
   bool const bPassed = iTextSize > 0;
   std::println("SMOKE|CHECK|style|{}|raygui interface and raylib API usable",
      bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|raygui header-only consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
