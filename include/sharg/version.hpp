// -----------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2020, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2020, Knut Reinert & MPI für molekulare Genetik
// Copyright (c) 2020-2021, deCODE Genetics
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/b.i.o./blob/master/LICENSE
// -----------------------------------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <cstdint>

/*!\file
 * \brief Provides SARG version macros and global variables.
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 */

//!\brief The major version as MACRO.
#define SARG_VERSION_MAJOR 0
//!\brief The minor version as MACRO.
#define SARG_VERSION_MINOR 1
//!\brief The patch version as MACRO.
#define SARG_VERSION_PATCH 0

//!\brief The full version as MACRO (number).
#define SARG_VERSION (SARG_VERSION_MAJOR * 10000 + SARG_VERSION_MINOR * 100 + SARG_VERSION_PATCH)

/*!\brief Converts a number to a string. Preprocessor needs this indirection to
 * properly expand the values to strings.
 */
#define SARG_VERSION_CSTRING_HELPER_STR(str) #str

//!\brief Converts version numbers to string.
#define SARG_VERSION_CSTRING_HELPER_FUNC(MAJOR, MINOR, PATCH)                                                           \
    SARG_VERSION_CSTRING_HELPER_STR(MAJOR)                                                                              \
    "." SARG_VERSION_CSTRING_HELPER_STR(MINOR) "." SARG_VERSION_CSTRING_HELPER_STR(PATCH)

//!\brief The full version as null terminated string.
#define SARG_VERSION_CSTRING SARG_VERSION_CSTRING_HELPER_FUNC(SARG_VERSION_MAJOR, SARG_VERSION_MINOR, SARG_VERSION_PATCH)

namespace sarg
{

//!\brief The major version.
constexpr uint8_t sarg_version_major = SARG_VERSION_MAJOR;
//!\brief The minor version.
constexpr uint8_t sarg_version_minor = SARG_VERSION_MINOR;
//!\brief The patch version.
constexpr uint8_t sarg_version_patch = SARG_VERSION_PATCH;

//!\brief The full version as `std::size_t`.
constexpr std::size_t sarg_version = SARG_VERSION;

//!\brief The full version as null terminated string.
constexpr char const * sarg_version_cstring = SARG_VERSION_CSTRING;

} // namespace sarg

#undef SARG_VERSION_CSTRING_HELPER_STR
#undef SARG_VERSION_CSTRING_HELPER_FUNC
