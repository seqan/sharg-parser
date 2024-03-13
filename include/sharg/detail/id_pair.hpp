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

#include <sharg/platform.hpp>

//!\cond

namespace sharg::detail
{

/*!\brief A simple struct to store a short and a long identifier for an option.
 * \ingroup parser
 */
struct id_pair
{
    char short_id{};       //!< The short identifier for the option.
    std::string long_id{}; //!< The long identifier for the option.

    id_pair() = default;
    id_pair(id_pair const &) = default;
    id_pair & operator=(id_pair const &) = default;
    id_pair(id_pair &&) = default;
    id_pair & operator=(id_pair &&) = default;
    ~id_pair() = default;

    id_pair(char const short_id) : short_id{short_id}
    {}

    id_pair(std::string long_id) : long_id{std::move(long_id)}
    {}

    id_pair(char const short_id, std::string long_id) : short_id{short_id}, long_id{std::move(long_id)}
    {}

    //!\brief Compares two id_pairs for equality.
    friend bool operator==(id_pair const & lhs, id_pair const & rhs)
    {
        return (!lhs.empty_short_id() && lhs.short_id == rhs.short_id)
            || (!lhs.empty_long_id() && lhs.long_id == rhs.long_id);
    }

    friend bool operator==(id_pair const & lhs, char const & rhs)
    {
        return !lhs.empty_short_id() && lhs.short_id == rhs;
    }

    friend bool operator==(id_pair const & lhs, std::string const & rhs)
    {
        return !lhs.empty_long_id() && lhs.long_id == rhs;
    }

    bool empty_short_id() const
    {
        return empty(short_id);
    }

    bool empty_long_id() const
    {
        return empty(long_id);
    }

    bool empty() const
    {
        return empty_short_id() && empty_long_id();
    }

    template <typename id_type>
    static bool empty(id_type const & id);

    template <typename id_type>
    static auto find(std::unordered_set<id_pair> const & used_ids, id_type const & id);

    template <typename id_type>
    static bool contains(std::unordered_set<id_pair> const & used_ids, id_type const & id);
};

} // namespace sharg::detail

namespace std
{

template <>
struct hash<sharg::detail::id_pair>
{
    size_t operator()(sharg::detail::id_pair const & value) const noexcept
    {
        size_t const h1 = std::hash<char>{}(value.short_id);
        size_t const h2 = std::hash<std::string>{}(value.long_id);
        return h1 ^ (h2 << 1);
    }
};

} // namespace std

namespace sharg::detail
{

template <typename id_type>
inline bool id_pair::empty(id_type const & id)
{
    if constexpr (std::same_as<id_type, id_pair>)
        return id.empty();
    else if constexpr (std::same_as<id_type, char>)
        return id == '\0';
    else
        return id.empty();
}

template <typename id_type>
inline auto id_pair::find(std::unordered_set<id_pair> const & used_ids, id_type const & id)
{
    if (empty(id))
        return used_ids.end();

    return std::ranges::find_if(used_ids,
                                [&id](id_pair const & pair)
                                {
                                    return pair == id;
                                });
}

template <typename id_type>
inline bool id_pair::contains(std::unordered_set<id_pair> const & used_ids, id_type const & id)
{
    return find(used_ids, id) != used_ids.end();
}

} // namespace sharg::detail

//!\endcond
