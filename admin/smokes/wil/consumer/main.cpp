// SPDX-FileCopyrightText: 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: PolyForm-Noncommercial-1.0.0
//
// This is a real BCC64X compile/link/run smoke for the upstream WIL headers.
// No MSVC runtime, MinGW toolchain, fake WIL types or camera is involved.

#include <windows.h>
#include <unknwn.h>
#include <wil/com.h>
#include <wil/resource.h>
#include <atomic>
#include <print>
#include <utility>

namespace {
   /** \brief Lifetime counters that remain alive after the COM object is deleted.
       \details Used to verify the Release and QueryInterface semantics.
   */
   struct ComCounters {
      std::atomic<unsigned> uActive{0};
      std::atomic<unsigned> uDestroyed{0};
      std::atomic<unsigned> uAddRef{0};
      std::atomic<unsigned> uRelease{0};
      std::atomic<unsigned> uQuery{0};
   };

   /** \brief Minimal test IUnknown implementation with ordinary COM ownership.
       \details Starts with one reference. QueryInterface supports IUnknown only.
       \note The mock is single-threaded; its reference counter is atomic for clarity.
   */
   class MockUnknown final : public IUnknown {
      ComCounters& m_aCounters;
      std::atomic<ULONG> m_uReferences{1};

   public:
      /** \brief Initialize a COM object with one owned reference.
          \param aCounters Lifetime counters.
          \throw Nothing.
      */
      explicit MockUnknown(ComCounters& aCounters) noexcept :
         m_aCounters(aCounters) {
         ++m_aCounters.uActive;
      }

      /** \brief Record destruction.
          \details Called only when Release reaches zero.
          \throw Nothing.
      */
      ~MockUnknown() noexcept {
         --m_aCounters.uActive;
         ++m_aCounters.uDestroyed;
      }

      /** \brief Return the IUnknown interface with AddRef.
          \param theId Requested IID.
          \param ppObject Output COM interface pointer.
          \returns S_OK for IUnknown, E_POINTER for null output, otherwise E_NOINTERFACE.
          \throw Nothing.
      */
      HRESULT STDMETHODCALLTYPE QueryInterface(REFIID theId,void** ppObject) noexcept override {
         ++m_aCounters.uQuery;
         if(!ppObject) return E_POINTER;
         *ppObject=nullptr;
         if(!IsEqualIID(theId,__uuidof(IUnknown))) return E_NOINTERFACE;
         *ppObject=static_cast<IUnknown*>(this);
         AddRef();
         return S_OK;
      }

      /** \brief Retain one COM reference.
          \returns New reference count.
          \throw Nothing.
      */
      ULONG STDMETHODCALLTYPE AddRef() noexcept override {
         ++m_aCounters.uAddRef;
         return ++m_uReferences;
      }

      /** \brief Release one COM reference and destroy at zero.
          \returns Remaining reference count.
          \throw Nothing.
      */
      ULONG STDMETHODCALLTYPE Release() noexcept override {
         ++m_aCounters.uRelease;
         ULONG const uRemaining=--m_uReferences;
         if(!uRemaining) delete this;
         return uRemaining;
      }
   };

   /** \brief Check an observable assertion and print its result.
       \param bCondition True when the condition passes.
       \param svName Stable smoke-test label.
       \returns The supplied condition.
       \throw Any exception from std::println.
   */
   bool Check(bool bCondition,char const* szName) {
      std::println("SMOKE|CHECK|{}|{}|WIL C++23 BCC64X COM semantics",
         szName,bCondition?"PASS":"FAIL");
      return bCondition;
   }
}

/** \brief Verify the upstream WIL interface target and COM lifetime operations.
    \details This program does not open a device, depend on COM activation
    or require the Windows camera permission. It uses the actual WIL headers.
    \returns Zero on success, one on failed assertions.
    \throw Any exception from the standard output library.
*/
int main() {
   bool bPass=true;
   ComCounters aCounters;

   {
      wil::com_ptr_nothrow<IUnknown> theObject;
      theObject.attach(new MockUnknown(aCounters));
      bPass=Check(bool(theObject) && aCounters.uActive==1,"attach") && bPass;

      {
         wil::com_ptr_nothrow<IUnknown> theCopy=theObject;
         bPass=Check(bool(theCopy) && aCounters.uAddRef==1,"copy-addref") && bPass;

         wil::com_ptr_nothrow<IUnknown> theMoved=std::move(theCopy);
         bPass=Check(!theCopy && bool(theMoved),"move-no-addref") && bPass;

         wil::com_ptr_nothrow<IUnknown> theQueried;
         HRESULT const hr=theObject.query_to(theQueried.put());
         bPass=Check(hr==S_OK && bool(theQueried) &&
            aCounters.uQuery==1 && aCounters.uAddRef==2,"query-interface") && bPass;

         IUnknown* pRaw=theQueried.detach();
         bPass=Check(!theQueried && pRaw!=nullptr,"detach") && bPass;
         theQueried.attach(pRaw);
         bPass=Check(bool(theQueried),"attach-owned") && bPass;
      }

      bPass=Check(aCounters.uActive==1 &&
         aCounters.uRelease==2,"scope-releases") && bPass;
      theObject.reset();
   }

   bPass=Check(aCounters.uActive==0 && aCounters.uDestroyed==1 &&
      aCounters.uRelease==3,"final-release") && bPass;

   std::println("SMOKE|RESULT|{}|WIL header-only COM consumer",
      bPass?"PASS":"FAIL");
   return bPass?0:1;
}
