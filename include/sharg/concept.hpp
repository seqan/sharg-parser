// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

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
 * \ingroup misc
 * \brief Concept for types that can be parsed from a std::istream via the stream operator.
 * \tparam value_type The type to check whether it's stremable via std::istream.
 *
 * Read up on how to make a type model this concept here:
 *
 * \ref fulfil_parsable.
 *
 * ### Requirements
 *
 * `std::istream` must support the (un)formatted input function (`operator>>`) for an l-value of a given `value_type`.
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
// clang-format off
template <typename value_type>
concept istreamable = requires (std::istream & is, value_type & val)
{
    {is >> val} -> std::same_as<std::istream&>;
};
// clang-format on

/*!\concept sharg::ostreamable
 * \ingroup misc
 * \brief Concept for types that can be parsed into a std::ostream via the stream operator.
 * \tparam type The type to check whether it's stremable via std::ostream or it's a container over streamable values.
 *
 * Read up on how to make a type model this concept here:
 *
 * \ref fulfil_parsable.
 *
 * ### Requirements
 *
 * `std::ostream` must support the (un)formatted output function (`operator<<`) for an l-value of a given `type` or
 * for an l-value of `type::reference`.
 *
 * \stableapi{Since version 1.0.}
 */
// clang-format off
template <typename type>
concept ostreamable = requires (std::ostream & os, type & val)
                      {
                          {os << val} -> std::same_as<std::ostream&>;
                      }
                    ||
                      requires (std::ostream & os, std::ranges::range_reference_t<type> val)
                      {
                          {os << val} -> std::same_as<std::ostream&>;
                      };
// clang-format on

/*!\concept sharg::parsable
 * \brief Checks whether the the type can be used in an add_(positional_)option call on the parser.
 * \ingroup misc
 * \tparam option_type The type to check.
 *
 * Read up on how to make a type model this concept here:
 *
 * \ref fulfil_parsable.
 *
 * ### Requirements
 *
 * In order to model this concept, the type must either model sharg::istreamable and sharg::ostreamable or
 * model sharg::named_enumeration<option_type>.
 *
 * \remark For a complete overview, take a look at \ref parser
 *
 * \stableapi{Since version 1.0.}
 */
template <typename option_type>
concept parsable =
    (sharg::istreamable<option_type> && sharg::ostreamable<option_type>) || named_enumeration<option_type>;

} // namespace sharg
