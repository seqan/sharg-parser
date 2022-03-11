// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides auxiliary information.
 */

#pragma once

#include <string_view>
#include <unordered_map>
#include <iostream>

#include <sharg/platform.hpp>

namespace sharg::custom
{

/*!\brief A type that can be specialised to provide customisation point implementations for the sharg::argument_parser
 *        such that third party types may be adapted.
 * \tparam t The type you wish to specialise for.
 * \ingroup argument_parser
 *
 * \details
 *
 * ### Named Enumerations
 *
 * In order to use a third party type within the sharg::argument_parser::add_option or
 * sharg::argument_parser::add_positional_option call, you can specialise this struct in the following way:
 *
 * \include test/snippet/argument_parser/custom_argument_parsing_enumeration.cpp
 *
 * Please note that by default the `t const`, `t &` and `t const &` specialisations of this class inherit the
 * specialisation for `t` so you usually only need to provide a specialisation for `t`.
 *
 * \note Only use this if you cannot provide respective functions in your namespace. See the tutorial
 * \ref tutorial_argument_parser for an example of customising a type within your own namespace.
 *
 * \remark For a complete overview, take a look at \ref argument_parser
 */
template <typename t>
struct argument_parsing
{}; // forward

//!\cond
template <typename t>
struct argument_parsing<t const> : argument_parsing<t>
{};

template <typename t>
struct argument_parsing<t &> : argument_parsing<t>
{};

template <typename t>
struct argument_parsing<t const &> : argument_parsing<t>
{};
//!\endcond

} // sharg::custom

namespace sharg::detail
{
//!\brief A tag that allows controlled overload resolution via implicit base conversion rules.
//!\ingroup core
template <size_t I>
struct priority_tag
//!\cond
// Doxygen fail
: priority_tag<I-1>
//!\endcond
{};

//!\brief Recursion anchor for sharg::detail::priority_tag.
template <>
struct priority_tag<0>
{};

} // sharg::detail

namespace sharg::detail::adl_only
{

//!\brief Poison-pill overload to prevent non-ADL forms of unqualified lookup.
template <typename t>
std::unordered_map<std::string_view, t> enumeration_names(t) = delete;

//!\brief sharg::detail::customisation_point_object (CPO) definition for sharg::enumeration_names.
//!\ingroup argument_parser
//!\remark For a complete overview, take a look at \ref argument_parser
template <typename option_t>
struct enumeration_names_cpo
{
    /*!\name Constructors, destructor and assignment
     * \{
     */
    constexpr enumeration_names_cpo() = default; //!< Defaulted.
    constexpr enumeration_names_cpo(enumeration_names_cpo &&) = default; //!< Defaulted.
    constexpr enumeration_names_cpo(enumeration_names_cpo const &) = default; //!< Defaulted.
    constexpr enumeration_names_cpo & operator=(enumeration_names_cpo &&) = default; //!< Defaulted.
    constexpr enumeration_names_cpo & operator=(enumeration_names_cpo const &) = default; //!< Defaulted.
    //!\}

    /*!\brief If `option_t` isn't std::is_nothrow_default_constructible, enumeration_names will be called with
     *        std::type_identity instead of a default constructed alphabet.
     */
    template <typename option_type>
    using option_or_type_identity
        = std::conditional_t<std::is_nothrow_default_constructible_v<std::remove_cvref_t<option_type>>,
                             std::remove_cvref_t<option_type>,
                             std::type_identity<option_type>>;

    /*!\brief CPO overload (check 1 out of 2): explicit customisation via `sharg::custom::argument_parsing`
     * \tparam option_type The type of the option. (Needed to defer instantiation for incomplete types.)
     */
    template <typename option_type = option_t>
    static constexpr auto cpo_overload(sharg::detail::priority_tag<1>)
    noexcept(noexcept(sharg::custom::argument_parsing<option_type>::enumeration_names)) \
      -> decltype(sharg::custom::argument_parsing<option_type>::enumeration_names) \
    {
        return sharg::custom::argument_parsing<option_type>::enumeration_names;
    }

    /*!\brief CPO overload (check 1 out of 2): argument dependent lookup (ADL), i.e.
     *        `enumeration_names(option_type{})`
     * \tparam option_type The type of the option. (Needed to defer instantiation for incomplete types.)
     *
     * \details
     *
     * If the option_type isn't std::is_nothrow_default_constructible,
     * `enumeration_names(std::type_identity<option_t>{})` will be called.
     */
    template <typename option_type = option_t>
    static constexpr auto cpo_overload(sharg::detail::priority_tag<0>)
    noexcept(noexcept(enumeration_names(option_or_type_identity<option_type>{})))
      -> decltype(enumeration_names(option_or_type_identity<option_type>{}))
    {
        return enumeration_names(option_or_type_identity<option_type>{});
    }

    /*!\brief SFINAE-friendly call-operator to resolve CPO overload resolution.
     *
     * This operator implements the actual CPO overload resolution. Overload resolution will try each base class of
     * sharg::detail::priority_tag<1> and sharg::detail::priority_tag<0>.
     * sharg::detail::priority_tag<0> as first argument to `cpo_overload`. That means a high priority in
     * sharg::detail::priority_tag will be evaluated first and one can define an order which overload should be
     * prioritised if multiple overloads match.
     *
     * It perfectly forwards the result and noexcept-property of the `cpo_overload`.
     */
    template <typename ...args_t, typename option_type = option_t /*circumvent incomplete types*/>
    constexpr auto operator()(args_t && ...args) const
    noexcept(noexcept(cpo_overload(sharg::detail::priority_tag<1>{}, std::forward<args_t>(args)...)))
      -> decltype(cpo_overload(sharg::detail::priority_tag<1>{}, std::forward<args_t>(args)...))
    {
        return cpo_overload(sharg::detail::priority_tag<1>{}, std::forward<args_t>(args)...);
    }
};

} // namespace sharg::detail::adl_only

namespace sharg
{

/*!\name Customisation Points
 * \{
 */

/*!\brief Return a conversion map from std::string_view to option_type.
 * \tparam your_type Type of the value to retrieve the conversion map for.
 * \param value The value is not used, just its type.
 * \returns A std::unordered_map<std::string_view, your_type> that maps a string identifier to a value of your_type.
 * \ingroup argument_parser
 * \details
 *
 * This is a function object. Invoke it with the parameter(s) specified above.
 *
 * It acts as a wrapper and looks for two possible implementations (in this order):
 *
 *   1. A static member `enumeration_names` in `sharg::custom::argument_parsing<your_type>` that is of type
 *      `std::unordered_map<std::string_view, your_type>>`.
 *   2. A free function `enumeration_names(your_type const a)` in the namespace of your type (or as `friend`) which
 *      returns a `std::unordered_map<std::string_view, your_type>>`.
 *
 * ### Example
 *
 * If you are working on a type in your own namespace, you should implement a free function like this:
 *
 * \include test/snippet/argument_parser/custom_enumeration.cpp
 *
 * **Only if you cannot access the namespace of your type to customize** you may specialize
 * the sharg::custom::argument_parsing struct like this:
 *
 * \include test/snippet/argument_parser/custom_argument_parsing_enumeration.cpp
 *
 * \remark For a complete overview, take a look at \ref argument_parser
 *
 * ### Customisation point
 *
 * This is a customisation point (see \ref about_customisation). To specify the behaviour for your own type,
 * simply provide one of the two functions specified above.
 */
template <typename option_type>
//!\cond
    requires requires { { detail::adl_only::enumeration_names_cpo<option_type>{}() }; }
//!\endcond
inline auto const enumeration_names = detail::adl_only::enumeration_names_cpo<option_type>{}();
//!\}

/*!\concept sharg::named_enumeration
 * \brief Checks whether the free function sharg::enumeration_names can be called on the type.
 * \ingroup argument_parser
 * \tparam option_type The type to check.
 *
 * ### Requirements
 *
 * * A instance of sharg::enumeration_names<option_type> must exist and be of type
 *   `std::unordered_map<std::string, option_type>`.
 *
 * \remark For a complete overview, take a look at \ref argument_parser
 */
template <typename option_type>
concept named_enumeration = requires
{
    { sharg::enumeration_names<option_type> };
};

} // namespace sharg

//!\cond
namespace std
{
template <typename option_type>
    requires sharg::named_enumeration<std::remove_cvref_t<option_type>>
inline ostream & operator<<(ostream & s, option_type && op)
{
    for (auto & [key, value] : sharg::enumeration_names<option_type>)
    {
        if (op == value)
            return s << key;
    }

    return s << "<UNKNOWN_VALUE>";
}
} // namespace std
//!\endcond
