// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides the concept sharg::detail::is_container_option.
 */

#pragma once

#include <string>

#include <sharg/platform.hpp>

namespace sharg::detail
{

/*!\concept sharg::detail::is_container_option
 * \ingroup misc
 * \brief Whether the option type is considered to be a container.
 * \details
 *
 * When adding options or positionial arguments, a distinction needs to be made between container and non-container
 * `option_type`s.
 *
 * In general, all standard library containers except std::string can be considered containers.
 *
 * In order to be considered a container, the `option_type` must:
 * * not be `std::string`
 * * define a member type `value_type`
 * * provide a member function `push_back(value_type)`
 *
 * \noapi
 */
// clang-format off
template <typename option_type>
concept is_container_option = (!std::is_same_v<std::remove_cvref_t<option_type>, std::string>) &&
                              requires (option_type container,
                                        typename std::ranges::range_value_t<option_type> value)
                              {
                                  { container.push_back(value) };
                              };
// clang-format on

} // namespace sharg::detail
