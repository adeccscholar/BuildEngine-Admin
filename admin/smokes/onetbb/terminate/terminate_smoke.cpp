// SPDX-License-Identifier: MIT
// Process-isolated verification: never longjmp out of std::terminate.
#include <oneapi/tbb/global_control.h>
#include <oneapi/tbb/parallel_for.h>

#include <windows.h>
#include <exception>
#include <print>
#include <string>
#include <vector>

namespace {
constexpr DWORD iInternalExit = 77;
constexpr DWORD iUserExit = 78;

void OnTerminateInternal() noexcept {
   TerminateProcess(GetCurrentProcess(), iInternalExit);
}
void OnTerminateUser() noexcept {
   TerminateProcess(GetCurrentProcess(), iUserExit);
}

bool Child(char const* const szCase) {
   std::wstring wszExecutable(MAX_PATH, L'\0');
   DWORD const n = GetModuleFileNameW(nullptr, wszExecutable.data(),
      static_cast<DWORD>(wszExecutable.size()));
   if(n == 0 || n >= wszExecutable.size()) return false;
   wszExecutable.resize(n);

   std::wstring wszCommand = L"\"" + wszExecutable + L"\" --child-";
   for(char const* p = szCase; *p; ++p)
      wszCommand.push_back(static_cast<wchar_t>(*p));
   std::vector<wchar_t> vecCommand(wszCommand.begin(), wszCommand.end());
   vecCommand.push_back(L'\0');

   STARTUPINFOW theStartup {};
   theStartup.cb = sizeof(theStartup);
   PROCESS_INFORMATION theProcess {};
   if(!CreateProcessW(wszExecutable.c_str(), vecCommand.data(), nullptr,
      nullptr, FALSE, 0, nullptr, nullptr, &theStartup, &theProcess)) return false;

   DWORD const iWait = WaitForSingleObject(theProcess.hProcess, 30000);
   if(iWait != WAIT_OBJECT_0) {
      TerminateProcess(theProcess.hProcess, 99);
      WaitForSingleObject(theProcess.hProcess, INFINITE);
   }
   DWORD iExit = 0;
   bool const bExit = iWait == WAIT_OBJECT_0 &&
      GetExitCodeProcess(theProcess.hProcess, &iExit);
   CloseHandle(theProcess.hThread);
   CloseHandle(theProcess.hProcess);
   return bExit && iExit == (std::string(szCase) == "internal" ? iInternalExit : iUserExit);
}
}

int main(int const argc, char const* const* const argv) {
   if(argc == 2) {
      if(std::string(argv[1]) == "--child-internal") {
         std::set_terminate(OnTerminateInternal);
         oneapi::tbb::global_control const theControl {
            oneapi::tbb::global_control::terminate_on_exception, 1
         };
         oneapi::tbb::parallel_for(0, 1, -1, [](int) {});
         return 1;
      }
      if(std::string(argv[1]) == "--child-user") {
         std::set_terminate(OnTerminateUser);
         oneapi::tbb::global_control const theControl {
            oneapi::tbb::global_control::terminate_on_exception, 1
         };
         oneapi::tbb::parallel_for(0, 64, [](int) {
            throw std::exception {};
         });
         return 1;
      }
      return 2;
   }
   bool const bInternal = Child("internal");
   std::println("SMOKE|CHECK|terminate-internal|{}|isolated invalid-range termination",
      bInternal ? "PASS" : "FAIL");
   bool const bUser = Child("user");
   std::println("SMOKE|CHECK|terminate-user|{}|isolated worker-exception termination",
      bUser ? "PASS" : "FAIL");
   bool const bResult = bInternal && bUser;
   std::println("SMOKE|RESULT|{}|oneTBB terminate_on_exception isolated",
      bResult ? "PASS" : "FAIL");
   return bResult ? 0 : 1;
}
