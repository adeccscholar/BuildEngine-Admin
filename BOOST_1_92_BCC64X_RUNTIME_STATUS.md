# Boost 1.92.0 / BCC64X – Runtime Status and Handoff

Status date: 2026-08-31

This document is the technical re-entry point for the Boost 1.92.0 runtime findings with C++Builder 13 / BCC64X. Diagnosis is complete for the current third-party setup: the remaining text-archive failure is documented as a known BCC64X/libc++ DLL boundary and no longer blocks the wider library graph.

## 1. Goal and toolchain

Verified compiler/target contract:

- Compiler: BCC64X / Clang 20.1.7
- Language: C++23
- Target: `x86_64-w64-windows-gnu`
- C++ standard library: LLVM libc++
- Windows GNU foundation: MinGW-w64 / UCRT
- Thread model: posix
- Boost.Config uses the native Clang branch.
- `BOOST_EMBTC` is not active in the managed consumer path.
- `BOOST_NO_CXX11_NOEXCEPT` is not active.

MSVC, clang-cl, or any other replacement toolchain is not part of this proof.

## 2. Historical reference

The historical R193 proof for Boost 1.92.0 was green. Serialization was linked together with Boost.Iostreams, Boost.Locale, Boost.Nowide, and Boost.URL and executed a normal `text_oarchive`/`text_iarchive` round trip.

Reproducing this larger link composition today did not change the current failure mode. The additional link composition is therefore excluded as the sole cause.

## 3. Verified positive findings

The following paths are verified with the current BCC64X / Boost 1.92.0 package:

- Boost.Charconv runtime: PASS
- Boost.URL runtime: PASS
- standard locale/codecvt for `char` and `wchar_t`: PASS
- `std::stringstream::imbue` and `std::wstringstream::imbue`: PASS
- `boost::archive::codecvt_null<char>` and `<wchar_t>`: PASS
- local `boost::archive::basic_ostream_locale_saver`: PASS
- Boost.Iostreams plain output + flush: PASS
- local reconstruction of the text-primitive lifetime on a standard stream: PASS
- local reconstruction of the same lifetime on Boost.Iostreams: PASS
- Boost.Serialization binary archive on a standard stream: PASS
- Boost.Serialization binary archive on Boost.Iostreams: PASS
- `std::uncaught_exceptions()` in the EXE: PASS
- `boost::core::uncaught_exceptions()` in the EXE: PASS
- both functions from a separate BCC64X DLL: PASS

Boost.Iostreams and Boost.Serialization as a whole are therefore explicitly **not** classified as defective. The remaining finding concerns a narrower C++ stream ABI boundary.

## 4. Decisive boundary finding

A minimal custom BCC64X DLL reproducer receives a `std::ostream&` created in the EXE.

Reproduced sequence in Release and Debug:

```text
local-ostream-endl      PASS
dll-ostream-put         PASS
dll-ostream-flush       PASS
dll-ostream-insert-char BEGIN
<0xC0000005>
```

The remaining failure is therefore no longer only an indication from Boost.Serialization.

> In the examined BCC64X/LLVM-libc++ configuration, a BCC64X DLL can successfully use the elementary member functions `put()` and `flush()` on a `std::ostream&` created in the EXE, while the free/overloaded C++ stream insertion path `operator<<` reproducibly fails with `0xC0000005`.

`std::endl` contains the same insertion path and is therefore affected as well.

This finding very plausibly explains the previously isolated Boost.Serialization text-archive crash: `basic_text_oprimitive<std::ostream>` is explicitly instantiated in `boost_serialization.dll` and executes `os << std::endl` in its destructor on a stream supplied by the consumer.

The causal relationship between exactly this call and every possible text-archive case is technically very strongly supported, but is not generalized beyond the reproduced boundary finding.

## 5. Boost.Iostreams is closed

Boost.Iostreams remains removed from the description and TODO list as an open cause.

Verified:

1. normal writing and flush with `boost::iostreams::stream`;
2. local stream/locale lifetime;
3. complete binary-archive round trip.

The failure appears only when code from a DLL uses the problematic `std::ostream` insertion path on a consumer-owned stream.

## 6. Acceptance decision

The known crash path remains in the repository as a reproducible diagnostic source, but is **no longer executed as a normal acceptance gate**.

The active `boost-evidence-runtime-serialization.exe` still checks:

- local `std::endl` path;
- DLL `put()`;
- DLL `flush()`;
- local text-primitive lifetime;
- Boost.Iostreams plain output;
- local text-primitive lifetime on Boost.Iostreams;
- binary serialization through Boost.Iostreams.

It then reports two stable `KNOWN-LIMITATION` lines and exits successfully.

The following functions remain only as reproducers in the source and are not called during the acceptance run:

- DLL `operator<<('\n')`;
- DLL `std::endl`;
- real Boost.Serialization text-archive construct/destroy probes.

A green Boost gate therefore means:

> The defined BCC64X usage profile is verified; the known libc++ C++ stream DLL boundary and Boost.Serialization text archives affected by it are documented and are not part of the accepted profile.

It explicitly does not mean that the known text-archive reproducer has been fixed.

## 7. Reproducer files

Active or retained diagnostic sources:

- `admin/smokes/boost/component-gate/runtime-serialization.cpp`
- `admin/smokes/boost/component-gate/runtime-stream-boundary.cpp`
- `admin/smokes/boost/component-gate/runtime-stream-boundary.h`
- `admin/smokes/boost/component-gate/runtime-uncaught.cpp`
- `admin/smokes/boost/component-gate/runtime-uncaught-boundary.cpp`
- `admin/smokes/boost/component-gate/runtime-uncaught-boundary.h`

## 8. Do not revisit without new evidence

- Boost.Iostreams as an independent defect
- larger R193 link composition
- Boost.Locale WinAPI backend
- general locale/codecvt defect
- `boost::core::uncaught_exceptions()`
- binary archives in general
- payload/`std::string` round trip as the primary cause

## 9. Relevant Admin commits

- `c218b8ba3c838b5cd6a4d757b5197cf70a6a3d53` – reproduced R193 Serialization link graph
- `f00eafc55b8368529cb67ab0ed64fd9c43a9d4be` – disabled Boost.Locale BCC64X WinAPI backend
- `5fc3562e7d5658b27d4b3708e79f26ff99fd51c9` – extended locale/codecvt/archive diagnostics
- `2f9b810ff0dc69e874516151e96cc71d44d6df3d` – constructor/destructor checkpoints
- `fe4a05017829dbb2c358c066f85dc634da2a87b4` – Serialization boundary test
- `2260fdc79885969a8e40bf92cc311119a6781d0d` – `uncaught_exceptions()` DLL reproducer
- `8fc158cb48d25a1108f5ec50af62f1ce014acb9f` – integrated `std::ostream&` DLL boundary
- `98328f3a60848910e8e0951954b2061ba55d837a` – removed known crash path from acceptance while retaining evidence

## 10. BuildEngine currentness

During diagnosis it was additionally discovered that smoke currentness previously considered only the source path, not source content. Changed smoke sources could therefore incorrectly remain `current`.

BuildEngine commit:

- `9ae53c65f530bf57c889123380a263723523175b` – smoke source tree is fingerprinted through relative file paths + SHA-256; changes invalidate the affected smoke.

## 11. Next third-party work block

Boost is closed for current project progress.

Next sequence:

1. Integrate and verify Xerces-C as an independent BCC64X package.
2. Then integrate ACE 8.0.6 / TAO 4.0.6 based on the already successfully verified evidence path.
3. Enable Xerces-C as a dependency for the ACE/TAO components that were skipped in the earlier MPC run because they `require xerces`, especially `ACE_XML_Utils`.
4. Reuse the existing ACE/TAO evidence, patches, MPC/BMake contract, and service gates; do not reinvent the successful port.
