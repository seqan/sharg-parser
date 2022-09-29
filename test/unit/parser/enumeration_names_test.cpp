// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>

namespace foo
{
enum class bar
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
enum class bar
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

TEST(parse_type_test, parse_success_enum_option)
{
    {
        foo::bar option_value{};

        char const * argv[] = {"./parser_test", "-e", "two"};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'e'});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE(option_value == foo::bar::two);
    }

    {
        Other::bar option_value{};

        char const * argv[] = {"./parser_test", "-e", "two"};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'e'});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE(option_value == Other::bar::two);
    }
}

TEST(parse_type_test, parse_error_enum_option)
{
    foo::bar option_value{};

    char const * argv[] = {"./parser_test", "-e", "four"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'e'});

    EXPECT_THROW(parser.parse(), sharg::user_input_error);
}

// https://github.com/seqan/seqan3/issues/2464
TEST(parse_test, issue2464)
{
    using option_t = foo::bar;
    // Using a non-existing value of foo::bar should throw.
    {
        char const * argv[] = {"./parser_test", "-e", "nine"};

        option_t option_value{};

        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'e'});
        EXPECT_THROW(parser.parse(), sharg::user_input_error);
    }
    {
        char const * argv[] = {"./parser_test", "-e", "one", "-e", "nine"};

        std::vector<option_t> option_values{};

        sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
        parser.add_option(option_values, sharg::config{.short_id = 'e'});
        EXPECT_THROW(parser.parse(), sharg::user_input_error);
    }

    // Invalid inputs for enums are handled before any validator is evaluated.
    // Thus the exception will be sharg::user_input_error and not sharg::validation_error.
    {
        char const * argv[] = {"./parser_test", "-e", "nine"};

        sharg::value_list_validator enum_validator{(sharg::enumeration_names<option_t> | std::views::values)};
        option_t option_value{};

        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'e', .advanced = true, .validator = enum_validator});
        EXPECT_THROW(parser.parse(), sharg::user_input_error);
    }
    {
        char const * argv[] = {"./parser_test", "-e", "one", "-e", "nine"};

        sharg::value_list_validator enum_validator{(sharg::enumeration_names<option_t> | std::views::values)};
        std::vector<option_t> option_values{};

        sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
        parser.add_option(option_values, sharg::config{.short_id = 'e', .advanced = true, .validator = enum_validator});
        EXPECT_THROW(parser.parse(), sharg::user_input_error);
    }
}

TEST(parse_test, enum_error_message)
{
    // foo::bar does not contain duplicate values
    {
        char const * argv[] = {"./parser_test", "-e", "nine"};

        foo::bar option_value{};

        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'e'});

        std::string expected_message{"You have chosen an invalid input value: nine. "
                                     "Please use one of: [one, two, three]"};

        try
        {
            parser.parse();
            FAIL();
        }
        catch (sharg::user_input_error const & exception)
        {
            EXPECT_EQ(expected_message, exception.what());
        }
        catch (...)
        {
            FAIL();
        }
    }
    // Other::bar does contain duplicate values
    {
        char const * argv[] = {"./parser_test", "-e", "nine"};

        Other::bar option_value{};

        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'e'});

        std::string expected_message{"You have chosen an invalid input value: nine. "
                                     "Please use one of: [1, one, 2, two]"};

        try
        {
            parser.parse();
            FAIL();
        }
        catch (sharg::user_input_error const & exception)
        {
            EXPECT_EQ(expected_message, exception.what());
        }
        catch (...)
        {
            FAIL();
        }
    }
}

// https://github.com/seqan/seqan3/pull/2381
TEST(parse_test, container_options)
{
    std::vector<foo::bar> option_values{};

    char const * argv[] = {"./parser_test", "-e", "two", "-e", "one", "-e", "three"};
    sharg::parser parser{"test_parser", 7, argv, sharg::update_notifications::off};
    parser.add_option(option_values, sharg::config{.short_id = 'e'});

    EXPECT_NO_THROW(parser.parse());

    EXPECT_TRUE(option_values == (std::vector<foo::bar>{foo::bar::two, foo::bar::one, foo::bar::three}));
}
