// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>

namespace sharg::detail
{
struct test_accessor
{
    static auto & executable_name(sharg::parser & parser)
    {
        return parser.executable_name;
    }
};
} // namespace sharg::detail

TEST(parse_type_test, add_option_short_id)
{
    std::string option_value;

    char const * argv[] = {"./parser_test", "-s", "option_string"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_section("My options");       // no-op for code coverage
    parser.add_subsection("My suboptions"); // no-op for code coverage
    parser.add_line("line");                // no-op for code coverage
    parser.add_list_item("list", "item");   // no-op for code coverage
    parser.add_option(option_value, sharg::config{.short_id = 's'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "option_string");

    // add with no space
    char const * argv2[] = {"./parser_test", "-Soption_string"};
    sharg::parser parser2{"test_parser", 2, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.short_id = 'S'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "option_string");

    // ad with = sign
    char const * argv3[] = {"./parser_test", "-s=option_string"};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};
    parser3.add_option(option_value, sharg::config{.short_id = 's'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser3.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "option_string");
}

TEST(parse_type_test, add_option_long_id)
{
    std::string option_value;

    char const * argv[] = {"./parser_test", "--string-option", "option_string"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.long_id = "string-option"});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "option_string");

    char const * argv3[] = {"./parser_test", "--string-option=option_string"};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};
    parser3.add_option(option_value, sharg::config{.long_id = "string-option"});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser3.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "option_string");
}

TEST(parse_type_test, add_flag_short_id_single)
{
    bool option_value1{false};
    bool option_value2{false};

    char const * argv[] = {"./parser_test", "-f"};
    sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
    parser.add_flag(option_value1, sharg::config{.short_id = 'f'});
    parser.add_flag(option_value2, sharg::config{.short_id = 'a'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value1, true);
    EXPECT_EQ(option_value2, false);
}

TEST(parse_type_test, add_flag_short_id_multiple)
{
    bool option_value1{false};
    bool option_value2{false};
    bool option_value3{false};
    bool option_value4{false};

    char const * argv[] = {"./parser_test", "-fbc"};
    sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
    parser.add_flag(option_value1, sharg::config{.short_id = 'f'});
    parser.add_flag(option_value2, sharg::config{.short_id = 'a'});
    parser.add_flag(option_value3, sharg::config{.short_id = 'b'});
    parser.add_flag(option_value4, sharg::config{.short_id = 'c'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value1, true);
    EXPECT_EQ(option_value2, false);
    EXPECT_EQ(option_value3, true);
    EXPECT_EQ(option_value4, true);
}

TEST(parse_type_test, add_flag_long_id)
{
    bool option_value1{false};
    bool option_value2{false};

    char const * argv[] = {"./parser_test", "--another-flag"};
    sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
    parser.add_flag(option_value1, sharg::config{.long_id = "flag"});
    parser.add_flag(option_value2, sharg::config{.long_id = "another-flag"});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value1, false);
    EXPECT_EQ(option_value2, true);
}

TEST(parse_type_test, add_positional_option)
{
    std::string positional_value;

    char const * argv[] = {"./parser_test", "positional_string"};
    sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
    parser.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "positional_string");
}

TEST(parse_type_test, independent_add_order)
{
    // testing same command line input different add_* order

    std::string positional_value;
    bool flag_value{false};
    int option_value;

    // Order 1: option, flag, positional
    char const * argv[] = {"./parser_test", "-i", "2", "-b", "arg"};
    sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});
    parser.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false; // reinstate to default value

    // Order 1: flag, option, positional
    sharg::parser parser2{"test_parser", 5, argv, sharg::update_notifications::off};
    parser2.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser2.add_option(option_value, sharg::config{.short_id = 'i'});
    parser2.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: option, positional, flag
    sharg::parser parser3{"test_parser", 5, argv, sharg::update_notifications::off};
    parser3.add_option(option_value, sharg::config{.short_id = 'i'});
    parser3.add_positional_option(positional_value, sharg::config{});
    parser3.add_flag(flag_value, sharg::config{.short_id = 'b'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser3.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: flag, positional, option
    sharg::parser parser4{"test_parser", 5, argv, sharg::update_notifications::off};
    parser4.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser4.add_positional_option(positional_value, sharg::config{});
    parser4.add_option(option_value, sharg::config{.short_id = 'i'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser4.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: positional, flag, option
    sharg::parser parser5{"test_parser", 5, argv, sharg::update_notifications::off};
    parser5.add_positional_option(positional_value, sharg::config{});
    parser5.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser5.add_option(option_value, sharg::config{.short_id = 'i'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser5.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: positional, option, flag
    sharg::parser parser6{"test_parser", 5, argv, sharg::update_notifications::off};
    parser6.add_positional_option(positional_value, sharg::config{});
    parser6.add_option(option_value, sharg::config{.short_id = 'i'});
    parser6.add_flag(flag_value, sharg::config{.short_id = 'b'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser6.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);
}

TEST(parse_type_test, independent_cmd_order)
{
    // testing different command line order

    std::string positional_value;
    bool flag_value{false};
    int option_value;

    // Order 1: option, flag, positional (POSIX conform)
    char const * argv[] = {"./parser_test", "-i", "2", "-b", "arg"};
    sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});
    parser.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false; // reinstate to default value

    // Order 1: flag, option, positional (POSIX conform)
    char const * argv2[] = {"./parser_test", "-b", "-i", "2", "arg"};
    sharg::parser parser2{"test_parser", 5, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.short_id = 'i'});
    parser2.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser2.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: option, positional, flag
    char const * argv3[] = {"./parser_test", "-i", "2", "arg", "-b"};
    sharg::parser parser3{"test_parser", 5, argv3, sharg::update_notifications::off};
    parser3.add_option(option_value, sharg::config{.short_id = 'i'});
    parser3.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser3.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser3.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: flag, positional, option
    char const * argv4[] = {"./parser_test", "-b", "arg", "-i", "2"};
    sharg::parser parser4{"test_parser", 5, argv4, sharg::update_notifications::off};
    parser4.add_option(option_value, sharg::config{.short_id = 'i'});
    parser4.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser4.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser4.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: positional, flag, option
    char const * argv5[] = {"./parser_test", "arg", "-b", "-i", "2"};
    sharg::parser parser5{"test_parser", 5, argv5, sharg::update_notifications::off};
    parser5.add_option(option_value, sharg::config{.short_id = 'i'});
    parser5.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser5.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser5.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);

    flag_value = false;

    // Order 1: positional, option, flag
    char const * argv6[] = {"./parser_test", "arg", "-i", "2", "-b"};
    sharg::parser parser6{"test_parser", 5, argv6, sharg::update_notifications::off};
    parser6.add_option(option_value, sharg::config{.short_id = 'i'});
    parser6.add_flag(flag_value, sharg::config{.short_id = 'b'});
    parser6.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser6.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(positional_value, "arg");
    EXPECT_EQ(option_value, 2);
    EXPECT_EQ(flag_value, true);
}

TEST(parse_test, double_dash_separation_success)
{
    std::string option_value;

    // string option with dash
    char const * argv[] = {"./parser_test", "--", "-strange"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_positional_option(option_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "-strange");

    // negative integer option
    int option_value_int;
    char const * argv2[] = {"./parser_test", "--", "-120"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    parser2.add_positional_option(option_value_int, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value_int, -120);
}

TEST(parse_test, special_characters_as_value_success)
{
    std::string option_value;

    // weird option value. But since r/regex option is parsed and with it's
    // value should work correct
    char const * argv[] = {"./parser_test", "--regex", "-i=/45*&//--"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.long_id = "regex"});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "-i=/45*&//--");
}

TEST(parse_test, empty_value_error)
{
    int option_value;

    // short option
    char const * argv[] = {"./parser_test", "-i"};
    sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser.parse(), sharg::parser_error);

    // long option
    char const * argv2[] = {"./parser_test", "--long"};
    sharg::parser parser2{"test_parser", 2, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.long_id = "long"});

    EXPECT_THROW(parser2.parse(), sharg::parser_error);

    // short option
    char const * argv3[] = {"./parser_test", "-i="};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};
    parser3.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser3.parse(), sharg::parser_error);

    // short option
    char const * argv4[] = {"./parser_test", "--long="};
    sharg::parser parser4{"test_parser", 2, argv4, sharg::update_notifications::off};
    parser4.add_option(option_value, sharg::config{.long_id = "long"});

    EXPECT_THROW(parser4.parse(), sharg::parser_error);
}

TEST(parse_type_test, parse_success_bool_option)
{
    bool option_value;
    bool positional_value;

    // numbers 0 and 1
    {
        char const * argv[] = {"./parser_test", "-b", "1", "0"};
        sharg::parser parser{"test_parser", 4, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'b'});
        parser.add_positional_option(positional_value, sharg::config{});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_value, true);
        EXPECT_EQ(positional_value, false);
    }

    // true and false
    {
        char const * argv[] = {"./parser_test", "-b", "true", "false"};
        sharg::parser parser{"test_parser", 4, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'b'});
        parser.add_positional_option(positional_value, sharg::config{});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_value, true);
        EXPECT_EQ(positional_value, false);
    }
}

TEST(parse_type_test, parse_success_int_option)
{
    int option_value;
    size_t positional_value;

    char const * argv[] = {"./parser_test", "-i", "-2", "278"};
    sharg::parser parser{"test_parser", 4, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});
    parser.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());

    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, -2);
    EXPECT_EQ(positional_value, 278u);
}

TEST(parse_type_test, parse_success_double_option)
{
    double option_value;
    double positional_value;

    char const * argv[] = {"./parser_test", "-d", "12.457", "0.123"};
    sharg::parser parser{"test_parser", 4, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'd'});
    parser.add_positional_option(positional_value, sharg::config{});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());

    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_FLOAT_EQ(option_value, 12.457);
    EXPECT_FLOAT_EQ(positional_value, 0.123);

    // double expression with e
    char const * argv2[] = {"./parser_test", "-d", "6.0221418e23"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.short_id = 'd'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());

    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_FLOAT_EQ(option_value, 6.0221418e23);
    EXPECT_FLOAT_EQ(positional_value, 0.123);
}

TEST(parse_type_test, parse_error_bool_option)
{
    bool option_value;

    // fail on character input
    char const * argv[] = {"./parser_test", "-b", "a"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'b'});

    EXPECT_THROW(parser.parse(), sharg::parser_error);

    // fail on number input expect 0 and 1
    char const * argv2[] = {"./parser_test", "-b", "124"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.short_id = 'b'});

    EXPECT_THROW(parser2.parse(), sharg::parser_error);
}

TEST(parse_type_test, parse_error_int_option)
{
    int option_value;

    // fail on character
    char const * argv[] = {"./parser_test", "-i", "abc"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser.parse(), sharg::parser_error);

    // fail on number followed by character
    char const * argv2[] = {"./parser_test", "-i", "2abc"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser2.parse(), sharg::parser_error);

    // fail on double
    char const * argv3[] = {"./parser_test", "-i", "3.12"};
    sharg::parser parser3{"test_parser", 3, argv3, sharg::update_notifications::off};
    parser3.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser3.parse(), sharg::parser_error);

    // fail on negative number for unsigned
    unsigned option_value_u;
    char const * argv4[] = {"./parser_test", "-i", "-1"};
    sharg::parser parser4{"test_parser", 3, argv4, sharg::update_notifications::off};
    parser4.add_option(option_value_u, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser4.parse(), sharg::parser_error);

    // fail on overflow
    int8_t option_value_int8;
    char const * argv5[] = {"./parser_test", "-i", "129"};
    sharg::parser parser5{"test_parser", 3, argv5, sharg::update_notifications::off};
    parser5.add_option(option_value_int8, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser5.parse(), sharg::parser_error);

    uint8_t option_value_uint8;
    char const * argv6[] = {"./parser_test", "-i", "267"};
    sharg::parser parser6{"test_parser", 3, argv6, sharg::update_notifications::off};
    parser6.add_option(option_value_uint8, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser6.parse(), sharg::parser_error);
}

TEST(parse_type_test, parse_error_double_option)
{
    double option_value;

    // fail on character
    char const * argv[] = {"./parser_test", "-i", "abc"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'd'});

    EXPECT_THROW(parser.parse(), sharg::parser_error);

    // fail on number followed by character
    char const * argv2[] = {"./parser_test", "-d", "12.457a"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.short_id = 'd'});

    EXPECT_THROW(parser2.parse(), sharg::parser_error);
}

TEST(parse_test, too_many_arguments_error)
{
    int option_value;

    char const * argv[] = {"./parser_test", "5", "15"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_positional_option(option_value, sharg::config{});

    EXPECT_THROW(parser.parse(), sharg::too_many_arguments);

    // since -- indicates -i as a positional argument, this causes a too many args error
    char const * argv2[] = {"./parser_test", "2", "--", "-i"};
    sharg::parser parser2{"test_parser", 4, argv2, sharg::update_notifications::off};
    parser2.add_positional_option(option_value, sharg::config{});
    parser2.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser2.parse(), sharg::too_many_arguments);
}

TEST(parse_test, too_few_arguments_error)
{
    int option_value;

    char const * argv[] = {"./parser_test", "15"};
    sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
    parser.add_positional_option(option_value, sharg::config{});
    parser.add_positional_option(option_value, sharg::config{});

    EXPECT_THROW(parser.parse(), sharg::too_few_arguments);

    // since -- indicates -i as a positional argument, this causes a too many args error
    char const * argv2[] = {"./parser_test", "-i", "2"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    parser2.add_positional_option(option_value, sharg::config{});
    parser2.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser2.parse(), sharg::too_few_arguments);
}

TEST(parse_test, unknown_option_error)
{
    // unknown short option
    char const * argv[] = {"./parser_test", "-i", "15"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};

    EXPECT_THROW(parser.parse(), sharg::unknown_option);

    // unknown long option
    char const * argv2[] = {"./parser_test", "--arg", "8"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};

    EXPECT_THROW(parser2.parse(), sharg::unknown_option);

    // unknown short flag
    char const * argv3[] = {"./parser_test", "-a"};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};

    EXPECT_THROW(parser3.parse(), sharg::unknown_option);

    // unknown long flag
    char const * argv4[] = {"./parser_test", "--arg"};
    sharg::parser parser4{"test_parser", 2, argv4, sharg::update_notifications::off};

    EXPECT_THROW(parser4.parse(), sharg::unknown_option);

    // negative numbers are seen as options
    char const * argv5[] = {"./parser_test", "-5"};
    sharg::parser parser5{"test_parser", 2, argv5, sharg::update_notifications::off};

    EXPECT_THROW(parser5.parse(), sharg::unknown_option);

    // unknown short option in more complex command line
    int option_value_i;
    std::string option_value_a;
    std::string positional_option;
    char const * argv6[] = {"./parser_test", "-i", "129", "arg1", "-b", "bcd", "-a", "abc"};
    sharg::parser parser6{"test_parser", 8, argv6, sharg::update_notifications::off};
    parser6.add_option(option_value_i, sharg::config{.short_id = 'i'});
    parser6.add_option(option_value_a, sharg::config{.short_id = 'a'});
    parser6.add_positional_option(positional_option, sharg::config{});

    EXPECT_THROW(parser6.parse(), sharg::unknown_option);
}

TEST(parse_test, option_declared_multiple_times_error)
{
    int option_value;

    // short option
    char const * argv[] = {"./parser_test", "-i", "15", "-i", "3"};
    sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});

    EXPECT_THROW(parser.parse(), sharg::option_declared_multiple_times);

    // since -- indicates -i as a positional argument, this causes a too many args error
    char const * argv2[] = {"./parser_test", "--long", "5", "--long", "6"};
    sharg::parser parser2{"test_parser", 5, argv2, sharg::update_notifications::off};
    parser2.add_option(option_value, sharg::config{.long_id = "long"});

    EXPECT_THROW(parser2.parse(), sharg::option_declared_multiple_times);

    // since -- indicates -i as a positional argument, this causes a too many args error
    char const * argv3[] = {"./parser_test", "-i", "5", "--long", "6"};
    sharg::parser parser3{"test_parser", 5, argv3, sharg::update_notifications::off};
    parser3.add_option(option_value, sharg::config{.short_id = 'i', .long_id = "long"});

    EXPECT_THROW(parser3.parse(), sharg::option_declared_multiple_times);
}

TEST(parse_test, required_option_missing)
{
    int option_value;

    // option is required
    char const * argv[] = {"./parser_test", "5", "-i", "15"};
    sharg::parser parser{"test_parser", 4, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});
    parser.add_option(option_value, sharg::config{.short_id = 'a', .required = true});
    parser.add_positional_option(option_value, sharg::config{});

    EXPECT_THROW(parser.parse(), sharg::required_option_missing);
}

TEST(parse_test, argv_const_combinations)
{
    bool flag_value{false};

    char arg1[]{"./parser"};
    char arg2[]{"-f"};
    char * argv[] = {arg1, arg2};

    // all const*
    char const * const * const argv_all_const{argv};
    sharg::parser parser{"test_parser", 2, argv_all_const, sharg::update_notifications::off};
    parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(flag_value);

    // none const
    flag_value = false;
    parser = sharg::parser{"test_parser", 2, argv, sharg::update_notifications::off};
    parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(flag_value);

    // const 1
    flag_value = false;
    char const * argv_const1[] = {"./parser_test", "-f"};
    parser = sharg::parser{"test_parser", 2, argv_const1, sharg::update_notifications::off};
    parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(flag_value);

    // const 2
    flag_value = false;
    char * const argv_const2[] = {arg1, arg2};
    parser = sharg::parser{"test_parser", 2, argv_const2, sharg::update_notifications::off};
    parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(flag_value);

    // const 12
    flag_value = false;
    char const * const argv_const12[] = {arg1, arg2};
    parser = sharg::parser{"test_parser", 2, argv_const12, sharg::update_notifications::off};
    parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(flag_value);
}

TEST(parse_test, multiple_empty_options)
{
    int option_value;

    {
        char const * argv[]{"./empty_long", "-s=1"};
        sharg::parser parser{"empty_long", 2, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'i'});
        parser.add_option(option_value, sharg::config{.short_id = 's'});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(1, option_value);
    }

    {
        char const * argv[]{"./empty_long", "-s=1", "--unknown"};
        sharg::parser parser{"empty_long", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'i'});
        parser.add_option(option_value, sharg::config{.short_id = 's'});

        EXPECT_THROW(parser.parse(), sharg::unknown_option);
    }

    {
        char const * argv[]{"./empty_short", "--long=2"};
        sharg::parser parser{"empty_short", 2, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.long_id = "longi"});
        parser.add_option(option_value, sharg::config{.long_id = "long"});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(2, option_value);
    }
}

TEST(parse_test, version_check_option_error)
{
    { // version-check must be followed by a value
        char const * argv[] = {"./parser_test", "--version-check"};
        EXPECT_THROW((sharg::parser{"test_parser", 2, argv}), sharg::parser_error);
    }

    { // version-check value must be 0 or 1
        char const * argv[] = {"./parser_test", "--version-check", "foo"};
        EXPECT_THROW((sharg::parser{"test_parser", 3, argv}), sharg::parser_error);
    }
}

TEST(parse_test, subcommand_parser_success)
{
    bool flag_value{false};
    std::string option_value{};

    // parsing
    {
        char const * argv[]{"./top_level", "-f", "sub1", "foo"};
        sharg::parser top_level_parser{"top_level", 4, argv, sharg::update_notifications::off, {"sub1", "sub2"}};
        top_level_parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

        EXPECT_NO_THROW(top_level_parser.parse());
        EXPECT_EQ(true, flag_value);

        sharg::parser & sub_parser = top_level_parser.get_sub_parser();

        EXPECT_EQ(sub_parser.info.app_name, "top_level-sub1");

        sub_parser.add_positional_option(option_value, sharg::config{});

        EXPECT_NO_THROW(sub_parser.parse());
        EXPECT_EQ("foo", option_value);
    }

    flag_value = false; // reinstate to default value

    // top-level help page
    {
        char const * argv[]{"./top_level", "-h", "-f", "sub1", "foo"};
        sharg::parser top_level_parser{"top_level", 5, argv, sharg::update_notifications::off, {"sub1", "sub2"}};
        top_level_parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(top_level_parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        EXPECT_FALSE(std::string{testing::internal::GetCapturedStdout()}.empty());
    }

    flag_value = false;

    // sub-parser help page
    {
        char const * argv[]{"./top_level", "-f", "sub1", "-h"};
        sharg::parser top_level_parser{"top_level", 4, argv, sharg::update_notifications::off, {"sub1", "sub2"}};
        top_level_parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

        EXPECT_NO_THROW(top_level_parser.parse());
        EXPECT_EQ(true, flag_value);

        sharg::parser & sub_parser = top_level_parser.get_sub_parser();

        EXPECT_EQ(sub_parser.info.app_name, "top_level-sub1");

        sub_parser.add_positional_option(option_value, sharg::config{});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(sub_parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        EXPECT_FALSE(std::string{testing::internal::GetCapturedStdout()}.empty());
    }

    // sub command may contain dash, see https://github.com/seqan/product_backlog/issues/234
    {
        char const * argv[]{"./top_level", "-dash"};
        EXPECT_NO_THROW((sharg::parser{"top_level", 2, argv, sharg::update_notifications::off, {"-dash"}}));
    }
}

TEST(parse_test, subcommand_parser_error)
{
    // incorrect sub command
    char const * argv[]{"./top_level", "subiddysub", "-f"};
    { // see issue https://github.com/seqan/seqan3/issues/2172
        sharg::parser top_level_parser{"top_level", 3, argv, sharg::update_notifications::off, {"sub1", "sub2"}};

        EXPECT_THROW(top_level_parser.parse(), sharg::parser_error);
    }
}

TEST(parse_test, issue1544)
{
    { // wrong separation of long value:
        std::string option_value;
        char const * argv[] = {"./parser_test", "--foohallo"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.long_id = "foo"});

        EXPECT_THROW(parser.parse(), sharg::unknown_option);
    }

    { // unknown option (`--foo-bar`) that has a prefix of a known option (`--foo`)
        std::string option_value;
        char const * argv[] = {"./parser_test", "--foo", "hallo", "--foo-bar", "ballo"};
        sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.long_id = "foo"});

        EXPECT_THROW(parser.parse(), sharg::unknown_option);
    }

    { // known option (`--foo-bar`) that has a prefix of a unknown option (`--foo`)
        std::string option_value;
        char const * argv[] = {"./parser_test", "--foo", "hallo", "--foo-bar", "ballo"};
        sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.long_id = "foo-bar"});

        EXPECT_THROW(parser.parse(), sharg::unknown_option);
    }

    { // known option (`--foo`) is a prefix of another known option (`--foo-bar`)
        std::string foo_option_value;
        std::string foobar_option_value;
        char const * argv[] = {"./parser_test", "--foo", "hallo", "--foo-bar", "ballo"};
        sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
        parser.add_option(foo_option_value, sharg::config{.long_id = "foo"});
        parser.add_option(foobar_option_value, sharg::config{.long_id = "foo-bar"});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(foo_option_value, "hallo");
        EXPECT_EQ(foobar_option_value, "ballo");
    }
}

TEST(parse_test, is_option_set)
{
    std::string option_value{};
    char const * argv[] = {"./parser_test", "-l", "hallo", "--foobar", "ballo", "--", "--loo"};
    sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.short_id = 'l', .long_id = "loo"});
    parser.add_option(option_value, sharg::config{.short_id = 'f', .long_id = "foobar"});

    EXPECT_THROW(parser.is_option_set("foo"), sharg::design_error); // you cannot call option_is_set before parse()

    EXPECT_NO_THROW(parser.parse());

    EXPECT_TRUE(parser.is_option_set('l'));
    EXPECT_TRUE(parser.is_option_set("foobar"));

    EXPECT_FALSE(parser.is_option_set('f'));
    EXPECT_FALSE(parser.is_option_set("loo")); // --loo is behind the `--` which signals the end of options!

    // errors:
    EXPECT_THROW(parser.is_option_set("l"), sharg::design_error); // short identifiers are passed as chars not strings
    EXPECT_THROW(parser.is_option_set("f"), sharg::design_error); // short identifiers are passed as chars not strings

    EXPECT_THROW(parser.is_option_set("foo"), sharg::design_error);
    EXPECT_THROW(parser.is_option_set("--"), sharg::design_error);
    EXPECT_THROW(parser.is_option_set(""), sharg::design_error);

    EXPECT_THROW(parser.is_option_set('!'), sharg::design_error);
    EXPECT_THROW(parser.is_option_set('-'), sharg::design_error);
    EXPECT_THROW(parser.is_option_set('_'), sharg::design_error);
    EXPECT_THROW(parser.is_option_set('\0'), sharg::design_error);
}

// https://github.com/seqan/seqan3/issues/2835
TEST(parse_test, error_message_parsing)
{
    char const * argv[] = {"./parser_test", "--value", "-30"};

    uint64_t option_value{};

    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    parser.add_option(option_value, sharg::config{.long_id = "value"});

    std::string expected_message{"Value parse failed for --value: Argument -30 could not be parsed as type "
                                 "unsigned 64 bit integer."};

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

// https://github.com/seqan/seqan3/pull/2381
TEST(parse_test, container_options)
{
    {
        std::vector<int> option_values{};

        char const * argv[] = {"./parser_test", "-i", "2", "-i", "1", "-i", "3"};
        sharg::parser parser{"test_parser", 7, argv, sharg::update_notifications::off};
        parser.add_option(option_values, sharg::config{.short_id = 'i'});

        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE(option_values == (std::vector<int>{2, 1, 3}));
    }

    {
        std::vector<bool> option_values{};

        char const * argv[] = {"./parser_test", "-b", "true", "-b", "false", "-b", "true"};
        sharg::parser parser{"test_parser", 7, argv, sharg::update_notifications::off};
        parser.add_option(option_values, sharg::config{.short_id = 'b'});

        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE(option_values == (std::vector<bool>{true, false, true}));
    }
}

// https://github.com/seqan/seqan3/issues/2393
TEST(parse_test, container_default)
{
    // overwrite default
    {
        std::vector<int> option_values{1, 2, 3};

        char const * argv[] = {"./parser_test", "-i", "2", "-i", "1", "-i", "3"};
        sharg::parser parser{"test_parser", 7, argv, sharg::update_notifications::off};
        parser.add_option(option_values, sharg::config{.short_id = 'i'});

        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE(option_values == (std::vector<int>{2, 1, 3}));
    }
    // overwrite default, parameters are not consecutive
    {
        std::vector<int> option_values{1, 2, 3};
        bool bool_opt{false};

        char const * argv[] = {"./parser_test", "-i", "2", "-b", "true", "-i", "1", "-i", "3"};
        sharg::parser parser{"test_parser", 9, argv, sharg::update_notifications::off};
        parser.add_option(option_values, sharg::config{.short_id = 'i'});
        parser.add_option(bool_opt, sharg::config{.short_id = 'b'});

        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE(option_values == (std::vector<int>{2, 1, 3}));
    }
    // use default
    {
        std::vector<int> option_values{1, 2, 3};
        bool bool_opt{false};

        char const * argv[] = {"./parser_test", "-b", "true"};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        parser.add_option(option_values, sharg::config{.short_id = 'i'});
        parser.add_option(bool_opt, sharg::config{.short_id = 'b'});

        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE(option_values == (std::vector<int>{1, 2, 3}));
    }
    // overwrite default for positional options
    {
        std::vector<int> option_values{1, 2, 3};

        char const * argv[] = {"./parser_test", "2", "1", "3"};
        sharg::parser parser{"test_parser", 4, argv, sharg::update_notifications::off};
        parser.add_positional_option(option_values, sharg::config{});

        EXPECT_NO_THROW(parser.parse());

        EXPECT_TRUE(option_values == (std::vector<int>{2, 1, 3}));
    }
}

TEST(parse_test, executable_name)
{
    testing::internal::CaptureStdout();
    {
        std::array argv{"parser_test"};
        sharg::parser parser{"test_parser", argv.size(), argv.data(), sharg::update_notifications::off};
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        auto & executable_name = sharg::detail::test_accessor::executable_name(parser);
        ASSERT_EQ(executable_name.size(), 1);
        EXPECT_EQ(executable_name[0], "parser_test");
    }

    {
        std::array argv{"./parser_test"};
        sharg::parser parser{"test_parser", argv.size(), argv.data(), sharg::update_notifications::off};
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        auto & executable_name = sharg::detail::test_accessor::executable_name(parser);
        ASSERT_EQ(executable_name.size(), 1);
        EXPECT_EQ(executable_name[0], "./parser_test");
    }

    {
        std::array argv{"./bin/parser_test"};
        sharg::parser parser{"test_parser", argv.size(), argv.data(), sharg::update_notifications::off};
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        auto & executable_name = sharg::detail::test_accessor::executable_name(parser);
        ASSERT_EQ(executable_name.size(), 1);
        EXPECT_EQ(executable_name[0], "./bin/parser_test");
    }

    {
        std::array argv{"./bin/parser_test", "build"};
        sharg::parser parser{"test_parser", argv.size(), argv.data(), sharg::update_notifications::off, {"build"}};
        parser.parse();
        auto & sub_parser = parser.get_sub_parser();
        EXPECT_EXIT(sub_parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        auto & executable_name = sharg::detail::test_accessor::executable_name(sub_parser);
        ASSERT_EQ(executable_name.size(), 2);
        EXPECT_EQ(executable_name[0], "./bin/parser_test");
        EXPECT_EQ(executable_name[1], "build");
    }
    testing::internal::GetCapturedStdout();
}
