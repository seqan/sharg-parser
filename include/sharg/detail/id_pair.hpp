// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \author Enrico Seiler <enrico.seiler AT fu-berlin.de>
 * \brief Provides sharg::detail::id_pair.
 */

#pragma once

#include <string>
#include <unordered_set>

//!\cond

namespace sharg::detail
{

/*!\brief A simple struct to store a short and a long identifier for an option.
 * \ingroup parser
 */
struct id_pair
{
    std::string short_id{}; //!< The short identifier for the option.
    std::string long_id{};  //!< The long identifier for the option.

    id_pair() = default;
    id_pair(id_pair const &) = default;
    id_pair(id_pair &&) = default;
    id_pair & operator=(id_pair const &) = default;
    id_pair & operator=(id_pair &&) = default;
    ~id_pair() = default;

    id_pair(std::string id_short, std::string id_long) : short_id{std::move(id_short)}, long_id{std::move(id_long)}
    {}

    id_pair(char const id_short, std::string id_long) :
        short_id{std::string(id_short != '\0', id_short)},
        long_id{std::move(id_long)}
    {}

    id_pair(char const id_short) : short_id{std::string(id_short != '\0', id_short)}
    {}

    id_pair(std::string id_long) : long_id{std::move(id_long)}
    {}

    //!\brief Compares two id_pairs for equality.
    friend auto operator<=>(id_pair const &, id_pair const &) = default;

    static auto find(std::unordered_set<id_pair> const & used_ids, char const short_id);

    static auto find(std::unordered_set<id_pair> const & used_ids, std::string const & long_id);

    static bool contains(std::unordered_set<id_pair> const & used_ids, char const short_id);

    static bool contains(std::unordered_set<id_pair> const & used_ids, std::string const & long_id);
};

} // namespace sharg::detail

namespace std
{

template <>
struct hash<sharg::detail::id_pair>
{
    size_t operator()(sharg::detail::id_pair const & value) const noexcept
    {
        size_t h1 = std::hash<std::string>{}(value.short_id);
        size_t h2 = std::hash<std::string>{}(value.long_id);
        return h1 ^ (h2 << 1);
    }
};

} // namespace std

namespace sharg::detail
{

inline auto id_pair::find(std::unordered_set<id_pair> const & used_ids, char const short_id)
{
    std::string const id_str(1, short_id);
    auto it = std::ranges::find_if(used_ids,
                                   [&id_str](id_pair const & id)
                                   {
                                       return id.short_id == id_str;
                                   });
    return it;
}

inline auto id_pair::find(std::unordered_set<id_pair> const & used_ids, std::string const & long_id)
{
    auto it = std::ranges::find_if(used_ids,
                                   [&long_id](id_pair const & id)
                                   {
                                       return id.long_id == long_id;
                                   });
    return it;
}

inline bool id_pair::contains(std::unordered_set<id_pair> const & used_ids, char const short_id)
{
    return id_pair::find(used_ids, short_id) != used_ids.end();
}

inline bool id_pair::contains(std::unordered_set<id_pair> const & used_ids, std::string const & long_id)
{
    return id_pair::find(used_ids, long_id) != used_ids.end();
}

} // namespace sharg::detail

//!\endcond
