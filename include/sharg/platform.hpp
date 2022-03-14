// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

#pragma once

/*!\file
 * \brief Provides platform and dependency checks.
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 */

// ============================================================================
//  C++ standard and features
// ============================================================================

// C++ standard [required]
#ifdef __cplusplus
static_assert(__cplusplus >= 201709L, "SHARG requires C++20, make sure that you have set -std=c++20.");
#else
#    error "This is not a C++ compiler."
#endif

#include <version>

// ============================================================================
//  Dependencies
// ============================================================================

// SHARG [required]
#if __has_include(<sharg/version.hpp>)
#    include <sharg/version.hpp>
#else
#    error SHARG include directory not set correctly. Forgot to add -I ${INSTALLDIR}/include to your CXXFLAGS?
#endif

// ============================================================================
//  Documentation
// ============================================================================

// Doxygen related
// this macro is a NO-OP unless doxygen parses it, in which case it resolves to the argument
#ifndef SHARG_DOXYGEN_ONLY
#    define SHARG_DOXYGEN_ONLY(x)
#endif
