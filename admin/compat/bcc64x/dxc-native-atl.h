#pragma once

// Native-Windows equivalents of the ATL helper types used by DXC.
// This file is included only for the Windows BCC64X/Clang toolchain.
// It deliberately uses the Windows COM ABI and does not emulate Windows APIs.
#include <windows.h>
#include <objbase.h>
#include <oleauto.h>
#include <cassert>
#include <climits>
#include <intsafe.h>
#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>
#include <string>
#include <vector>
#include <utility>

#ifndef ATLASSERT
#define ATLASSERT(expression) assert(expression)
#endif

class AdeccDxcCAllocator {
public:
   static void* Allocate(std::size_t nBytes) noexcept {
      return ::CoTaskMemAlloc(nBytes);
   }
   static void* Reallocate(void* pData, std::size_t nBytes) noexcept {
      return ::CoTaskMemRealloc(pData, nBytes);
   }
   static void Free(void* pData) noexcept {
      ::CoTaskMemFree(pData);
   }
};

template<class T> class AdeccDxcCComPtr {
public:
   T* p {};

   AdeccDxcCComPtr() noexcept = default;
   AdeccDxcCComPtr(T* pValue) noexcept : p {pValue} {
      if(p) p->AddRef();
   }
   AdeccDxcCComPtr(AdeccDxcCComPtr const& theOther) noexcept : AdeccDxcCComPtr(theOther.p) {}
   template<class U> AdeccDxcCComPtr(AdeccDxcCComPtr<U> const& theOther) noexcept
      : AdeccDxcCComPtr(static_cast<T*>(theOther.p)) {}
   AdeccDxcCComPtr(AdeccDxcCComPtr&& theOther) noexcept : p {std::exchange(theOther.p, nullptr)} {}
   ~AdeccDxcCComPtr() noexcept { Release(); }

   AdeccDxcCComPtr& operator=(AdeccDxcCComPtr const& theOther) noexcept {
      if(this != std::addressof(theOther)) Assign(theOther.p);
      return *this;
   }
   AdeccDxcCComPtr& operator=(AdeccDxcCComPtr&& theOther) noexcept {
      if(this != std::addressof(theOther)) {
         Release();
         p = std::exchange(theOther.p, nullptr);
      }
      return *this;
   }
   T* operator=(T* pValue) noexcept { Assign(pValue); return p; }
   template<class U> AdeccDxcCComPtr& operator=(AdeccDxcCComPtr<U> const& theOther) noexcept {
      Assign(static_cast<T*>(theOther.p));
      return *this;
   }
   [[nodiscard]] operator T*() const noexcept { return p; }
   [[nodiscard]] T* operator->() const noexcept { return p; }
   [[nodiscard]] T* Get() const noexcept { return p; }
   [[nodiscard]] T** GetAddressOf() noexcept { return &p; }
   [[nodiscard]] T* const* GetAddressOf() const noexcept { return &p; }
   [[nodiscard]] T** operator&() noexcept {
      ATLASSERT(p == nullptr);
      return &p;
   }
   [[nodiscard]] bool operator!() const noexcept { return p == nullptr; }
   [[nodiscard]] bool operator==(T const* pOther) const noexcept { return p == pOther; }
   [[nodiscard]] bool operator!=(T const* pOther) const noexcept { return p != pOther; }

   void Attach(T* pValue) noexcept {
      if(p != pValue) { Release(); p = pValue; }
   }
   [[nodiscard]] T* Detach() noexcept { return std::exchange(p, nullptr); }
   void Release() noexcept {
      if(T* pOld = std::exchange(p, nullptr)) pOld->Release();
   }
   HRESULT CopyTo(T** ppValue) const noexcept {
      if(!ppValue) return E_POINTER;
      *ppValue = p;
      if(p) p->AddRef();
      return S_OK;
   }
   template<class U> HRESULT QueryInterface(U** ppValue) const noexcept {
      if(!p || !ppValue) return E_POINTER;
      return p->QueryInterface(__uuidof(U), reinterpret_cast<void**>(ppValue));
   }

private:
   void Assign(T* pValue) noexcept {
      if(pValue) pValue->AddRef();
      T* pOld = std::exchange(p, pValue);
      if(pOld) pOld->Release();
   }
};

// DXC's portable COM sources use a one-argument QueryInterface helper.
// Translate it to the native Windows COM ABI without modifying IUnknown.
template<class T, class U>
HRESULT DxcBcc64xQueryInterface(T* pSource, U** ppTarget) noexcept {
   if(!ppTarget) return E_POINTER;
   *ppTarget = nullptr;
   if(!pSource) return E_POINTER;
   return pSource->QueryInterface(__uuidof(U), reinterpret_cast<void**>(ppTarget));
}

template<class T, class U>
HRESULT DxcBcc64xQueryInterface(AdeccDxcCComPtr<T> const& theSource, U** ppTarget) noexcept {
   return DxcBcc64xQueryInterface(theSource.p, ppTarget);
}

// The BCC64X SDK does not expose SizeTToInt in the active header configuration.
inline HRESULT DxcBcc64xSizeTToInt(std::size_t nValue, int* pResult) noexcept {
   if(!pResult) return E_POINTER;
   if(nValue > static_cast<std::size_t>(INT_MAX)) return INTSAFE_E_ARITHMETIC_OVERFLOW;
   *pResult = static_cast<int>(nValue);
   return S_OK;
}

template<class T, class Allocator = AdeccDxcCAllocator> class AdeccDxcCHeapPtr {
public:
   T* m_pData {};

   AdeccDxcCHeapPtr() noexcept = default;
   explicit AdeccDxcCHeapPtr(T* pValue) noexcept : m_pData {pValue} {}
   AdeccDxcCHeapPtr(AdeccDxcCHeapPtr const&) = delete;
   AdeccDxcCHeapPtr& operator=(AdeccDxcCHeapPtr const&) = delete;
   AdeccDxcCHeapPtr(AdeccDxcCHeapPtr&& theOther) noexcept
      : m_pData {std::exchange(theOther.m_pData, nullptr)} {}
   AdeccDxcCHeapPtr& operator=(AdeccDxcCHeapPtr&& theOther) noexcept {
      if(this != std::addressof(theOther)) Attach(theOther.Detach());
      return *this;
   }
   ~AdeccDxcCHeapPtr() noexcept { Free(); }

   [[nodiscard]] operator T*() const noexcept { return m_pData; }
   [[nodiscard]] T* operator->() const noexcept { return m_pData; }
   [[nodiscard]] T** operator&() noexcept {
      ATLASSERT(m_pData == nullptr);
      return &m_pData;
   }
   [[nodiscard]] bool Allocate(std::size_t nElements = 1) noexcept {
      return AllocateBytes(nElements * sizeof(T));
   }
   [[nodiscard]] bool AllocateBytes(std::size_t nBytes) noexcept {
      ATLASSERT(m_pData == nullptr);
      m_pData = static_cast<T*>(Allocator::Allocate(nBytes));
      return m_pData != nullptr;
   }
   [[nodiscard]] bool Reallocate(std::size_t nElements) noexcept {
      return ReallocateBytes(nElements * sizeof(T));
   }
   [[nodiscard]] bool ReallocateBytes(std::size_t nBytes) noexcept {
      T* pNew = static_cast<T*>(Allocator::Reallocate(m_pData, nBytes));
      if(!pNew) return false;
      m_pData = pNew;
      return true;
   }
   void Attach(T* pValue) noexcept {
      if(m_pData != pValue) { Free(); m_pData = pValue; }
   }
   [[nodiscard]] T* Detach() noexcept { return std::exchange(m_pData, nullptr); }
   void Free() noexcept {
      if(T* pOld = std::exchange(m_pData, nullptr)) Allocator::Free(pOld);
   }
};

// Minimal ATL AdeccDxcCSimpleArray surface required by DXC IntelliSense.
// Preserve Add's boolean allocation-failure contract and indexed access.
template<class T> class AdeccDxcCSimpleArray {
   std::vector<T> vecElements;
public:
   [[nodiscard]] bool Add(T const& theValue) {
      try { vecElements.push_back(theValue); return true; }
      catch(std::bad_alloc const&) { return false; }
   }
   [[nodiscard]] int GetSize() const noexcept {
      return static_cast<int>(vecElements.size());
   }
   [[nodiscard]] T& operator[](int iIndex) noexcept {
      return vecElements[static_cast<std::size_t>(iIndex)];
   }
   [[nodiscard]] T const& operator[](int iIndex) const noexcept {
      return vecElements[static_cast<std::size_t>(iIndex)];
   }
};

template<class T> using AdeccDxcCComHeapPtr = AdeccDxcCHeapPtr<T>;

class AdeccDxcCHandle {
   HANDLE hHandle {};
public:
   explicit AdeccDxcCHandle(HANDLE hNew) noexcept : hHandle {hNew} {}
   AdeccDxcCHandle(AdeccDxcCHandle const&) = delete;
   AdeccDxcCHandle& operator=(AdeccDxcCHandle const&) = delete;
   ~AdeccDxcCHandle() noexcept {
      if(hHandle && hHandle != INVALID_HANDLE_VALUE) ::CloseHandle(hHandle);
   }
   [[nodiscard]] operator HANDLE() const noexcept { return hHandle; }
};

// UTF-8 to native UTF-16 conversion for DXC's Windows API call sites.
class AdeccDxcCA2W {
   std::wstring strWide;
public:
   explicit AdeccDxcCA2W(LPCSTR pUtf8) {
      if(!pUtf8) return;
      int const iLength = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
         pUtf8, -1, nullptr, 0);
      if(iLength <= 0) return;
      strWide.resize(static_cast<std::size_t>(iLength));
      if(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, pUtf8, -1,
         strWide.data(), iLength) == 0)
         strWide.clear();
   }
   [[nodiscard]] operator LPCWSTR() const noexcept { return strWide.c_str(); }
};

// Owns a BSTR and exposes BSTR* for COM [out] parameters.
class AdeccDxcCComBSTR {
   BSTR pValue {};
public:
   AdeccDxcCComBSTR() noexcept = default;
   AdeccDxcCComBSTR(unsigned int uLength, LPCWSTR pText)
      : pValue { ::SysAllocStringLen(pText, uLength) } {}
   AdeccDxcCComBSTR(AdeccDxcCComBSTR const&) = delete;
   AdeccDxcCComBSTR& operator=(AdeccDxcCComBSTR const&) = delete;
   AdeccDxcCComBSTR(AdeccDxcCComBSTR&& theOther) noexcept
      : pValue { std::exchange(theOther.pValue, nullptr) } {}
   AdeccDxcCComBSTR& operator=(AdeccDxcCComBSTR&& theOther) noexcept {
      if(this != std::addressof(theOther)) {
         ::SysFreeString(pValue);
         pValue = std::exchange(theOther.pValue, nullptr);
      }
      return *this;
   }
   ~AdeccDxcCComBSTR() noexcept { ::SysFreeString(pValue); }
   [[nodiscard]] BSTR Detach() noexcept { return std::exchange(pValue, nullptr); }
   [[nodiscard]] BSTR Get() const noexcept { return pValue; }
   [[nodiscard]] operator BSTR() const noexcept { return pValue; }
   [[nodiscard]] BSTR* operator&() noexcept {
      ATLASSERT(pValue == nullptr);
      return &pValue;
   }
};

class AdeccDxcCW2A {
   std::string strUtf8;
public:
   explicit AdeccDxcCW2A(LPCWSTR pWide) {
      if(!pWide) return;
      int const iLength = ::WideCharToMultiByte(
         CP_UTF8, 0, pWide, -1, nullptr, 0, nullptr, nullptr);
      if(iLength <= 0) return;
      strUtf8.resize(static_cast<std::size_t>(iLength));
      if(::WideCharToMultiByte(CP_UTF8, 0, pWide, -1,
         strUtf8.data(), iLength, nullptr, nullptr) == 0)
         strUtf8.clear();
   }
   [[nodiscard]] operator LPSTR() noexcept { return strUtf8.data(); }
   [[nodiscard]] operator LPCSTR() const noexcept { return strUtf8.c_str(); }
};

// Compatibility aliases required by the pinned upstream DXC sources.
// These are NOT Microsoft ATL implementations. The concrete types above
// are owned by BuildEngine and intentionally carry the AdeccDxc prefix.
using CAllocator = AdeccDxcCAllocator;
template<class T> using CComPtr = AdeccDxcCComPtr<T>;
template<class T, class Allocator = CAllocator>
using CHeapPtr = AdeccDxcCHeapPtr<T, Allocator>;
template<class T> using CSimpleArray = AdeccDxcCSimpleArray<T>;
template<class T> using CComHeapPtr = AdeccDxcCComHeapPtr<T>;
using CHandle = AdeccDxcCHandle;
using CA2W = AdeccDxcCA2W;
using CComBSTR = AdeccDxcCComBSTR;
using CW2A = AdeccDxcCW2A;
