// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

#pragma once

#include <cstddef>
#include <cstdint>

/*!\file
 * \brief Provides SHARG version macros and global variables.
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 */

//!\brief The major version as MACRO.
#define SHARG_VERSION_MAJOR 0
//!\brief The minor version as MACRO.
#define SHARG_VERSION_MINOR 1
//!\brief The patch version as MACRO.
#define SHARG_VERSION_PATCH 0

//!\brief The full version as MACRO (number).
#define SHARG_VERSION (SHARG_VERSION_MAJOR * 10000 + SHARG_VERSION_MINOR * 100 + SHARG_VERSION_PATCH)

/*!\brief Converts a number to a string. Preprocessor needs this indirection to
 * properly expand the values to strings.
 */
#define SHARG_VERSION_CSTRING_HELPER_STR(str) #str

//!\brief Converts version numbers to string.
#define SHARG_VERSION_CSTRING_HELPER_FUNC(MAJOR, MINOR, PATCH)                                                           \
    SHARG_VERSION_CSTRING_HELPER_STR(MAJOR)                                                                              \
    "." SHARG_VERSION_CSTRING_HELPER_STR(MINOR) "." SHARG_VERSION_CSTRING_HELPER_STR(PATCH)

//!\brief The full version as null terminated string.
#define SHARG_VERSION_CSTRING SHARG_VERSION_CSTRING_HELPER_FUNC(SHARG_VERSION_MAJOR, SHARG_VERSION_MINOR, SHARG_VERSION_PATCH)

namespace sharg
{

//!\brief The major version.
constexpr uint8_t sharg_version_major = SHARG_VERSION_MAJOR;
//!\brief The minor version.
constexpr uint8_t sharg_version_minor = SHARG_VERSION_MINOR;
//!\brief The patch version.
constexpr uint8_t sharg_version_patch = SHARG_VERSION_PATCH;

//!\brief The full version as `std::size_t`.
constexpr std::size_t sharg_version = SHARG_VERSION;

//!\brief The full version as null terminated string.
constexpr char const * sharg_version_cstring = SHARG_VERSION_CSTRING;

} // namespace sharg

#undef SHARG_VERSION_CSTRING_HELPER_STR
#undef SHARG_VERSION_CSTRING_HELPER_FUNC
