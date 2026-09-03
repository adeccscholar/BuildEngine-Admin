#include "SmokeC.h"

#include <orbsvcs/CosNamingC.h>
#include <tao/PortableServer/PortableServer.h>
#include <tao/corba.h>

#include <windows.h>

#include <array>
#include <chrono>
#include <format>
#include <print>
#include <string>
#include <string_view>
#include <thread>

namespace {

class ChildProcess final {
   HANDLE mhProcess {};

public:
   ChildProcess() = default;

   ~ChildProcess() {
      Stop();
   }

   ChildProcess(ChildProcess const&) = delete;
   ChildProcess& operator=(ChildProcess const&) = delete;

   [[nodiscard]] bool Start(std::wstring const& strExecutable, std::wstring const& strArguments) {
      std::wstring strCommand = L"\"" + strExecutable + L"\" " + strArguments;
      STARTUPINFOW theStartup {};
      theStartup.cb = sizeof(theStartup);
      PROCESS_INFORMATION theProcess {};

      if(!::CreateProcessW(strExecutable.c_str(), strCommand.data(), nullptr, nullptr, FALSE,
                           CREATE_NO_WINDOW, nullptr, nullptr, &theStartup, &theProcess))
         return false;

      ::CloseHandle(theProcess.hThread);
      mhProcess = theProcess.hProcess;
      return true;
   }

   [[nodiscard]] bool Stop() noexcept {
      if(!mhProcess) return true;

      DWORD uExitCode {};
      if(::GetExitCodeProcess(mhProcess, &uExitCode) && uExitCode == STILL_ACTIVE)
         ::TerminateProcess(mhProcess, 0U);

      DWORD const uWait = ::WaitForSingleObject(mhProcess, 5000U);
      ::CloseHandle(mhProcess);
      mhProcess = nullptr;
      return uWait == WAIT_OBJECT_0;
   }
};

[[nodiscard]] bool Check(std::string_view const svId,
                         bool const bPassed,
                         std::string_view const svDetail) {
   std::println("SMOKE|CHECK|{}|{}|{}", svId, bPassed ? "PASS" : "FAIL", svDetail);
   return bPassed;
}

[[nodiscard]] std::wstring FindNamingService() {
   std::array<wchar_t, 32768> arrPath {};
   DWORD const uLength = ::SearchPathW(nullptr, L"tao_cosnaming.exe", nullptr,
                                       static_cast<DWORD>(arrPath.size()), arrPath.data(), nullptr);
   if(uLength == 0U || uLength >= arrPath.size()) return {};
   return std::wstring { arrPath.data(), uLength };
}

[[nodiscard]] CosNaming::NamingContext_ptr ConnectNaming(CORBA::ORB_ptr pOrb,
                                                          std::string const& strCorbaloc) {
   for(unsigned uAttempt = 0U; uAttempt < 50U; ++uAttempt) {
      try {
         CORBA::Object_var theObject = pOrb->string_to_object(strCorbaloc.c_str());
         CosNaming::NamingContext_var theNaming = CosNaming::NamingContext::_narrow(theObject.in());
         if(!CORBA::is_nil(theNaming.in())) {
            CosNaming::BindingList_var theBindings;
            CosNaming::BindingIterator_var theIterator;
            theNaming->list(0U, theBindings.out(), theIterator.out());
            return theNaming._retn();
         }
      }
      catch(CORBA::Exception const&) {
      }
      std::this_thread::sleep_for(std::chrono::milliseconds { 100 });
   }
   return CosNaming::NamingContext::_nil();
}

[[nodiscard]] bool RunNamingSmoke(int& iArgc, char* pArgv[]) {
   std::wstring const strNamingService = FindNamingService();
   if(strNamingService.empty()) return false;

   unsigned const uPort = 39000U + (::GetCurrentProcessId() % 1000U);
   ChildProcess theNamingProcess;
   if(!theNamingProcess.Start(strNamingService,
         std::format(L"-ORBEndpoint iiop://127.0.0.1:{}", uPort)))
      return false;

   CORBA::ORB_var theOrb = CORBA::ORB_init(iArgc, pArgv);
   if(CORBA::is_nil(theOrb.in())) return false;

   CORBA::Object_var thePoaObject = theOrb->resolve_initial_references("RootPOA");
   PortableServer::POA_var theRootPoa = PortableServer::POA::_narrow(thePoaObject.in());
   if(CORBA::is_nil(theRootPoa.in())) return false;

   std::string const strCorbaloc = std::format("corbaloc:iiop:127.0.0.1:{}/NameService", uPort);
   CosNaming::NamingContext_var theNaming = ConnectNaming(theOrb.in(), strCorbaloc);
   if(CORBA::is_nil(theNaming.in())) return false;

   CORBA::Object_var theReference = theRootPoa->create_reference("IDL:BuildEngineSmoke/Probe:1.0");
   if(CORBA::is_nil(theReference.in())) return false;

   CosNaming::Name theName;
   theName.length(1U);
   theName[0U].id = CORBA::string_dup("BuildEngineSmoke");
   theName[0U].kind = CORBA::string_dup("Probe");
   theNaming->rebind(theName, theReference.in());

   CORBA::Object_var theResolved = theNaming->resolve(theName);
   if(CORBA::is_nil(theResolved.in())) return false;

   CORBA::String_var strOriginalIor = theOrb->object_to_string(theReference.in());
   CORBA::String_var strResolvedIor = theOrb->object_to_string(theResolved.in());
   bool const bSameReference = std::string_view { strOriginalIor.in() } == strResolvedIor.in();

   theNaming->unbind(theName);
   theOrb->destroy();
   bool const bStopped = theNamingProcess.Stop();
   return bSameReference && bStopped;
}

}

int main(int iArgc, char* pArgv[]) {
   bool bSuccess = true;

   bSuccess = Check("idl", sizeof(BuildEngineSmoke::Probe_ptr) > 0U,
                    "tao_idl generated client stub compiled") && bSuccess;

   try {
      bool const bNaming = RunNamingSmoke(iArgc, pArgv);
      bSuccess = Check("runtime", bNaming,
                       "ORB initialized; Naming Service started; name rebound and resolved") && bSuccess;
   }
   catch(CORBA::Exception const&) {
      bSuccess = Check("runtime", false, "CORBA exception") && bSuccess;
   }
   catch(...) {
      bSuccess = Check("runtime", false, "unknown exception") && bSuccess;
   }

   std::println("SMOKE|RESULT|{}|ACE/TAO consumer usable", bSuccess ? "PASS" : "FAIL");
   return bSuccess ? 0 : 1;
}
