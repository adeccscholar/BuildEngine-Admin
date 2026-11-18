#include <imgui.h>
#include <print>
int main() {
   IMGUI_CHECKVERSION();
   ImGuiContext* const pContext = ImGui::CreateContext();
   if(pContext == nullptr) return 1;
   ImGui::SetCurrentContext(pContext);
   ImGuiIO& theIo = ImGui::GetIO();
   theIo.DisplaySize = ImVec2 { 320.0F, 240.0F };
   theIo.DeltaTime = 1.0F / 60.0F;
   ImGui::NewFrame();
   ImGui::Begin("BCC64X Smoke");
   ImGui::TextUnformatted("Dear ImGui dynamic consumer");
   ImGui::End();
   ImGui::Render();
   bool const bPassed = ImGui::GetDrawData() != nullptr;
   ImGui::DestroyContext(pContext);
   std::println("SMOKE|RESULT|{}|Dear ImGui DLL consumer", bPassed ? "PASS" : "FAIL");
   return bPassed ? 0 : 1;
}
