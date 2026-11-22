#include <windows.h>
#include <detours.h>
#include <atomic>
#include <print>

namespace {
   using SleepFn = void (WINAPI*)(DWORD);
   SleepFn pfnOriginalSleep = &::Sleep;
   std::atomic<unsigned> uHookInvocations = 0;
   void WINAPI HookedSleep(DWORD dwMilliseconds) {
      uHookInvocations.fetch_add(1, std::memory_order_relaxed);
      pfnOriginalSleep(dwMilliseconds);
   }
}

int main() {
   LONG lResult = DetourTransactionBegin();
   if(lResult == NO_ERROR) {
      lResult = DetourUpdateThread(GetCurrentThread());
   }
   if(lResult == NO_ERROR) {
      lResult = DetourAttach(reinterpret_cast<PVOID*>(&pfnOriginalSleep), reinterpret_cast<PVOID>(&HookedSleep));
   }
   if(lResult == NO_ERROR) {
      lResult = DetourTransactionCommit();
   }
   else {
      DetourTransactionAbort();
   }
   if(lResult != NO_ERROR) {
      std::println("SMOKE|RESULT|FAIL|Detours attach error={}", lResult);
      return 1;
   }
   ::Sleep(0);
   bool const bObserved = uHookInvocations.load(std::memory_order_relaxed) == 1;
   lResult = DetourTransactionBegin();
   if(lResult == NO_ERROR) {
      lResult = DetourUpdateThread(GetCurrentThread());
   }
   if(lResult == NO_ERROR) {
      lResult = DetourDetach(reinterpret_cast<PVOID*>(&pfnOriginalSleep), reinterpret_cast<PVOID>(&HookedSleep));
   }
   if(lResult == NO_ERROR) {
      lResult = DetourTransactionCommit();
   }
   else {
      DetourTransactionAbort();
   }
   bool const bOK = bObserved && lResult == NO_ERROR;
   std::println("SMOKE|CHECK|api-interception|{}|Attach, intercept and detach", bOK ? "PASS" : "FAIL");
   std::println("SMOKE|RESULT|{}|Native Detours DLL", bOK ? "PASS" : "FAIL");
   return bOK ? 0 : 1;
}
