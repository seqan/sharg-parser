// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides helper concepts.
 */

#pragma once

#include <concepts>

#include <sharg/enumeration_names.hpp>

namespace sharg
{

/*!\concept sharg::istreamable
 * \ingroup parser
 * \brief Concept for types that can be parsed from a std::istream via the stream operator.
 * \tparam value_type The type to check whether it's stremable via std::istream.
 *
 * Read up on how to make a type model this concept here:
 *
 * \ref fulfil_parser_compatible_option.
 *
 * ### Requirements
 *
 * `std::istream` must support the (un)formatted input function (`operator>>`) for an l-value of a given `value_type`.
 */
template <typename value_type>
concept istreamable = requires (std::istream & is, value_type & val)
{
    {is >> val} -> std::same_as<std::istream&>;
};

/*!\concept sharg::ostreamable
 * \ingroup parser
 * \brief Concept for types that can be parsed into a std::ostream via the stream operator.
 * \tparam type The type to check whether it's stremable via std::ostream or it's a container over streamable values.
 *
 * Read up on how to make a type model this concept here:
 *
 * \ref fulfil_parser_compatible_option.
 *
 * ### Requirements
 *
 * `std::ostream` must support the (un)formatted output function (`operator<<`) for an l-value of a given `type` or
 * for an l-value of `type::reference`.
 */
template <typename type>
concept ostreamable = requires (std::ostream & os, type & val)
{
    {os << val} -> std::same_as<std::ostream&>;
} ||
requires (std::ostream & os, type & con)
{
    {os << con[0]} -> std::same_as<std::ostream&>;
};

/*!\concept sharg::parser_compatible_option
 * \brief Checks whether the the type can be used in an add_(positional_)option call on the parser.
 * \ingroup parser
 * \tparam option_type The type to check.
 *
 * Read up on how to make a type model this concept here:
 *
 * \ref fulfil_parser_compatible_option.
 *
 * ### Requirements
 *
 * In order to model this concept, the type must either model sharg::istreamable and sharg::ostreamable or
 * model sharg::named_enumeration<option_type>.
 *
 * \remark For a complete overview, take a look at \ref parser
 */
template <typename option_type>
concept parser_compatible_option = (sharg::istreamable<option_type> && sharg::ostreamable<option_type>) ||
                                    named_enumeration<option_type>;

} // namespace sharg
