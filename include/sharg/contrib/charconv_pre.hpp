// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

#pragma once

#include <cassert>
#include <system_error>
#include <limits>

#define SHARG_CONTRIB_CHARCONV_LIBCPP_BEGIN_NAMESPACE_STD namespace sharg::contrib::charconv {
#define SHARG_CONTRIB_CHARCONV_LIBCPP_END_NAMESPACE_STD }
#define SHARG_CONTRIB_CHARCONV_LIBCPP_STD_VER 17
#define SHARG_CONTRIB_CHARCONV_LIBCPP_FUNC_VIS
#define SHARG_CONTRIB_CHARCONV_LIBCPP_ENUM_VIS
#define SHARG_CONTRIB_CHARCONV_LIBCPP_TYPE_VIS
#define SHARG_CONTRIB_CHARCONV_LIBCPP_HIDDEN
#define SHARG_CONTRIB_CHARCONV_LIBCPP_INLINE_VISIBILITY
#define SHARG_CONTRIB_CHARCONV_LIBCPP_ASSERT(condition, message) assert(condition)

// import things from std namespace which charconv uses
namespace sharg::contrib::charconv
{
using std::errc;
using std::declval;
using std::numeric_limits;
using std::is_integral;
using std::is_unsigned;
using std::is_signed;
using std::make_unsigned_t;
using std::true_type;
using std::false_type;
using std::enable_if_t;

// forward declare
struct SHARG_CONTRIB_CHARCONV_LIBCPP_TYPE_VIS to_chars_result;

// forward declare to avoid "no matching function for call to
// ‘__to_chars_itoa(char*&, char*&, short unsigned int&, std::false_type)’"
template <typename _Tp>
inline SHARG_CONTRIB_CHARCONV_LIBCPP_INLINE_VISIBILITY to_chars_result
__to_chars_itoa(char* __first, char* __last, _Tp __value, false_type);

} // namespace sharg::contrib::charconv
