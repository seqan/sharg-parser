// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \brief Provides platform and dependency checks.
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 */

#pragma once

#include <version>

// ============================================================================
//  Compiler support general
// ============================================================================

/*!\def SHARG_COMPILER_IS_GCC
 * \brief Whether the current compiler is GCC.
 * \details
 * __GNUC__ is also used to indicate the support for GNU compiler extensions. To detect the presence of the GCC
 * compiler, one has to rule out other compilers.
 *
 * \sa https://sourceforge.net/p/predef/wiki/Compilers
 */
#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER) && !defined(__INTEL_LLVM_COMPILER)
#    define SHARG_COMPILER_IS_GCC 1
#else
#    define SHARG_COMPILER_IS_GCC 0
#endif

// ============================================================================
//  Compiler support
// ============================================================================

#if SHARG_COMPILER_IS_GCC && (__GNUC__ < 12)
#    error "Sharg requires at least GCC 12."
#endif

// clang-format off
#if defined(__INTEL_LLVM_COMPILER) && (__INTEL_LLVM_COMPILER < 20240000)
#    error "Sharg requires at least Intel OneAPI 2024."
#endif
// clang-format on

#if defined(__clang__) && defined(__clang_major__) && (__clang_major__ < 17)
#    error "Sharg requires at least Clang 17."
#endif

// ============================================================================
//  Standard library support
// ============================================================================

#if defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION < 170000)
#    error "Sharg requires at least libc++ 17."
#endif

#if defined(_GLIBCXX_RELEASE) && (_GLIBCXX_RELEASE < 12)
#    error "Sharg requires at least libstdc++ 12."
#endif

// ============================================================================
//  C++ standard and features
// ============================================================================

// C++ standard [required]
#ifdef __cplusplus
#    if (__cplusplus < 202100)
#        error "Sharg requires C++23, make sure that you have set -std=c++23."
#    endif
#else
#    error "This is not a C++ compiler."
#endif

// ============================================================================
//  Dependencies
// ============================================================================

// Sharg [required]
#if __has_include(<sharg/version.hpp>)
#    include <sharg/version.hpp>
#else
#    error "Sharg include directory not set correctly. Forgot to add -I ${INSTALLDIR}/include to your CXXFLAGS?"
#endif

// ============================================================================
//  Documentation
// ============================================================================

// Doxygen related
// this macro is a NO-OP unless doxygen parses it, in which case it resolves to the argument
#ifndef SHARG_DOXYGEN_ONLY
#    define SHARG_DOXYGEN_ONLY(x)
#endif
