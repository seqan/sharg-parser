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
static_assert(__cplusplus >= 201709, "SHARG requires C++20, make sure that you have set -std=c++20.");
#else
#    error "This is not a C++ compiler."
#endif

#if __has_include(<version>)
#    include <version>
#endif

// ============================================================================
//  Dependencies
// ============================================================================

// SHARG [required]
#if __has_include(<sharg/version.hpp>)
#    include <sharg/version.hpp>
#else
#    error SHARG include directory not set correctly. Forgot to add -I ${INSTALLDIR}/include to your CXXFLAGS?
#endif

// SeqAn3 [required]
#if __has_include(<seqan3/version.hpp>)
#    include <seqan3/version.hpp>
static_assert(seqan3::seqan3_version_major == 3, "SeqAn >= 3.1 is required by SHARG.");
static_assert(seqan3::seqan3_version_minor >= 1, "SeqAn >= 3.1 is required by SHARG.");
#else
#    error The SeqAn3 library was not included.
#endif

// ============================================================================
//  Documentation
// ============================================================================

// Doxygen related
// this macro is a NO-OP unless doxygen parses it, in which case it resolves to the argument
#ifndef SHARG_DOXYGEN_ONLY
#    define SHARG_DOXYGEN_ONLY(x)
#endif
