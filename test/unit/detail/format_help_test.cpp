// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>

// reused global variables
std::string std_cout;
std::string expected;
int option_value{5};
bool flag_value{false};
std::vector<std::string> pos_opt_value{};
char const * argv_without_any_options[] = {"./help_add_test"};
char const * argv_with_h[] = {"./help_add_test", "-h"};
char const * argv_with_hh[] = {"./help_add_test", "-hh"};
char const * argv_with_version[] = {"./help_add_test", "--version"};

std::string const basic_options_str = "OPTIONS\n"
                                      "\n"
                                      "  Basic options:\n"
                                      "    -h, --help\n"
                                      "          Prints the help page.\n"
                                      "    -hh, --advanced-help\n"
                                      "          Prints the help page including advanced options.\n"
                                      "    --version\n"
                                      "          Prints the version information.\n"
                                      "    --copyright\n"
                                      "          Prints the copyright/license information.\n"
                                      "    --export-help (std::string)\n"
                                      "          Export the help page information. Value must be one of [html, man].\n"
                                      "    --version-check (bool)\n"
                                      "          Whether to check for the newest app version. Default: true.\n";

std::string const basic_version_str = "VERSION\n"
                                      "    Last update:\n"
                                      "    test_parser version:\n"
                                      "    Sharg version: "
                                    + std::string{sharg::sharg_version_cstring} + "\n";

std::string license_text()
{
    std::ifstream license_file{std::string{{SHARG_TEST_LICENSE_DIR}} + "/LICENSE.md"};
    EXPECT_TRUE(license_file) << "Could not open file '" SHARG_TEST_LICENSE_DIR "/LICENSE.md'";
    std::stringstream buffer;
    buffer << license_file.rdbuf();

    std::string str = buffer.str();
    size_t license_start = str.find("```\n") + 4;
    size_t license_end = str.find("```", license_start);

    return str.substr(license_start, license_end - license_start);
}

namespace sharg::detail
{
struct test_accessor
{
    static void set_terminal_width(sharg::parser & parser, unsigned terminal_width)
    {
        std::visit(
            [terminal_width](auto & f)
            {
                if constexpr (std::is_same_v<decltype(f), sharg::detail::format_help &>)
                    f.layout = sharg::detail::format_help::console_layout_struct{terminal_width};
            },
            parser.format);
    }
};
} // namespace sharg::detail

TEST(help_page_printing, short_help)
{
    expected = "empty_options\n"
               "=============\n"
               "    ./some_binary_name synopsis\n"
               "    Try -h or --help for more information.\n";

    // Empty call with no options given. For sharg::detail::format_short_help
    // even if required options exist.

    auto check_expected_short_help = [&](auto && argv)
    {
        int const argc = sizeof(argv) / sizeof(*argv);
        sharg::parser parser{"empty_options", argc, argv};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.info.synopsis.push_back("./some_binary_name synopsis");
        int option_value{};
        parser.add_option(option_value, sharg::config{.short_id = 'i', .required = true});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std_cout = testing::internal::GetCapturedStdout();
        EXPECT_EQ(std_cout, expected);
    };

    char const * argv_with_version_check[] = {"./help_add_test", "--version-check", "0"};

    check_expected_short_help(argv_without_any_options);
    check_expected_short_help(argv_with_version_check);
}

TEST(help_page_printing, no_information)
{
    // Empty help call with -h
    sharg::parser parser1{"test_parser", 2, argv_with_h};
    sharg::detail::test_accessor::set_terminal_width(parser1, 80);
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser1.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_short_copyright)
{
    // Again, but with short copyright, long copyright, and citation.
    sharg::parser short_copy("test_parser", 2, argv_with_h);
    sharg::detail::test_accessor::set_terminal_width(short_copy, 80);
    short_copy.info.short_copyright = "short";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(short_copy.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str + "\n"
             + "LEGAL\n"
               "    test_parser Copyright: short\n"
               "    SeqAn Copyright: 2006-2022 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_long_copyright)
{
    sharg::parser long_copy("test_parser", 2, argv_with_h);
    sharg::detail::test_accessor::set_terminal_width(long_copy, 80);
    long_copy.info.long_copyright = "long";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(long_copy.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str + "\n"
             + "LEGAL\n"
               "    SeqAn Copyright: 2006-2022 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n"
               "    For full copyright and/or warranty information see --copyright.\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_citation)
{
    sharg::parser citation("test_parser", 2, argv_with_h);
    sharg::detail::test_accessor::set_terminal_width(citation, 80);
    citation.info.citation = "citation";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(citation.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str + "\n"
             + "LEGAL\n"
               "    SeqAn Copyright: 2006-2022 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n"
               "    In your academic works please cite: citation\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_author)
{
    sharg::parser author("test_parser", 2, argv_with_h);
    sharg::detail::test_accessor::set_terminal_width(author, 80);
    author.info.author = "author";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(author.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str + "\n"
             + "LEGAL\n"
               "    Author: author\n"
               "    SeqAn Copyright: 2006-2022 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, with_email)
{
    sharg::parser email("test_parser", 2, argv_with_h);
    sharg::detail::test_accessor::set_terminal_width(email, 80);
    email.info.email = "email";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(email.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str + "\n"
             + "LEGAL\n"
               "    Contact: email\n"
               "    SeqAn Copyright: 2006-2022 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, empty_advanced_help)
{
    // Empty help call with -hh
    sharg::parser parser2{"test_parser", 2, argv_with_hh};
    sharg::detail::test_accessor::set_terminal_width(parser2, 80);
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser2.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, empty_version_call)
{
    // Empty version call
    sharg::parser parser3{"test_parser", 2, argv_with_version};
    sharg::detail::test_accessor::set_terminal_width(parser3, 80);
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser3.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, version_call)
{
    // Version call with url and options.
    sharg::parser parser4{"test_parser", 2, argv_with_version};
    sharg::detail::test_accessor::set_terminal_width(parser4, 80);
    parser4.info.url = "https://seqan.de";
    parser4.add_option(option_value, sharg::config{.short_id = 'i'});
    parser4.add_flag(flag_value, sharg::config{.short_id = 'f'});
    parser4.add_positional_option(pos_opt_value, sharg::config{});
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser4.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_version_str + "\n"
             + "URL\n"
               "    https://seqan.de\n";
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, do_not_print_hidden_options)
{
    // Add an option and request help.
    sharg::parser parser5{"test_parser", 2, argv_with_h};
    sharg::detail::test_accessor::set_terminal_width(parser5, 80);
    parser5.add_option(option_value, sharg::config{.short_id = 'i', .hidden = true});
    parser5.add_flag(flag_value, sharg::config{.short_id = 'f', .hidden = true});
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser5.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, advanced_options)
{
    int32_t option_value{5};
    uint8_t another_option_value{2};
    bool flag_value{false};

    auto set_up = [&option_value, &flag_value, &another_option_value](sharg::parser & parser)
    {
        // default or required information are always displayed
        parser.add_section("default section", false);
        parser.add_subsection("default subsection", false);
        parser.add_option(
            option_value,
            sharg::config{.short_id = 'i', .long_id = "int", .description = "this is a int option.", .required = true});
        parser.add_flag(flag_value,
                        sharg::config{.short_id = 'g',
                                      .long_id = "goo",
                                      .description = "this is a flag.",
                                      .required = true}); // required has no impact here
        parser.add_list_item("-s, --some", "list item.", false);
        parser.add_line("some line.", true, false);

        // advanced information
        parser.add_section("advanced section", true);
        parser.add_subsection("advanced subsection", true);
        parser.add_option(
            another_option_value,
            sharg::config{.short_id = 'j', .long_id = "jnt", .description = "this is a int option.", .advanced = true});
        parser.add_flag(
            flag_value,
            sharg::config{.short_id = 'f', .long_id = "flag", .description = "this is a flag.", .advanced = true});
        parser.add_list_item("-s, --some", "list item.", true);
        parser.add_line("some line.", true, true);
    };

    // without -hh, only the non/advanced information are shown
    sharg::parser parser_normal_help{"test_parser", 2, argv_with_h};
    sharg::detail::test_accessor::set_terminal_width(parser_normal_help, 80);
    set_up(parser_normal_help);
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser_normal_help.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str
             + "\n"
               "DEFAULT SECTION\n"
               "\n"
               "  default subsection\n"
               "    -i, --int (signed 32 bit integer)\n"
               "          this is a int option.\n"
               "    -g, --goo\n"
               "          this is a flag.\n"
               "    -s, --some\n"
               "          list item.\n"
               "    some line.\n"
               "\n"
             + basic_version_str;
    EXPECT_EQ(std_cout, expected);

    // with -hh everything is shown
    sharg::parser parser_advanced_help{"test_parser", 2, argv_with_hh};
    sharg::detail::test_accessor::set_terminal_width(parser_advanced_help, 80);
    set_up(parser_advanced_help);
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser_advanced_help.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser\n"
               "===========\n"
               "\n"
             + basic_options_str
             + "\n"
               "DEFAULT SECTION\n"
               "\n"
               "  default subsection\n"
               "    -i, --int (signed 32 bit integer)\n"
               "          this is a int option.\n"
               "    -g, --goo\n"
               "          this is a flag.\n"
               "    -s, --some\n"
               "          list item.\n"
               "    some line.\n"
               "\n"
               "ADVANCED SECTION\n"
               "\n"
               "  advanced subsection\n"
               "    -j, --jnt (unsigned 8 bit integer)\n"
               "          this is a int option. Default: 2.\n"
               "    -f, --flag\n"
               "          this is a flag.\n"
               "    -s, --some\n"
               "          list item.\n"
               "    some line.\n"
               "\n"
             + basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

enum class foo
{
    one,
    two,
    three
};

auto enumeration_names(foo)
{
    return std::unordered_map<std::string_view, foo>{{"one", foo::one}, {"two", foo::two}, {"three", foo::three}};
}

TEST(help_page_printing, full_information)
{
    int8_t required_option{};
    int8_t non_list_optional{1};
    foo enum_option_value{};

    // Add synopsis, description, short description, positional option, option, flag, and example.
    sharg::parser parser6{"test_parser", 2, argv_with_h};
    sharg::detail::test_accessor::set_terminal_width(parser6, 80);
    parser6.info.synopsis.push_back("./some_binary_name synopsis");
    parser6.info.synopsis.push_back("./some_binary_name synopsis2");
    parser6.info.description.push_back("description");
    parser6.info.description.push_back("description2");
    parser6.info.short_description = "so short";
    parser6.add_option(option_value,
                       sharg::config{.short_id = 'i',
                                     .long_id = "int",
                                     .description = "this is a int option.",
                                     .default_message = "A number"});
    parser6.add_option(
        enum_option_value,
        sharg::config{.short_id = 'e',
                      .long_id = "enum",
                      .description = "this is an enum option.",
                      .validator = sharg::value_list_validator{sharg::enumeration_names<foo> | std::views::values}});
    parser6.add_option(required_option,
                       sharg::config{.short_id = 'r',
                                     .long_id = "required-int",
                                     .description = "this is another int option.",
                                     .required = true});
    parser6.add_section("Flags");
    parser6.add_subsection("SubFlags");
    parser6.add_line("here come all the flags");
    parser6.add_flag(flag_value, sharg::config{.short_id = 'f', .long_id = "flag", .description = "this is a flag."});
    parser6.add_positional_option(non_list_optional, sharg::config{.description = "this is not a list."});
    parser6.add_positional_option(pos_opt_value, sharg::config{.description = "this is a positional option."});
    parser6.info.examples.push_back("example");
    parser6.info.examples.push_back("example2");
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser6.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std_cout = testing::internal::GetCapturedStdout();
    expected = "test_parser - so short\n"
               "======================\n"
               "\n"
               "SYNOPSIS\n"
               "    ./some_binary_name synopsis\n"
               "    ./some_binary_name synopsis2\n"
               "\n"
               "DESCRIPTION\n"
               "    description\n"
               "\n"
               "    description2\n"
               "\n"
               "POSITIONAL ARGUMENTS\n"
               "    ARGUMENT-1 (signed 8 bit integer)\n"
               "          this is not a list.\n"
               "    ARGUMENT-2 (List of std::string)\n"
               "          this is a positional option. Default: [].\n"
               "\n"
             + basic_options_str
             + "    -i, --int (signed 32 bit integer)\n"
               "          this is a int option. Default: A number.\n"
               "    -e, --enum (foo)\n"
               "          this is an enum option. Default: one. Value must be one of [three,\n"
               "          two, one].\n"
               "    -r, --required-int (signed 8 bit integer)\n"
               "          this is another int option.\n"
               "\n"
               "FLAGS\n"
               "\n"
               "  SubFlags\n"
               "    here come all the flags\n"
               "    -f, --flag\n"
               "          this is a flag.\n"
               "\n"
               "EXAMPLES\n"
               "    example\n"
               "\n"
               "    example2\n"
               "\n"
             + basic_version_str;
    EXPECT_EQ(std_cout, expected);
}

TEST(help_page_printing, copyright)
{
    // Tests the --copyright call.
    char const * argvCopyright[] = {"./copyright", "--copyright"};
    sharg::parser copyright("myApp", 2, argvCopyright);

    std::string license_string = license_text();

    // Test --copyright with empty short and long copyright info.
    {
        testing::internal::CaptureStdout();
        EXPECT_EXIT(copyright.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std_cout = testing::internal::GetCapturedStdout();

        expected = "================================================================================\n"
                   "Copyright information for myApp:\n"
                   "--------------------------------------------------------------------------------\n"
                   "myApp copyright information not available.\n"
                   "================================================================================\n"
                   "This program contains SeqAn code licensed under the following terms:\n"
                   "--------------------------------------------------------------------------------\n"
                 + license_string;

        EXPECT_EQ(std_cout, expected);
    }

    // Test --copyright with a non-empty short copyright and an empty long copyright.
    copyright.info.short_copyright = "short copyright line 1\nshort copyright line 2";
    {
        testing::internal::CaptureStdout();
        EXPECT_EXIT(copyright.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std_cout = testing::internal::GetCapturedStdout();

        expected = "================================================================================\n"
                   "Copyright information for myApp:\n"
                   "--------------------------------------------------------------------------------\n"
                   "myApp full copyright information not available. Displaying short copyright information instead:\n"
                   "short copyright line 1\n"
                   "short copyright line 2\n"
                   "================================================================================\n"
                   "This program contains SeqAn code licensed under the following terms:\n"
                   "--------------------------------------------------------------------------------\n"
                 + license_string;

        EXPECT_EQ(std_cout, expected);
    }

    // Test --copyright with a non-empty short copyright and a non-empty long copyright.
    copyright.info.long_copyright = "long copyright line 1\nlong copyright line 2";
    {
        testing::internal::CaptureStdout();
        EXPECT_EXIT(copyright.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std_cout = testing::internal::GetCapturedStdout();

        expected = "================================================================================\n"
                   "Copyright information for myApp:\n"
                   "--------------------------------------------------------------------------------\n"
                   "long copyright line 1\n"
                   "long copyright line 2\n"
                   "================================================================================\n"
                   "This program contains SeqAn code licensed under the following terms:\n"
                   "--------------------------------------------------------------------------------\n"
                 + license_string;

        EXPECT_EQ(std_cout, expected);
    }
}

TEST(parse_test, subcommand_parser)
{
    int option_value{};
    std::string option_value2{};

    char const * argv[]{"./test_parser", "-h"};
    sharg::parser top_level_parser{"test_parser", 2, argv, sharg::update_notifications::on, {"sub1", "sub2"}};
    sharg::detail::test_accessor::set_terminal_width(top_level_parser, 80);
    top_level_parser.info.description.push_back("description");
    top_level_parser.add_option(option_value,
                                sharg::config{.short_id = 'f', .long_id = "foo", .description = "foo bar."});

    testing::internal::CaptureStdout();
    EXPECT_EXIT(top_level_parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();

    std::string expected = "test_parser\n"
                           "===========\n"
                           "\n"
                           "DESCRIPTION\n"
                           "    description\n"
                           "\n"
                           "SUBCOMMANDS\n"
                           "    This program must be invoked with one of the following subcommands:\n"
                           "    - sub1\n"
                           "    - sub2\n"
                           "    See the respective help page for further details (e.g. by calling\n"
                           "    test_parser sub1 -h).\n"
                           "\n"
                           "    The following options below belong to the top-level parser and need to be\n"
                           "    specified before the subcommand key word. Every argument after the\n"
                           "    subcommand key word is passed on to the corresponding sub-parser.\n"
                           "\n"
                         + basic_options_str
                         + "    -f, --foo (signed 32 bit integer)\n"
                           "          foo bar. Default: 0.\n"
                           "\n"
                         + basic_version_str;

    EXPECT_EQ(std_cout, expected);
}
