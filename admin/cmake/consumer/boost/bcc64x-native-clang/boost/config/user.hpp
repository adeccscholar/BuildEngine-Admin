// Copyright (c) 2026 adecc Systemhaus GmbH
// SPDX-License-Identifier: MIT
// Project: adecc Scholar
#ifndef ADECC_BOOST_BCC64X_NATIVE_CLANG_USER_CONFIG_HPP
#define ADECC_BOOST_BCC64X_NATIVE_CLANG_USER_CONFIG_HPP

#if !defined(__CODEGEARC__) || !defined(__clang__)
#  error "Boost native-Clang config override is only valid for Clang-based Embarcadero BCC64X"
#endif

// Capability-based BCC64X policy proven by the Boost 1.92.0 evidence gates.
// Do not patch Boost.Config or individual Boost defect macros.  Current BCC64X
// is routed through Boost's upstream Clang compiler configuration directly.
#ifndef BOOST_COMPILER_CONFIG
#  define BOOST_COMPILER_CONFIG "boost/config/compiler/clang.hpp"
#endif

#endif
