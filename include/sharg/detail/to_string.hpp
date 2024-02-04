// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides sharg::detail::to_string.
 */

#pragma once

#include <sstream>

#include <sharg/concept.hpp>
#include <sharg/detail/concept.hpp>

namespace sharg::detail
{

//!\brief A string containing all supported export formats.
static std::string const supported_exports =
#if SHARG_HAS_TDL
    "[html, man, ctd, cwl]";
#else
    "[html, man]";
#endif

//!\brief Concept for views whose value type is ostreamable.
template <typename container_t>
concept is_ostreamable_view = std::ranges::view<container_t> && ostreamable<std::ranges::range_value_t<container_t>>;

/*!\brief Streams all parameters via std::ostringstream and returns a concatenated string.
 * \ingroup misc
 * \tparam    value_types Must be sharg::ostreamable (stream << value).
 * \param[in] values     Variable number of parameters of any type that implement the stream operator.
 * \returns A concatenated string of all values (no separator in between is added).
 */
template <typename... value_types>
    requires (ostreamable<value_types> && ...)
std::string to_string(value_types &&... values)
{
    std::stringstream stream;

    auto print = [&stream](auto && val)
    {
        using value_t = std::remove_cvref_t<decltype(val)>;

        // When passing a `std::vector<std::string> | std::views::transform(...)` which returns `std::quoted(str)` for
        // each element, the `std::quoted`'s return value does not model a range, but is ostreamable.
        if constexpr (is_container_option<value_t> || is_ostreamable_view<value_t>)
        {
            if (val.empty())
            {
                stream << "[]";
            }
            else
            {
                stream << '[';
                auto it = val.begin();
                stream << *it++;
                for (; it != val.end(); ++it)
                    stream << ", " << *it;
                stream << ']';
            }
        }
        else if constexpr (std::is_same_v<value_t, int8_t> || std::is_same_v<value_t, uint8_t>)
        {
            stream << static_cast<int16_t>(val);
        }
        else
        {
            stream << val;
        }
    };

    (print(std::forward<value_types>(values)), ...);

    return stream.str();
}

} // namespace sharg::detail
