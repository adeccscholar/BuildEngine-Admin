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
#include <limits>
#include <new>
#include <type_traits>
#include <string>
#include <vector>
#include <utility>
#include <system_error>
#include <stdexcept>
#include <cwchar>
#include <cstdint>

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
   [[nodiscard]] T& operator*() const noexcept { ATLASSERT(p != nullptr); return *p; }
   [[nodiscard]] bool operator<(T const* pOther) const noexcept { return p < pOther; }
   void Swap(AdeccDxcCComPtr& theOther) noexcept { std::swap(p, theOther.p); }
   [[nodiscard]] bool IsEqualObject(IUnknown* pOther) const noexcept {
      if(!p && !pOther) return true;
      if(!p || !pOther) return false;
      AdeccDxcCComPtr<IUnknown> theLeft;
      AdeccDxcCComPtr<IUnknown> theRight;
      if(FAILED(p->QueryInterface(__uuidof(IUnknown),
         reinterpret_cast<void**>(theLeft.GetAddressOf()))))
         return false;
      if(FAILED(pOther->QueryInterface(__uuidof(IUnknown),
         reinterpret_cast<void**>(theRight.GetAddressOf()))))
         return false;
      return theLeft.Get() == theRight.Get();
   }

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
      try {
         if(vecElements.size() >= static_cast<std::size_t>(INT_MAX)) return false;
         vecElements.push_back(theValue);
         return true;
      }
      catch(std::bad_alloc const&) { return false; }
   }
   [[nodiscard]] T* GetData() noexcept { return vecElements.data(); }
   [[nodiscard]] T const* GetData() const noexcept { return vecElements.data(); }
   void RemoveAll() noexcept { vecElements.clear(); }
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

// Native replacements for the CAtlArray contract used by the original tests.
// Keep allocation-failure reporting, indexed storage and contiguous layout.
template<class T> class AdeccDxcArray {
   std::vector<T> vecElements;
public:
   [[nodiscard]] std::size_t GetCount() const noexcept { return vecElements.size(); }
   [[nodiscard]] bool SetCount(std::size_t uCount) noexcept {
      try { vecElements.resize(uCount); return true; }
      catch(std::bad_alloc const&) { return false; }
      catch(std::length_error const&) { return false; }
   }
   [[nodiscard]] T* GetData() noexcept { return vecElements.data(); }
   [[nodiscard]] T const* GetData() const noexcept { return vecElements.data(); }
};

template<class T> using CAtlArray = AdeccDxcArray<T>;

[[noreturn]] inline void AtlThrow(HRESULT hr) {
   throw std::system_error(static_cast<int>(hr), std::system_category(),
      "DXC native Windows operation failed");
}

// Portable Base64 decoding for original DXC GPU shader tests, without ATL.
[[nodiscard]] inline int Base64DecodeGetRequiredLength(int iLength) noexcept {
   return iLength >= 0 && iLength <= (INT_MAX / 3) - 4
      ? ((iLength + 3) / 4) * 3 : -1;
}

[[nodiscard]] inline bool Base64Decode(
   char const* szInput, int iLength, BYTE* pOutput, int* pOutputLength) noexcept {
   if(!szInput || iLength < 0 || !pOutputLength || *pOutputLength < 0)
      return false;
   int iWritten = 0;
   unsigned uAccumulator = 0;
   int iBits = 0;
   bool bPadding = false;
   for(int i = 0; i < iLength; ++i) {
      unsigned char const uChar = static_cast<unsigned char>(szInput[i]);
      if(uChar == '=' ) { bPadding = true; continue; }
      if(uChar == ' ' || uChar == '\n' || uChar == '\r' || uChar == '\t')
         continue;
      if(bPadding) return false;
      int iValue = -1;
      if(uChar >= 'A' && uChar <= 'Z') iValue = uChar - 'A';
      else if(uChar >= 'a' && uChar <= 'z') iValue = uChar - 'a' + 26;
      else if(uChar >= '0' && uChar <= '9') iValue = uChar - '0' + 52;
      else if(uChar == '+') iValue = 62;
      else if(uChar == '/') iValue = 63;
      if(iValue < 0) return false;
      uAccumulator = (uAccumulator << 6) | static_cast<unsigned>(iValue);
      iBits += 6;
      if(iBits >= 8) {
         iBits -= 8;
         if(iWritten >= *pOutputLength || !pOutput) return false;
         pOutput[iWritten++] = static_cast<BYTE>((uAccumulator >> iBits) & 0xff);
      }
   }
   if(iBits != 0 && (uAccumulator & ((1u << iBits) - 1u)) != 0)
      return false;
   *pOutputLength = iWritten;
   return true;
}

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
// The published m_psz is a non-owning alias of the private conversion
// buffer. Copy/move are disabled to prevent dangling internal pointers.
class AdeccDxcCA2W {
   std::wstring strWide;
public:
   LPWSTR m_psz {};
   AdeccDxcCA2W(AdeccDxcCA2W const&) = delete;
   AdeccDxcCA2W& operator=(AdeccDxcCA2W const&) = delete;
   AdeccDxcCA2W(AdeccDxcCA2W&&) = delete;
   AdeccDxcCA2W& operator=(AdeccDxcCA2W&&) = delete;
   explicit AdeccDxcCA2W(LPCSTR pUtf8) {
      if(!pUtf8) return;
      int const iLength = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS,
         pUtf8, -1, nullptr, 0);
      if(iLength <= 0) return;
      strWide.resize(static_cast<std::size_t>(iLength));
      if(::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, pUtf8, -1,
         strWide.data(), iLength) != 0)
         m_psz = strWide.data();
      else
         strWide.clear();
   }
   AdeccDxcCA2W(LPCSTR pUtf8, unsigned int uCodePage) : AdeccDxcCA2W(pUtf8) {
      if(uCodePage != CP_UTF8) throw std::invalid_argument("DXC native converter requires UTF-8");
   }
   [[nodiscard]] operator LPWSTR() noexcept { return m_psz; }
   [[nodiscard]] operator LPCWSTR() const noexcept { return m_psz; }
};

// Owns a BSTR and exposes BSTR* for COM [out] parameters.
// Explicit BuildEngine-owned BSTR type with the DXC/ATL source contract.
// Ownership transfers only through Attach/Detach; Copy always allocates.
class AdeccDxcCComBSTR {
public:
   BSTR m_str {};

   AdeccDxcCComBSTR() noexcept = default;
   explicit AdeccDxcCComBSTR(LPCWSTR pText)
      : m_str { ::SysAllocString(pText) } {}
   AdeccDxcCComBSTR(unsigned int uLength, LPCWSTR pText)
      : m_str { ::SysAllocStringLen(pText, uLength) } {}
   AdeccDxcCComBSTR(AdeccDxcCComBSTR const&) = delete;
   AdeccDxcCComBSTR& operator=(AdeccDxcCComBSTR const&) = delete;
   AdeccDxcCComBSTR(AdeccDxcCComBSTR&& theOther) noexcept
      : m_str { std::exchange(theOther.m_str, nullptr) } {}
   AdeccDxcCComBSTR& operator=(AdeccDxcCComBSTR&& theOther) noexcept {
      if(this != std::addressof(theOther))
         Attach(theOther.Detach());
      return *this;
   }
   ~AdeccDxcCComBSTR() noexcept { Empty(); }

   AdeccDxcCComBSTR& operator=(LPCWSTR pText) {
      // SysAllocString must run before Empty(): the source may alias m_str.
      BSTR pNew = pText ? ::SysAllocString(pText) : nullptr;
      if(pText && !pNew) throw std::bad_alloc {};
      Attach(pNew);
      return *this;
   }
   [[nodiscard]] HRESULT CopyTo(BSTR* ppValue) const noexcept {
      return Copy(ppValue);
   }
   [[nodiscard]] unsigned int Length() const noexcept { return ::SysStringLen(m_str); }
   [[nodiscard]] operator BSTR() const noexcept { return m_str; }
   [[nodiscard]] BSTR* operator&() noexcept {
      ATLASSERT(m_str == nullptr);
      return &m_str;
   }
   [[nodiscard]] wchar_t& operator[](std::size_t uIndex) noexcept {
      ATLASSERT(uIndex < Length());
      return m_str[uIndex];
   }
   [[nodiscard]] wchar_t const& operator[](std::size_t uIndex) const noexcept {
      ATLASSERT(uIndex < Length());
      return m_str[uIndex];
   }
   [[nodiscard]] bool operator==(AdeccDxcCComBSTR const& theOther) const noexcept {
      return Length() == theOther.Length() &&
         (Length() == 0 || std::char_traits<wchar_t>::compare(m_str, theOther.m_str, Length()) == 0);
   }
   [[nodiscard]] bool operator==(LPCWSTR pWide) const noexcept {
      return pWide && m_str && std::wcscmp(m_str, pWide) == 0;
   }
   [[nodiscard]] bool operator==(LPCSTR pUtf8) const {
      if(!pUtf8) return m_str == nullptr;
      AdeccDxcCA2W theWide(pUtf8);
      LPCWSTR const pText = static_cast<LPCWSTR>(theWide);
      return pText && m_str && std::wcscmp(m_str, pText) == 0;
   }
   void Empty() noexcept {
      ::SysFreeString(std::exchange(m_str, nullptr));
   }
   void Attach(BSTR pNew) noexcept {
      if(m_str != pNew) {
         Empty();
         m_str = pNew;
      }
   }
   [[nodiscard]] BSTR Detach() noexcept { return std::exchange(m_str, nullptr); }
   [[nodiscard]] BSTR Get() const noexcept { return m_str; }
   [[nodiscard]] BSTR Copy() const noexcept {
      return m_str ? ::SysAllocStringLen(m_str, Length()) : nullptr;
   }
   [[nodiscard]] HRESULT Copy(BSTR* ppValue) const noexcept {
      if(!ppValue) return E_POINTER;
      *ppValue = Copy();
      return m_str && !*ppValue ? E_OUTOFMEMORY : S_OK;
   }
   [[nodiscard]] HRESULT Append(LPCWSTR pText, unsigned int uLength) {
      if(uLength == 0) return S_OK;
      if(!pText) return E_POINTER;
      unsigned int const uOldLength = Length();
      if(uLength > (std::numeric_limits<unsigned int>::max)() - uOldLength)
         return E_OUTOFMEMORY;
      BSTR pNew = ::SysAllocStringLen(nullptr, uOldLength + uLength);
      if(!pNew) return E_OUTOFMEMORY;
      if(uOldLength != 0) std::char_traits<wchar_t>::copy(pNew, m_str, uOldLength);
      std::char_traits<wchar_t>::copy(pNew + uOldLength, pText, uLength);
      ::SysFreeString(m_str);
      m_str = pNew;
      return S_OK;
   }
   [[nodiscard]] HRESULT Append(LPCWSTR pText) {
      return pText ? Append(pText, static_cast<unsigned int>(std::char_traits<wchar_t>::length(pText))) : S_OK;
   }
   [[nodiscard]] HRESULT Append(LPCSTR pUtf8) {
      if(!pUtf8) return S_OK;
      AdeccDxcCA2W theText(pUtf8);
      LPCWSTR const pWide = static_cast<LPCWSTR>(theText);
      if(*pUtf8 != '\0' && (!pWide || *pWide == L'\0')) return E_INVALIDARG;
      return Append(pWide);
   }
};

// DXC still accesses m_psz in unmodified upstream translation units.
// Keep the field compatible while using Win32 UTF-8 conversion internally.
class AdeccDxcCW2A {
   std::string strUtf8;
public:
   LPSTR m_psz {};
   AdeccDxcCW2A(AdeccDxcCW2A const&) = delete;
   AdeccDxcCW2A& operator=(AdeccDxcCW2A const&) = delete;
   AdeccDxcCW2A(AdeccDxcCW2A&&) = delete;
   AdeccDxcCW2A& operator=(AdeccDxcCW2A&&) = delete;
   explicit AdeccDxcCW2A(LPCWSTR pWide) {
      if(!pWide) return;
      int const iLength = ::WideCharToMultiByte(
         CP_UTF8, 0, pWide, -1, nullptr, 0, nullptr, nullptr);
      if(iLength <= 0) return;
      strUtf8.resize(static_cast<std::size_t>(iLength));
      if(::WideCharToMultiByte(CP_UTF8, 0, pWide, -1,
         strUtf8.data(), iLength, nullptr, nullptr) != 0)
         m_psz = strUtf8.data();
      else
         strUtf8.clear();
   }
   [[nodiscard]] operator LPSTR() noexcept { return m_psz; }
   [[nodiscard]] operator LPCSTR() const noexcept { return m_psz; }
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
// Original DXC DIA Symbol::SetValue requires the ATL CComVariant value
// contract, backed by the native Windows VARIANT and Automation allocator.
class AdeccDxcCComVariant : public VARIANT {
public:
   AdeccDxcCComVariant() noexcept { ::VariantInit(this); }
   AdeccDxcCComVariant(AdeccDxcCComVariant const& theOther) : AdeccDxcCComVariant() {
      HRESULT const hr = ::VariantCopy(this, const_cast<VARIANT*>(static_cast<VARIANT const*>(&theOther)));
      if(FAILED(hr)) throw std::bad_alloc {};
   }
   AdeccDxcCComVariant& operator=(AdeccDxcCComVariant const& theOther) {
      if(this != std::addressof(theOther)) {
         HRESULT const hr = Copy(const_cast<VARIANT*>(static_cast<VARIANT const*>(&theOther)));
         if(FAILED(hr)) throw std::bad_alloc {};
      }
      return *this;
   }
   ~AdeccDxcCComVariant() noexcept { ::VariantClear(this); }

   [[nodiscard]] HRESULT ChangeType(VARTYPE theType) noexcept {
      VARIANT theConverted {};
      ::VariantInit(&theConverted);
      HRESULT const hr = ::VariantChangeType(&theConverted, this, 0, theType);
      if(FAILED(hr)) return hr;
      ::VariantClear(this);
      *static_cast<VARIANT*>(this) = theConverted;
      return S_OK;
   }
   [[nodiscard]] HRESULT Copy(VARIANT const* pSource) noexcept {
      if(!pSource) return E_POINTER;
      if(static_cast<VARIANT const*>(this) == pSource) return S_OK;
      return ::VariantCopy(this, const_cast<VARIANT*>(pSource));
   }
   AdeccDxcCComVariant& operator=(unsigned int uValue) {
      ::VariantClear(this);
      vt = VT_UI4;
      ulVal = uValue;
      return *this;
   }
   AdeccDxcCComVariant& operator=(LPCSTR pUtf8) {
      AdeccDxcCA2W theWide(pUtf8);
      LPCWSTR pWide = static_cast<LPCWSTR>(theWide);
      if(pUtf8 && !pWide) throw std::bad_alloc {};
      BSTR pNew = pWide ? ::SysAllocString(pWide) : nullptr;
      if(pWide && !pNew) throw std::bad_alloc {};
      ::VariantClear(this);
      vt = VT_BSTR;
      bstrVal = pNew;
      return *this;
   }
};

using CComBSTR = AdeccDxcCComBSTR;
using CComVariant = AdeccDxcCComVariant;
using CW2A = AdeccDxcCW2A;

// Compile-time contracts for the historical DXC ATL-facing API surface.
// These checks are compiled in every native DXC translation unit that
// includes the compatibility header, not merely in a separate smoke test.
static_assert(std::is_same_v<decltype(std::declval<CComBSTR&>().Detach()), BSTR>);
static_assert(std::is_same_v<decltype(std::declval<CComBSTR&>().operator&()), BSTR*>);
static_assert(std::is_nothrow_move_constructible_v<CComBSTR>);
static_assert(std::is_nothrow_move_assignable_v<CComBSTR>);
static_assert(!std::is_copy_constructible_v<CComBSTR>);
static_assert(std::is_same_v<decltype(std::declval<CComPtr<IUnknown>&>().Detach()), IUnknown*>);
static_assert(std::is_same_v<decltype(std::declval<CComPtr<IUnknown>&>().GetAddressOf()), IUnknown**>);
static_assert(std::is_same_v<decltype(std::declval<CComPtr<IUnknown> const&>().Get()), IUnknown*>);
static_assert(std::is_same_v<decltype(std::declval<CHeapPtr<char>&>().Detach()), char*>);
static_assert(std::is_same_v<decltype(std::declval<CSimpleArray<int>&>().GetSize()), int>);
static_assert(std::is_same_v<decltype(std::declval<CA2W const&>().operator LPCWSTR()), LPCWSTR>);
static_assert(std::is_same_v<decltype(std::declval<CW2A const&>().operator LPCSTR()), LPCSTR>);
static_assert(std::is_same_v<decltype(std::declval<CComBSTR const&>().Copy()), BSTR>);
static_assert(std::is_same_v<decltype(std::declval<CComBSTR const&>().Length()), unsigned int>);
static_assert(std::is_same_v<decltype(std::declval<CComBSTR&>().Append(L"test")), HRESULT>);
static_assert(std::is_same_v<decltype(std::declval<CComBSTR&>().Append("test")), HRESULT>);
static_assert(std::is_same_v<decltype(std::declval<CComPtr<IUnknown>&>().operator*()), IUnknown&>);
static_assert(std::is_same_v<decltype(std::declval<CSimpleArray<int>&>().GetData()), int*>);
static_assert(std::is_same_v<decltype(std::declval<CA2W&>().m_psz), LPWSTR>);
static_assert(std::is_same_v<decltype(std::declval<CW2A&>().m_psz), LPSTR>);
static_assert(!std::is_copy_constructible_v<CA2W> && !std::is_move_constructible_v<CA2W>);
static_assert(!std::is_copy_constructible_v<CW2A> && !std::is_move_constructible_v<CW2A>);

static_assert(std::is_base_of_v<VARIANT, CComVariant>);
static_assert(std::is_same_v<decltype(std::declval<CComBSTR&>().CopyTo(static_cast<BSTR*>(nullptr))), HRESULT>);
static_assert(std::is_same_v<decltype(std::declval<CComBSTR&>() = static_cast<LPCWSTR>(nullptr)), CComBSTR&>);
