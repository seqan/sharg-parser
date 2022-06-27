// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
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

/*!\brief Streams all parameters via std::ostringstream and returns a concatenated string.
 * \tparam    value_types Must be sharg::ostreamable (stream << value).
 * \param[in] values     Variable number of parameters of any type that implement the stream operator.
 * \returns A concatenated string of all values (no separator in between is added).
 */
template <typename... value_types>
    requires (ostreamable<value_types> && ...)
std::string to_string(value_types &&... values)
{
    std::stringstream stream;

    auto print = [&stream](auto val)
    {
        if constexpr (is_container_option<decltype(val)>)
        {
            if (val.empty())
            {
                stream << "[]";
            }
            else
            {
                stream << '[';
                stream << val[0];
                for (size_t i = 1; i < val.size(); ++i)
                    stream << ", " << val[i];
                stream << ']';
            }
        }
        else if constexpr (std::is_same_v<std::remove_cvref_t<decltype(val)>, int8_t>
                           || std::is_same_v<std::remove_cvref_t<decltype(val)>, uint8_t>)
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
