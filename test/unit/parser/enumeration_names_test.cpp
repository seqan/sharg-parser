// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>
#include <sharg/test/expect_throw_msg.hpp>
#include <sharg/test/test_fixture.hpp>

namespace foo
{

enum class bar : uint8_t
{
    one,
    two,
    three
};

auto enumeration_names(bar)
{
    return std::unordered_map<std::string_view, bar>{{"one", bar::one}, {"two", bar::two}, {"three", bar::three}};
}

} // namespace foo

namespace Other
{

enum class bar : uint8_t
{
    one,
    two
};

} // namespace Other

namespace sharg::custom
{

template <>
struct parsing<Other::bar>
{
    static inline std::unordered_map<std::string_view, Other::bar> const enumeration_names{{"one", Other::bar::one},
                                                                                           {"1", Other::bar::one},
                                                                                           {"two", Other::bar::two},
                                                                                           {"2", Other::bar::two}};
};

} // namespace sharg::custom

class enumeration_names_test : public sharg::test::test_fixture
{};

TEST_F(enumeration_names_test, parse_success_enum_option)
{
    foo::bar value{};
    Other::bar value2{};

    auto parser = get_parser("-e", "two");
    parser.add_option(value, sharg::config{.short_id = 'e'});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(value == foo::bar::two);

    parser = get_parser("-e", "two");
    parser.add_option(value2, sharg::config{.short_id = 'e'});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(value2 == Other::bar::two);
}

TEST_F(enumeration_names_test, parse_error_enum_option)
{
    foo::bar option_value{};

    auto parser = get_parser("-e", "four");
    parser.add_option(option_value, sharg::config{.short_id = 'e'});
    EXPECT_THROW(parser.parse(), sharg::user_input_error);
}

// https://github.com/seqan/seqan3/issues/2464
TEST_F(enumeration_names_test, issue2464)
{
    foo::bar option_value{};
    std::vector<foo::bar> option_values{};

    // Using a non-existing value of foo::bar should throw.
    auto parser = get_parser("-e", "nine");
    parser.add_option(option_value, sharg::config{.short_id = 'e'});
    EXPECT_THROW(parser.parse(), sharg::user_input_error);

    parser = get_parser("-e", "one", "-e", "nine");
    parser.add_option(option_values, sharg::config{.short_id = 'e'});
    EXPECT_THROW(parser.parse(), sharg::user_input_error);

    // Invalid inputs for enums are handled before any validator is evaluated.
    // Thus the exception will be sharg::user_input_error and not sharg::validation_error.
    sharg::value_list_validator enum_validator{(sharg::enumeration_names<foo::bar> | std::views::values)};

    parser = get_parser("-e", "nine");
    parser.add_option(option_value, sharg::config{.short_id = 'e', .advanced = true, .validator = enum_validator});
    EXPECT_THROW(parser.parse(), sharg::user_input_error);

    parser = get_parser("-e", "one", "-e", "nine");
    parser.add_option(option_values, sharg::config{.short_id = 'e', .advanced = true, .validator = enum_validator});
    EXPECT_THROW(parser.parse(), sharg::user_input_error);
}

TEST_F(enumeration_names_test, enum_error_message)
{
    foo::bar value{};
    Other::bar value2{};

    // foo::bar does not contain duplicate values
    auto parser = get_parser("-e", "nine");
    parser.add_option(value, sharg::config{.short_id = 'e'});
    EXPECT_THROW_MSG(parser.parse(),
                     sharg::user_input_error,
                     "You have chosen an invalid input value: nine. Please use one of: [one, two, three]");

    // Other::bar does contain duplicate values
    parser = get_parser("-e", "nine");
    parser.add_option(value2, sharg::config{.short_id = 'e'});
    EXPECT_THROW_MSG(parser.parse(),
                     sharg::user_input_error,
                     "You have chosen an invalid input value: nine. Please use one of: [1, one, 2, two]");
}

// https://github.com/seqan/seqan3/pull/2381
TEST_F(enumeration_names_test, container_options)
{
    std::vector<foo::bar> option_values{};

    auto parser = get_parser("-e", "two", "-e", "one", "-e", "three");
    parser.add_option(option_values, sharg::config{.short_id = 'e'});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(option_values == (std::vector<foo::bar>{foo::bar::two, foo::bar::one, foo::bar::three}));
}
