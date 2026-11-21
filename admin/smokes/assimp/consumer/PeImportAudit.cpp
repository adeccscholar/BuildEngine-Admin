// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Verify the real PE import table rather than guessing from a loader exit code.
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace fs = std::filesystem;

struct PeSection {
   std::uint32_t uVirtualSize;
   std::uint32_t uVirtualAddress;
   std::uint32_t uRawSize;
   std::uint32_t uRawAddress;
   };

/** \brief Read a little-endian 16-bit value from a binary PE buffer.
    \details Throws on truncated input instead of producing an invalid RVA.
    \param vecBytes Complete file bytes.
    \param uOffset Byte offset.
    \returns The decoded value.
    \throw std::runtime_error When the offset is outside the file. */
std::uint16_t Read16(std::vector<std::uint8_t> const& vecBytes, std::size_t const uOffset) {
   if(uOffset > vecBytes.size() || vecBytes.size() - uOffset < 2) {
      throw std::runtime_error { "PE is truncated while reading a word" };
      }
   return static_cast<std::uint16_t>(vecBytes[uOffset]) |
      static_cast<std::uint16_t>(vecBytes[uOffset + 1] << 8);
   }

/** \brief Read a little-endian 32-bit value from a binary PE buffer.
    \details Uses fixed offsets and checks every read.
    \param vecBytes Complete file bytes.
    \param uOffset Byte offset.
    \returns The decoded value.
    \throw std::runtime_error When the offset is outside the file. */
std::uint32_t Read32(std::vector<std::uint8_t> const& vecBytes, std::size_t const uOffset) {
   if(uOffset > vecBytes.size() || vecBytes.size() - uOffset < 4) {
      throw std::runtime_error { "PE is truncated while reading a dword" };
      }
   return static_cast<std::uint32_t>(vecBytes[uOffset]) |
      (static_cast<std::uint32_t>(vecBytes[uOffset + 1]) << 8) |
      (static_cast<std::uint32_t>(vecBytes[uOffset + 2]) << 16) |
      (static_cast<std::uint32_t>(vecBytes[uOffset + 3]) << 24);
   }

/** \brief Load one native PE file for inspection.
    \details Does not load or execute Windows code or search PATH.
    \param thePath Exact path to a staged executable or DLL.
    \returns Binary contents.
    \throw std::runtime_error On inaccessible, empty, or incomplete files. */
std::vector<std::uint8_t> ReadBytes(fs::path const& thePath) {
   std::ifstream theStream { thePath, std::ios::binary | std::ios::ate };
   if(!theStream) {
      throw std::runtime_error { "Cannot open PE file: " + thePath.string() };
      }
   std::streampos const theEnd = theStream.tellg();
   if(theEnd <= 0) {
      throw std::runtime_error { "Empty or inaccessible PE: " + thePath.string() };
      }
   std::vector<std::uint8_t> vecBytes(static_cast<std::size_t>(theEnd));
   theStream.seekg(0);
   theStream.read(reinterpret_cast<char*>(vecBytes.data()), static_cast<std::streamsize>(vecBytes.size()));
   if(!theStream) {
      throw std::runtime_error { "Incomplete PE read: " + thePath.string() };
      }
   return vecBytes;
   }

/** \brief Convert an image RVA into a raw file offset.
    \details Uses section mappings from the PE headers, not string scanning.
    \param vecBytes Complete PE file bytes.
    \param vecSections Section table.
    \param uRva Relative virtual address.
    \param uHeadersSize Size of PE image headers.
    \returns Offset, or an empty optional when the RVA is not file-backed.
    \throw std::runtime_error Never. */
std::optional<std::size_t> ToOffset(std::vector<std::uint8_t> const& vecBytes,
   std::vector<PeSection> const& vecSections, std::uint32_t const uRva,
   std::uint32_t const uHeadersSize) {
   if(uRva < uHeadersSize && uRva < vecBytes.size()) {
      return uRva;
      }
   for(PeSection const& theSection : vecSections) {
      if(uRva < theSection.uVirtualAddress) {
         continue;
         }
      std::uint32_t const uDelta = uRva - theSection.uVirtualAddress;
      if(uDelta >= theSection.uRawSize) {
         continue;
         }
      std::size_t const uOffset = static_cast<std::size_t>(theSection.uRawAddress) + uDelta;
      if(uOffset < vecBytes.size()) {
         return uOffset;
         }
      }
   return std::nullopt;
   }

/** \brief Read a zero-terminated DLL basename from a mapped PE RVA.
    \details An import name is limited to a conservative 260-byte bound.
    \param vecBytes Complete file bytes.
    \param uOffset Raw file offset of the import name.
    \returns DLL name as stored by the binary.
    \throw std::runtime_error On invalid or unterminated import names. */
std::string ReadName(std::vector<std::uint8_t> const& vecBytes, std::size_t const uOffset) {
   if(uOffset >= vecBytes.size()) {
      throw std::runtime_error { "Invalid import-name offset" };
      }
   std::string strName;
   for(std::size_t uIndex = uOffset; uIndex < vecBytes.size() &&
      uIndex - uOffset < 260; ++uIndex) {
      if(vecBytes[uIndex] == 0) {
         return strName;
         }
      strName.push_back(static_cast<char>(vecBytes[uIndex]));
      }
   throw std::runtime_error { "Invalid unterminated PE import name" };
   }

/** \brief Extract import DLL names from an AMD64 PE32+ binary.
    \details Reads the import data directory and maps each descriptor through the section table.
    \param thePath Exact binary path.
    \returns DLL basenames from the import directory.
    \throw std::runtime_error On malformed or non-AMD64 PE binaries. */
std::vector<std::string> ReadImports(fs::path const& thePath) {
   std::vector<std::uint8_t> const vecBytes = ReadBytes(thePath);
   if(Read16(vecBytes, 0) != 0x5a4d) {
      throw std::runtime_error { "Missing DOS MZ header: " + thePath.string() };
      }
   std::size_t const uNt = Read32(vecBytes, 0x3c);
   if(Read32(vecBytes, uNt) != 0x00004550 || Read16(vecBytes, uNt + 4) != 0x8664) {
      throw std::runtime_error { "Expected x64 PE image: " + thePath.string() };
      }
   std::uint16_t const uSections = Read16(vecBytes, uNt + 6);
   std::uint16_t const uOptionalSize = Read16(vecBytes, uNt + 20);
   std::size_t const uOptional = uNt + 24;
   if(uSections > 96 || uOptionalSize < 128 || Read16(vecBytes, uOptional) != 0x20b ||
      Read32(vecBytes, uOptional + 108) < 2) {
      throw std::runtime_error { "Unsupported PE32+ headers: " + thePath.string() };
      }
   std::uint32_t const uHeadersSize = Read32(vecBytes, uOptional + 60);
   std::uint32_t const uImportRva = Read32(vecBytes, uOptional + 120);
   std::vector<PeSection> vecSections;
   std::size_t const uTable = uOptional + uOptionalSize;
   for(std::size_t uIndex = 0; uIndex < uSections; ++uIndex) {
      std::size_t const uEntry = uTable + uIndex * 40;
      vecSections.push_back(PeSection {
         .uVirtualSize = Read32(vecBytes, uEntry + 8),
         .uVirtualAddress = Read32(vecBytes, uEntry + 12),
         .uRawSize = Read32(vecBytes, uEntry + 16),
         .uRawAddress = Read32(vecBytes, uEntry + 20)
         });
      }
   if(uImportRva == 0) {
      return {};
      }
   std::optional<std::size_t> const theImportOffset = ToOffset(vecBytes, vecSections, uImportRva, uHeadersSize);
   if(!theImportOffset) {
      throw std::runtime_error { "Import directory has no file-backed RVA: " + thePath.string() };
      }
   std::vector<std::string> vecImports;
   for(std::size_t uIndex = 0; uIndex < 4096; ++uIndex) {
      std::size_t const uDescriptor = *theImportOffset + uIndex * 20;
      std::uint32_t const uNameRva = Read32(vecBytes, uDescriptor + 12);
      std::uint32_t const uFirstThunk = Read32(vecBytes, uDescriptor + 16);
      std::uint32_t const uOriginalThunk = Read32(vecBytes, uDescriptor);
      if(uNameRva == 0 && uFirstThunk == 0 && uOriginalThunk == 0) {
         return vecImports;
         }
      std::optional<std::size_t> const theNameOffset = ToOffset(vecBytes, vecSections, uNameRva, uHeadersSize);
      if(!theNameOffset) {
         throw std::runtime_error { "Import name has no file-backed RVA: " + thePath.string() };
         }
      vecImports.push_back(ReadName(vecBytes, *theNameOffset));
      }
   throw std::runtime_error { "PE import descriptor table is not terminated" };
   }

/** \brief Normalize a case-insensitive Windows DLL basename.
    \details ASCII DLL basenames are case-insensitive on the target platform.
    \param svValue Input basename.
    \returns Lowercase name.
    \throw std::runtime_error Never. */
std::string Lower(std::string_view const svValue) {
   std::string strResult { svValue };
   std::ranges::transform(strResult, strResult.begin(), [](unsigned char const uChar) {
      return static_cast<char>(std::tolower(uChar));
      });
   return strResult;
   }

/** \brief Audit the exact staged Assimp and zlib DLL variants.
    \details A Release import of libzd.dll is a build/link error, not a staging request.
    \param iArgc Number of command-line arguments.
    \param pszArgv Arguments: directory, Release or Debug, and the installed Assimp DLL basename.
    \returns Zero when import closure is configuration-consistent.
    \throw std::runtime_error Caught and reported before process termination. */
int main(int const iArgc, char const* const* const pszArgv) {
   try {
      if(iArgc != 4) {
         throw std::runtime_error { "Usage: assimp-import-audit <application-directory> <Release|Debug> <assimp-dll>" };
         }
      fs::path const theDirectory { pszArgv[1] };
      std::string_view const svConfiguration { pszArgv[2] };
      std::string const strAssimpDll = Lower(pszArgv[3]);
      if(strAssimpDll.empty() || !strAssimpDll.ends_with(".dll") ||
         fs::path { strAssimpDll }.filename().string() != strAssimpDll) {
         throw std::runtime_error { "Invalid imported Assimp DLL basename" };
         }
      if(svConfiguration != "Release" && svConfiguration != "Debug") {
         throw std::runtime_error { "Configuration must be Release or Debug" };
         }
      bool const bRelease = svConfiguration == "Release";
      std::string const strExpected = bRelease ? "libz.dll" : "libzd.dll";
      std::string const strForbidden = bRelease ? "libzd.dll" : "libz.dll";
      if(!fs::exists(theDirectory / strExpected)) {
         throw std::runtime_error { "Required staged zlib DLL is absent: " +
            (theDirectory / strExpected).string() };
         }
      fs::path const theAssimpDll = theDirectory / strAssimpDll;
      if(!fs::exists(theAssimpDll)) {
         throw std::runtime_error { "The exact imported Assimp DLL is missing from smoke output: " + theAssimpDll.string() };
         }
      fs::path const theConsumer = theDirectory / "assimp-consumer.exe";
      if(!fs::exists(theConsumer)) {
         throw std::runtime_error { "Assimp smoke executable is missing: " + theConsumer.string() };
         }
      std::vector<std::string> const vecConsumerImports = ReadImports(theConsumer);
      bool const bImportsAssimp = std::ranges::any_of(vecConsumerImports,
         [&strAssimpDll](std::string const& strImport) {
            return Lower(strImport) == strAssimpDll;
            });
      if(!bImportsAssimp) {
         std::println("[PE-AUDIT] Consumer imports:");
         for(std::string const& strImport : vecConsumerImports) {
            std::println("[PE-AUDIT]   {}", strImport);
            }
         throw std::runtime_error { "Consumer is not linked against selected Assimp DLL: " + strAssimpDll };
         }
      if(bRelease && strAssimpDll == "libassimpd.dll") {
         throw std::runtime_error { "Release consumer cannot use libassimpd.dll" };
         }
      std::println("[PE-AUDIT] Consumer imports selected Assimp DLL: {}", strAssimpDll);
      std::vector<fs::path> vecFiles;
      for(fs::directory_entry const& theEntry : fs::directory_iterator { theDirectory }) {
         if(!theEntry.is_regular_file()) {
            continue;
            }
         std::string const strExtension = Lower(theEntry.path().extension().string());
         if(strExtension == ".exe" || strExtension == ".dll") {
            vecFiles.push_back(theEntry.path());
            }
         }
      std::ranges::sort(vecFiles);
      std::size_t uZlibImports = 0;
      for(fs::path const& theFile : vecFiles) {
         for(std::string const& strImport : ReadImports(theFile)) {
            std::string const strKey = Lower(strImport);
            if(strKey == strForbidden) {
               std::println("[PE-AUDIT] FAIL: {} imports forbidden {} in {}",
                  theFile.filename().string(), strImport, svConfiguration);
               return 1;
               }
            if(strKey == strExpected) {
               ++uZlibImports;
               std::println("[PE-AUDIT] {} imports managed {}", theFile.filename().string(), strImport);
               }
            }
         }
      if(uZlibImports == 0) {
         throw std::runtime_error { "No staged PE file imports managed " + strExpected +
            "; inspect the Assimp link line and installed DLL configuration" };
         }
      std::println("[PE-AUDIT] PASS: {} zlib imports use {} only; {} PE files inspected",
         uZlibImports, strExpected, vecFiles.size());
      return 0;
      }
   catch(std::exception const& theException) {
      std::println(stderr, "[PE-AUDIT] FAIL: {}", theException.what());
      return 1;
      }
   }
