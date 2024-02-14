// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <sharg/parser.hpp>

TEST(design_error, app_name_validation)
{
    char const * argv[] = {"./parser_test", "-i", "3"};
    int const argc{3};
    int option_value;

    auto create_parser = [&](std::string && app_name)
    {
        sharg::parser parser{std::move(app_name), argc, argv, sharg::update_notifications::off};
        parser.add_option(option_value, sharg::config{.short_id = 'i'});
        return parser;
    };

    EXPECT_NO_THROW(create_parser("test_parser").parse());
    EXPECT_NO_THROW(create_parser("test-parser1234_foo").parse());

    EXPECT_THROW(create_parser("test parser").parse(), sharg::design_error);
    EXPECT_THROW(create_parser("test;").parse(), sharg::design_error);
    EXPECT_THROW(create_parser(";").parse(), sharg::design_error);
    EXPECT_THROW(create_parser("test;bad script:D").parse(), sharg::design_error);
}

// -----------------------------------------------------------------------------
// option config verification
// -----------------------------------------------------------------------------

TEST(verify_option_config_test, short_option_was_used_before)
{
    int option_value;

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.short_id = 'i'}), sharg::design_error);
}

TEST(verify_option_config_test, long_option_was_used_before)
{
    int option_value;

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    parser.add_option(option_value, sharg::config{.long_id = "int"});
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.long_id = "int"}), sharg::design_error);
}

TEST(verify_option_config_test, short_and_long_id_empty)
{
    int option_value;

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    EXPECT_THROW(parser.add_option(option_value, sharg::config{}), sharg::design_error);
}

TEST(verify_option_config_test, special_identifiers)
{
    int option_value;

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.short_id = 'h'}), sharg::design_error);
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.long_id = "help"}), sharg::design_error);
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.long_id = "advanced-help"}), sharg::design_error);
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.long_id = "export-help"}), sharg::design_error);
}

TEST(verify_option_config_test, single_character_long_id)
{
    int option_value;

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.long_id = "z"}), sharg::design_error);
}

TEST(verify_option_config_test, non_printable_characters)
{
    int option_value;

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.short_id = '\t'}), sharg::design_error);
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.long_id = "no\n"}), sharg::design_error);
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.long_id = "-no"}), sharg::design_error);
}

// -----------------------------------------------------------------------------
// flag config verification
// -----------------------------------------------------------------------------

TEST(verify_flag_config_test, default_value_is_true)
{
    bool true_value{true};

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    EXPECT_THROW(parser.add_flag(true_value, sharg::config{.short_id = 'i'}), sharg::design_error);
}

TEST(verify_flag_config_test, short_option_was_used_before)
{
    bool flag_value{false};

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    parser.add_flag(flag_value, sharg::config{.short_id = 'i'});
    EXPECT_THROW(parser.add_flag(flag_value, sharg::config{.short_id = 'i'}), sharg::design_error);
}

TEST(verify_flag_config_test, long_option_was_used_before)
{
    bool flag_value{false};

    char const * argv[] = {"./parser_test"};
    sharg::parser parser6{"test_parser", 1, argv};
    parser6.add_flag(flag_value, sharg::config{.long_id = "int"});
    EXPECT_THROW(parser6.add_flag(flag_value, sharg::config{.long_id = "int"}), sharg::design_error);
}

TEST(verify_flag_config_test, short_and_long_id_empty)
{
    bool flag_value{false};

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    EXPECT_THROW(parser.add_flag(flag_value, sharg::config{}), sharg::design_error);
}

TEST(verify_flag_config_test, single_character_long_id)
{
    bool flag_value;

    char const * argv[] = {"./parser_test"};
    sharg::parser parser10{"test_parser", 1, argv};
    EXPECT_THROW(parser10.add_flag(flag_value, sharg::config{.long_id = "z"}), sharg::design_error);
}

// -----------------------------------------------------------------------------
// flag config verification
// -----------------------------------------------------------------------------

TEST(verify_positional_option_config_test, list_option_not_the_very_last_option)
{
    int option_value;
    std::vector<int> vec;

    char const * argv[] = {"./parser_test", "arg1", "arg2", "arg3"};
    sharg::parser parser{"test_parser", 4, argv};
    parser.add_positional_option(vec, sharg::config{});
    EXPECT_THROW(parser.add_positional_option(option_value, sharg::config{}), sharg::design_error);
}

TEST(verify_positional_option_config_test, short_id_set)
{
    int option_value;

    char const * argv[] = {"./parser_test", "arg1"};
    sharg::parser parser{"test_parser", 2, argv};
    EXPECT_THROW(parser.add_positional_option(option_value, sharg::config{.short_id = 'a'}), sharg::design_error);
}

TEST(verify_positional_option_config_test, long_id_set)
{
    int option_value;

    char const * argv[] = {"./parser_test", "arg1"};
    sharg::parser parser{"test_parser", 2, argv};
    EXPECT_THROW(parser.add_positional_option(option_value, sharg::config{.long_id = "abc"}), sharg::design_error);
}

TEST(verify_positional_option_config_test, advanced_config_set)
{
    int option_value;

    char const * argv[] = {"./parser_test", "arg1"};
    sharg::parser parser{"test_parser", 2, argv};
    EXPECT_THROW(parser.add_positional_option(option_value, sharg::config{.advanced = true}), sharg::design_error);
}

TEST(verify_positional_option_config_test, hidden_config_set)
{
    int option_value;

    char const * argv[] = {"./parser_test", "arg1"};
    sharg::parser parser{"test_parser", 2, argv};
    EXPECT_THROW(parser.add_positional_option(option_value, sharg::config{.hidden = true}), sharg::design_error);
}

// -----------------------------------------------------------------------------
// default_message config verification
// -----------------------------------------------------------------------------

TEST(verify_default_message_config_test, required_option_set)
{
    int option_value;

    char const * argv[] = {"./parser_test", "arg1"};
    sharg::parser parser{"test_parser", 2, argv};
    EXPECT_THROW(parser.add_option(option_value,
                                   sharg::config{.long_id = "int", .default_message = "Some number", .required = true}),
                 sharg::design_error);
}

TEST(verify_default_message_config_test, positional_option_set)
{
    int option_value;

    char const * argv[] = {"./parser_test", "arg1"};
    sharg::parser parser{"test_parser", 2, argv};
    EXPECT_THROW(parser.add_positional_option(option_value, sharg::config{.default_message = "Some number"}),
                 sharg::design_error);
}

TEST(verify_default_message_config_test, flag_set)
{
    bool value{};

    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    EXPECT_THROW(parser.add_flag(value, sharg::config{.short_id = 'i', .default_message = "false"}),
                 sharg::design_error);
}

// -----------------------------------------------------------------------------
// general
// -----------------------------------------------------------------------------

TEST(parse_test, parse_called_twice)
{
    std::string option_value;

    char const * argv[] = {"./parser_test", "--version-check", "false", "-s", "option_string"};
    sharg::parser parser{"test_parser", 5, argv};
    parser.add_option(option_value, sharg::config{.short_id = 's'});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "option_string");

    EXPECT_THROW(parser.parse(), sharg::design_error);
}

TEST(parse_test, subcommand_parser_error)
{
    bool flag_value{false};

    // subcommand parsing was not enabled on construction but get_sub_parser() is called
    {
        char const * argv[]{"./top_level", "-f"};
        sharg::parser top_level_parser{"top_level", 2, argv, sharg::update_notifications::off};
        top_level_parser.add_flag(flag_value, sharg::config{.short_id = 'f'});

        EXPECT_NO_THROW(top_level_parser.parse());
        EXPECT_EQ(true, flag_value);

        EXPECT_THROW(top_level_parser.get_sub_parser(), sharg::design_error);
    }

    // subcommand key word must only contain alpha numeric characters
    {
        char const * argv[]{"./top_level", "-f"};
        EXPECT_THROW((sharg::parser{"top_level", 2, argv, sharg::update_notifications::off, {"with space"}}),
                     sharg::design_error);
    }

    // no positional/options are allowed
    {
        char const * argv[]{"./top_level", "foo"};
        sharg::parser top_level_parser{"top_level", 2, argv, sharg::update_notifications::off, {"foo"}};

        EXPECT_THROW((top_level_parser.add_option(flag_value, sharg::config{.short_id = 'f'})), sharg::design_error);
        EXPECT_THROW((top_level_parser.add_positional_option(flag_value, sharg::config{})), sharg::design_error);
    }
}

TEST(parse_test, not_allowed_after_parse)
{
    int32_t value{};
    bool flag{};

    std::vector<std::string> const arguments{"./parser_test", "-i", "3"};
    sharg::parser parser{"test_parser", arguments};
    parser.add_option(value, sharg::config{.short_id = 'i'});
    EXPECT_NO_THROW(parser.parse());

    auto check_error = [](auto call_fn, std::string const function_name)
    {
        try
        {
            call_fn();
            FAIL();
        }
        catch (sharg::design_error const & exception)
        {
            EXPECT_EQ(function_name + " may only be used before calling parse().", exception.what());
        }
        catch (...)
        {
            FAIL();
        }
    };

    // clang-format off
    check_error([&parser, &value]() { parser.add_option(value, sharg::config{.short_id = 'i'}); }, "add_option");
    check_error([&parser, &flag]() { parser.add_flag(flag, sharg::config{.short_id = 'i'}); }, "add_flag");
    check_error([&parser, &value]() { parser.add_positional_option(value, sharg::config{}); }, "add_positional_option");
    check_error([&parser]() { parser.add_section(""); }, "add_section");
    check_error([&parser]() { parser.add_subsection(""); }, "add_subsection");
    check_error([&parser]() { parser.add_line(""); }, "add_line");
    check_error([&parser]() { parser.add_list_item("", ""); }, "add_list_item");
    // clang-format on
}
