// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>
#include <sharg/test/expect_throw_msg.hpp>
#include <sharg/test/test_fixture.hpp>

class subcommand_test : public sharg::test::test_fixture
{
protected:
    static constexpr std::string_view expected_top_short_help = "test_parser\n===========n"
                                                                "    Try -h or --help for more information.\n";

    static inline std::string expected_top_full_help =
        "test_parser\n"
        "===========\n"
        "\n"
        "SUBCOMMANDS\n"
        "    This program must be invoked with one of the following subcommands:\n"
        "    - build\n"
        "    See the respective help page for further details (e.g. by calling\n"
        "    test_parser build -h).\n"
        "\n"
        "    The following options belong to the top-level parser and need to be\n"
        "    specified before the subcommand key word. Every argument after the\n"
        "    subcommand key word is passed on to the corresponding sub-parser.\n"
        "\n"
        "OPTIONS\n"
        "    -o (std::string)\n"
        "          Default: \"\"\n"
        "\n"
        + basic_options_str + '\n' + version_str();

    static constexpr std::string_view expected_sub_short_help = "test_parser-build\n=================\n"
                                                                "    Try -h or --help for more information.\n";

    static inline std::string expected_sub_full_help = "test_parser-build\n"
                                                       "=================\n"
                                                       "\n"
                                                       "OPTIONS\n"
                                                       "    -o (std::string)\n"
                                                       "          Default: \"\"\n"
                                                       "\n"
                                                     + basic_options_str + '\n' + version_str("-build");

    static inline std::string value{};

    static inline void clear_and_add_option(sharg::parser & parser)
    {
        value.clear();
        parser.info.synopsis = {""};
        parser.add_option(value, sharg::config{.short_id = 'o'});
    }
};

TEST_F(subcommand_test, simple_option)
{
    auto parser = get_subcommand_parser({"build", "-o", "foo"}, {"build"});
    EXPECT_NO_THROW(parser.parse());

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_NO_THROW(sub_parser.parse());
    EXPECT_EQ(value, "foo");
}

TEST_F(subcommand_test, subcommand_is_option_value)
{
    auto parser = get_subcommand_parser({"-o", "build", "build", "-o", "build"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "build");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_NO_THROW(sub_parser.parse());
    EXPECT_EQ(value, "build");
}

TEST_F(subcommand_test, option_value_is_option)
{
    auto parser = get_subcommand_parser({"-o", "-o", "build", "-o", "-o"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "-o");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_NO_THROW(sub_parser.parse());
    EXPECT_EQ(value, "-o");
}

TEST_F(subcommand_test, wrong_subcommand)
{
    constexpr std::string_view expected_message = "You specified an unknown subcommand! Available subcommands are: "
                                                  "[build]. Use -h/--help for more information.";

    auto parser = get_subcommand_parser({"-o", "build", "buidl", "-o", "build"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_THROW_MSG(parser.parse(), sharg::user_input_error, expected_message);
    EXPECT_EQ(value, "");
}

TEST_F(subcommand_test, subcommand_is_option_value_equals_syntax)
{
    auto parser = get_subcommand_parser({"-o=build", "build", "-o=build"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "build");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_NO_THROW(sub_parser.parse());
    EXPECT_EQ(value, "build");
}

TEST_F(subcommand_test, subcommand_is_flag)
{
    std::array<bool, 5> flag_values{false, false, false, false, false};

    auto parser = get_subcommand_parser({"-build", "build", "-o", "build"}, {"build"});
    parser.add_flag(flag_values[0], sharg::config{.short_id = 'b'});
    parser.add_flag(flag_values[1], sharg::config{.short_id = 'u'});
    parser.add_flag(flag_values[2], sharg::config{.short_id = 'i'});
    parser.add_flag(flag_values[3], sharg::config{.short_id = 'l'});
    parser.add_flag(flag_values[4], sharg::config{.short_id = 'd'});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE(std::ranges::all_of(flag_values, std::identity{})); // All true

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_NO_THROW(sub_parser.parse());
    EXPECT_EQ(value, "build");
}

TEST_F(subcommand_test, no_help)
{
    auto parser = get_subcommand_parser({"-o", "build"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "build");
}

TEST_F(subcommand_test, top_full_help)
{
    auto parser = get_subcommand_parser({"-o", "build", "--help"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected_top_full_help);
    EXPECT_EQ(value, "");
}

TEST_F(subcommand_test, sub_short_help)
{
    auto parser = get_subcommand_parser({"-o", "build", "build"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "build");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_EQ(get_parse_cout_on_exit(sub_parser), expected_sub_short_help);
    EXPECT_EQ(value, "");
}

TEST_F(subcommand_test, sub_full_help)
{
    auto parser = get_subcommand_parser({"-o", "build", "build", "--help"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "build");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_EQ(get_parse_cout_on_exit(sub_parser), expected_sub_full_help);
    EXPECT_EQ(value, "");
}

TEST_F(subcommand_test, sub_short_help_no_options)
{
    auto parser = get_subcommand_parser({"build"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_EQ(get_parse_cout_on_exit(sub_parser), expected_sub_short_help);
    EXPECT_EQ(value, "");
}

TEST_F(subcommand_test, sub_full_help_no_options)
{
    auto parser = get_subcommand_parser({"build", "--help"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_EQ(get_parse_cout_on_exit(sub_parser), expected_sub_full_help);
    EXPECT_EQ(value, "");
}

TEST_F(subcommand_test, sub_full_help_all_options)
{
    auto parser = get_subcommand_parser({"-o", "build", "build", "-o", "build", "--help"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "build");

    ASSERT_NO_THROW(parser.get_sub_parser());
    auto & sub_parser = parser.get_sub_parser();
    clear_and_add_option(sub_parser);

    EXPECT_EQ(get_parse_cout_on_exit(sub_parser), expected_sub_full_help);
    EXPECT_EQ(value, "");
}

TEST_F(subcommand_test, option_value_is_special_command)
{
    auto parser = get_subcommand_parser({"-o", "--help"}, {"build"});
    clear_and_add_option(parser);
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "--help");
}

TEST_F(subcommand_test, recursive_subcommands)
{
    auto parser = get_subcommand_parser({"index", "show", "--help"}, {"index"});
    EXPECT_NO_THROW(parser.parse());

    auto & sub_parser = parser.get_sub_parser();
    ASSERT_EQ(sub_parser.info.app_name, "test_parser-index");
    sub_parser.add_subcommands({"show"});
    EXPECT_NO_THROW(sub_parser.parse());

    auto & sub_sub_parser = sub_parser.get_sub_parser();
    ASSERT_EQ(sub_sub_parser.info.app_name, "test_parser-index-show");
    clear_and_add_option(sub_sub_parser);

    std::string expected_sub_sub_full_help = "test_parser-index-show\n"
                                             "======================\n"
                                             "\n"
                                             "OPTIONS\n"
                                             "    -o (std::string)\n"
                                             "          Default: \"\"\n"
                                             "\n"
                                           + basic_options_str + '\n' + version_str("-index-show");

    EXPECT_EQ(get_parse_cout_on_exit(sub_sub_parser), expected_sub_sub_full_help);
}

TEST_F(subcommand_test, copy_meta_data)
{
    sharg::parser_meta_data info{.version = "1.0.0",
                                 .author = "SeqAn-Team",
                                 .email = "mail@example.org",
                                 .date = "1970-01-01",
                                 .url = "example.org",
                                 .short_copyright = "BSD 3-Clause",
                                 .long_copyright = "BSD 3-Clause Text",
                                 .citation = "Cite me!"};

    auto parser = get_subcommand_parser({"index", "--help"}, {"index"});
    info.app_name = parser.info.app_name;
    parser.info = info;
    EXPECT_EQ(parser.info.app_name, "test_parser");
    ASSERT_EQ(parser.info, info); // Sanity check for test setup
    EXPECT_NO_THROW(parser.parse());

    auto & sub_parser = parser.get_sub_parser();
    EXPECT_EQ(sub_parser.info.app_name, "test_parser-index");
    info.app_name = sub_parser.info.app_name;
    EXPECT_EQ(sub_parser.info, info);

    std::string expected_sub_full_help = "test_parser-index\n"
                                         "=================\n"
                                         "\n"
                                         "OPTIONS\n"
                                         "\n"
                                       + basic_options_str
                                       + "\n"
                                         "VERSION\n"
                                         "    Last update: 1970-01-01\n"
                                         "    test_parser-index version: 1.0.0\n"
                                         "    Sharg version: "
                                       + sharg::sharg_version_cstring
                                       + "\n"
                                         "\n"
                                         "URL\n"
                                         "    example.org\n"
                                         "\n"
                                         "LEGAL\n"
                                         "    test_parser-index Copyright: BSD 3-Clause\n"
                                         "    Author: SeqAn-Team\n"
                                         "    Contact: mail@example.org\n"
                                         "    SeqAn Copyright: 2006-2025 Knut Reinert, FU-Berlin; released under the\n"
                                         "    3-clause BSDL.\n"
                                         "    In your academic works please cite:\n"
                                         "    [1]   Cite me!\n"
                                         "    For full copyright and/or warranty information see --copyright.\n";

    EXPECT_EQ(get_parse_cout_on_exit(sub_parser), expected_sub_full_help);
}
