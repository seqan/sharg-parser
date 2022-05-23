// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <sharg/parser.hpp>

TEST(design_error, app_name_validation)
{
    char const * argv[] = {"./parser_test"};

    EXPECT_NO_THROW((sharg::parser{"test_parser", 1, argv}));
    EXPECT_NO_THROW((sharg::parser{"test-parser1234_foo", 1, argv}));

    EXPECT_THROW((sharg::parser{"test parser", 1, argv}), sharg::design_error);
    EXPECT_THROW((sharg::parser{"test;", 1, argv}), sharg::design_error);
    EXPECT_THROW((sharg::parser{";", 1, argv}), sharg::design_error);
    EXPECT_THROW((sharg::parser{"test;bad script:D", 1, argv}), sharg::design_error);
}

TEST(parse_test, design_error)
{
    int option_value;

    // short option
    char const * argv[] = {"./parser_test"};
    sharg::parser parser{"test_parser", 1, argv};
    parser.add_option(option_value, sharg::config{.short_id = 'i'});
    EXPECT_THROW(parser.add_option(option_value, sharg::config{.short_id = 'i'}), sharg::design_error);

    // long option
    sharg::parser parser2{"test_parser", 1, argv};
    parser2.add_option(option_value, sharg::config{.long_id = "int"});
    EXPECT_THROW(parser2.add_option(option_value, sharg::config{.long_id = "int"}), sharg::design_error);

    // empty long and short identifier
    sharg::parser parser3{"test_parser", 1, argv};
    EXPECT_THROW(parser3.add_option(option_value, sharg::config{}), sharg::design_error);

    bool true_value{true};

    // default true
    sharg::parser parser4{"test_parser", 1, argv};
    EXPECT_THROW(parser4.add_flag(true_value, sharg::config{.short_id = 'i'}), sharg::design_error);

    bool flag_value{false};

    // short flag
    sharg::parser parser5{"test_parser", 1, argv};
    parser5.add_flag(flag_value, sharg::config{.short_id = 'i'});
    EXPECT_THROW(parser5.add_flag(flag_value, sharg::config{.short_id = 'i'}), sharg::design_error);

    // long flag
    sharg::parser parser6{"test_parser", 1, argv};
    parser6.add_flag(flag_value, sharg::config{.long_id = "int"});
    EXPECT_THROW(parser6.add_flag(flag_value, sharg::config{.long_id = "int"}), sharg::design_error);

    // empty short and long identifier
    sharg::parser parser7{"test_parser", 1, argv};
    EXPECT_THROW(parser7.add_flag(flag_value, sharg::config{}), sharg::design_error);

    // positional option not at the end
    char const * argv2[] = {"./parser_test", "arg1", "arg2", "arg3"};
    std::vector<int> vec;
    sharg::parser parser8{"test_parser", 4, argv2};
    parser8.add_positional_option(vec, sharg::config{});
    EXPECT_THROW(parser8.add_positional_option(option_value, sharg::config{}), sharg::design_error);

    // using h, help, advanced-help, and export-help
    sharg::parser parser9{"test_parser", 1, argv};
    EXPECT_THROW(parser9.add_option(option_value, sharg::config{.short_id = 'h'}), sharg::design_error);
    EXPECT_THROW(parser9.add_option(option_value, sharg::config{.long_id = "help"}), sharg::design_error);
    EXPECT_THROW(parser9.add_option(option_value, sharg::config{.long_id = "advanced-help"}), sharg::design_error);
    EXPECT_THROW(parser9.add_option(option_value, sharg::config{.long_id = "export-help"}), sharg::design_error);

    // using one-letter long identifiers.
    sharg::parser parser10{"test_parser", 1, argv};
    EXPECT_THROW(parser10.add_option(option_value, sharg::config{.long_id = "z"}), sharg::design_error);
    EXPECT_THROW(parser10.add_flag(flag_value, sharg::config{.long_id = "z"}), sharg::design_error);

    // using non-printable characters
    sharg::parser parser11{"test_parser", 1, argv};
    EXPECT_THROW(parser11.add_option(option_value, sharg::config{.short_id = '\t'}), sharg::design_error);
    EXPECT_THROW(parser11.add_flag(flag_value, sharg::config{.long_id = "no\n"}), sharg::design_error);
    EXPECT_THROW(parser11.add_flag(flag_value, sharg::config{.long_id = "-no"}), sharg::design_error);
}

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
