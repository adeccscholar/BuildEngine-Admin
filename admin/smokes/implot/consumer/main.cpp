#include <imgui.h>
#include <implot.h>
#include <array>
#include <print>
int main() {
   IMGUI_CHECKVERSION();
   ImGuiContext* const pImGui = ImGui::CreateContext();
   if(pImGui == nullptr) return 1;
   ImPlotContext* const pImPlot = ImPlot::CreateContext();
   ImGui::GetIO().DisplaySize = ImVec2 { 320.0F, 240.0F };
   ImGui::GetIO().DeltaTime = 1.0F / 60.0F;
   ImGui::NewFrame();
   ImGui::Begin("ImPlot");
   std::array<double, 4> const arrX { 0.0, 1.0, 2.0, 3.0 };
   std::array<double, 4> const arrY { 0.0, 1.0, 4.0, 9.0 };
   if(ImPlot::BeginPlot("Quadratic")) {
      ImPlot::PlotLine("y=x^2", arrX.data(), arrY.data(), 4);
      ImPlot::EndPlot();
   }
   ImGui::End();
   ImGui::Render();
   bool const bPassed = pImPlot != nullptr && ImGui::GetDrawData() != nullptr;
   ImPlot::DestroyContext(pImPlot);
   ImGui::DestroyContext(pImGui);
   std::println("SMOKE|CHECK|runtime|{}|ImPlot context and draw data created", bPassed ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|ImPlot DLL consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
