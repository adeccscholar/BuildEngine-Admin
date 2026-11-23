#pragma once

// Native-Windows equivalents of the ATL helper types used by DXC.
// This file is included only for the Windows BCC64X/Clang toolchain.
// It deliberately uses the Windows COM ABI and does not emulate Windows APIs.
#include <windows.h>
#include <objbase.h>
#include <cassert>
#include <cstddef>
#include <string>
#include <utility>

#ifndef ATLASSERT
#define ATLASSERT(expression) assert(expression)
#endif

class CAllocator {
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

template<class T> class CComPtr {
public:
   T* p {};

   CComPtr() noexcept = default;
   explicit CComPtr(T* pValue) noexcept : p {pValue} {
      if(p) p->AddRef();
   }
   CComPtr(CComPtr const& theOther) noexcept : CComPtr(theOther.p) {}
   template<class U> CComPtr(CComPtr<U> const& theOther) noexcept
      : CComPtr(static_cast<T*>(theOther.p)) {}
   CComPtr(CComPtr&& theOther) noexcept : p {std::exchange(theOther.p, nullptr)} {}
   ~CComPtr() noexcept { Release(); }

   CComPtr& operator=(CComPtr const& theOther) noexcept {
      if(this != &theOther) Assign(theOther.p);
      return *this;
   }
   CComPtr& operator=(CComPtr&& theOther) noexcept {
      if(this != &theOther) {
         Release();
         p = std::exchange(theOther.p, nullptr);
      }
      return *this;
   }
   T* operator=(T* pValue) noexcept { Assign(pValue); return p; }
   template<class U> CComPtr& operator=(CComPtr<U> const& theOther) noexcept {
      Assign(static_cast<T*>(theOther.p));
      return *this;
   }
   [[nodiscard]] operator T*() const noexcept { return p; }
   [[nodiscard]] T* operator->() const noexcept { return p; }
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

template<class T, class Allocator = CAllocator> class CHeapPtr {
public:
   T* m_pData {};

   CHeapPtr() noexcept = default;
   explicit CHeapPtr(T* pValue) noexcept : m_pData {pValue} {}
   CHeapPtr(CHeapPtr const&) = delete;
   CHeapPtr& operator=(CHeapPtr const&) = delete;
   CHeapPtr(CHeapPtr&& theOther) noexcept
      : m_pData {std::exchange(theOther.m_pData, nullptr)} {}
   CHeapPtr& operator=(CHeapPtr&& theOther) noexcept {
      if(this != &theOther) Attach(theOther.Detach());
      return *this;
   }
   ~CHeapPtr() noexcept { Free(); }

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

template<class T> using CComHeapPtr = CHeapPtr<T>;

class CHandle {
   HANDLE hHandle {};
public:
   explicit CHandle(HANDLE hNew) noexcept : hHandle {hNew} {}
   CHandle(CHandle const&) = delete;
   CHandle& operator=(CHandle const&) = delete;
   ~CHandle() noexcept {
      if(hHandle && hHandle != INVALID_HANDLE_VALUE) ::CloseHandle(hHandle);
   }
   [[nodiscard]] operator HANDLE() const noexcept { return hHandle; }
};

class CW2A {
   std::string strUtf8;
public:
   explicit CW2A(LPCWSTR pWide) {
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
