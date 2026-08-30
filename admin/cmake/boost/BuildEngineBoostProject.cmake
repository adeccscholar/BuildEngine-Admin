# Copyright (c) 2026 adecc Systemhaus GmbH
# SPDX-License-Identifier: MIT
# Project: adecc Scholar
#
# Producer integration for the target-verified Boost 1.92.0 BCC64X CMake model.
# Upstream Boost source remains untouched.

include("${CMAKE_CURRENT_LIST_DIR}/bcc64x-native-clang/project-include.cmake")

function(_adecc_boost_validate_managed_openssl)
   foreach(theVar IN ITEMS
      OPENSSL_INCLUDE_DIR
      ADECC_BOOST_OPENSSL_SSL_LIBRARY
      ADECC_BOOST_OPENSSL_CRYPTO_LIBRARY)
      if(NOT DEFINED ${theVar} OR "${${theVar}}" STREQUAL "")
         message(FATAL_ERROR "Boost producer requires pinned ${theVar}")
      endif()
   endforeach()

   foreach(theFile IN ITEMS
      "${OPENSSL_INCLUDE_DIR}/openssl/ssl.h"
      "${ADECC_BOOST_OPENSSL_SSL_LIBRARY}"
      "${ADECC_BOOST_OPENSSL_CRYPTO_LIBRARY}")
      if(NOT EXISTS "${theFile}")
         message(FATAL_ERROR "Boost producer managed OpenSSL input missing: ${theFile}")
      endif()
   endforeach()

   # CMake 4.1 FindOpenSSL uses SSL_EAY/LIB_EAY on the WIN32 branch when
   # neither MSVC nor MINGW is set.  BCC64X follows that branch.  Seed the
   # actual find_library cache variables, not merely the documented results.
   set(SSL_EAY "${ADECC_BOOST_OPENSSL_SSL_LIBRARY}" CACHE FILEPATH "Managed OpenSSL SSL import library" FORCE)
   set(LIB_EAY "${ADECC_BOOST_OPENSSL_CRYPTO_LIBRARY}" CACHE FILEPATH "Managed OpenSSL Crypto import library" FORCE)
   set(OPENSSL_SSL_LIBRARY "${ADECC_BOOST_OPENSSL_SSL_LIBRARY}" CACHE FILEPATH "Managed OpenSSL SSL import library" FORCE)
   set(OPENSSL_CRYPTO_LIBRARY "${ADECC_BOOST_OPENSSL_CRYPTO_LIBRARY}" CACHE FILEPATH "Managed OpenSSL Crypto import library" FORCE)

   find_package(OpenSSL MODULE REQUIRED COMPONENTS SSL Crypto)
   if(NOT TARGET OpenSSL::SSL OR NOT TARGET OpenSSL::Crypto)
      message(FATAL_ERROR "Boost producer requires OpenSSL::SSL and OpenSSL::Crypto")
   endif()

   file(REAL_PATH "${ADECC_BOOST_OPENSSL_SSL_LIBRARY}" _adecc_expected_ssl)
   file(REAL_PATH "${ADECC_BOOST_OPENSSL_CRYPTO_LIBRARY}" _adecc_expected_crypto)
   file(REAL_PATH "${OPENSSL_SSL_LIBRARY}" _adecc_result_ssl)
   file(REAL_PATH "${OPENSSL_CRYPTO_LIBRARY}" _adecc_result_crypto)
   if(NOT _adecc_result_ssl STREQUAL _adecc_expected_ssl OR
      NOT _adecc_result_crypto STREQUAL _adecc_expected_crypto)
      message(FATAL_ERROR "Boost producer FindOpenSSL escaped the managed package")
   endif()
endfunction()

function(_adecc_boost_apply_cobalt_windows_libraries)
   if(NOT TARGET boost_cobalt)
      message(FATAL_ERROR "Boost.Cobalt expected target boost_cobalt is missing")
   endif()
   target_link_libraries(boost_cobalt PUBLIC ws2_32 mswsock bcrypt)
endfunction()

function(_adecc_boost_verify_protocol_targets)
   foreach(theTarget IN ITEMS boost_cobalt boost_mysql boost_redis)
      if(NOT TARGET ${theTarget})
         message(FATAL_ERROR "Requested Boost protocol target is absent: ${theTarget}")
      endif()
   endforeach()
endfunction()

# R193 compatibility decision: BCC64X uses the working std::locale/codecvt
# path, but not Boost.Locale's WinAPI backend.  The upstream CMake default
# enables that backend solely from WIN32, which is too broad for this target.
if(PROJECT_NAME STREQUAL "boost_locale" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
   set(BOOST_LOCALE_ENABLE_WINAPI OFF CACHE BOOL "Boost.Locale WinAPI backend disabled for BCC64X" FORCE)
   message(STATUS "Boost.Locale BCC64X policy: WinAPI backend OFF; std::locale backend remains enabled")
endif()

get_property(_adecc_boost_openssl_checked GLOBAL PROPERTY ADECC_BOOST_OPENSSL_CHECKED)
if(NOT _adecc_boost_openssl_checked)
   _adecc_boost_validate_managed_openssl()
   set_property(GLOBAL PROPERTY ADECC_BOOST_OPENSSL_CHECKED TRUE)
   cmake_language(DEFER DIRECTORY "${CMAKE_SOURCE_DIR}" CALL _adecc_boost_verify_protocol_targets)
endif()

if(PROJECT_NAME STREQUAL "boost_cobalt" AND CMAKE_SYSTEM_NAME STREQUAL "Windows")
   cmake_language(DEFER CALL _adecc_boost_apply_cobalt_windows_libraries)
endif()
