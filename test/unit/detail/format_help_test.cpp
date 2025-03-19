// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>
#include <sharg/test/test_fixture.hpp>

// Reused global variables
class format_help_test : public sharg::test::test_fixture
{
protected:
    std::string std_cout{};
    std::string expected{};
    int option_value{5};
    bool flag_value{false};
    std::vector<std::string> pos_opt_value{};

    static inline std::string license_text = []()
    {
        std::ifstream license_file{std::string{SHARG_TEST_LICENSE_DIR} + "/LICENSE.md"};
        EXPECT_TRUE(license_file) << "Could not open file '" SHARG_TEST_LICENSE_DIR "/LICENSE.md'";
        std::stringstream buffer;
        buffer << license_file.rdbuf();

        std::string str = buffer.str();
        size_t license_start = str.find("```\n") + 4;
        size_t license_end = str.find("```", license_start);

        return str.substr(license_start, license_end - license_start);
    }();
};

TEST_F(format_help_test, short_help)
{
    expected = "test_parser\n"
               "===========\n"
               "    ./some_binary_name synopsis\n"
               "    Try -h or --help for more information.\n";

    auto parser = get_parser();

    // Empty call with no options given. For sharg::detail::format_short_help
    // even if required options exist.
    auto check_expected_short_help = [&]()
    {
        parser.info.synopsis.push_back("./some_binary_name synopsis");
        int option_value{};
        parser.add_option(option_value, sharg::config{.short_id = 'i', .required = true});

        EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
    };

    {
        SCOPED_TRACE("Trace: First test");
        check_expected_short_help();
    }

    {
        SCOPED_TRACE("Trace: Second test");
        parser = get_parser("--version-check", "0");
        check_expected_short_help();
    }
}

TEST_F(format_help_test, quote_strings)
{
    auto parser = get_parser("-h");

    std::string value1{};
    std::string value2{"Some string"};
    std::vector<std::string> container_value{"Some", "other", "string"};

    parser.add_option(value1, sharg::config{.short_id = 'a', .long_id = "string1"});
    parser.add_option(value2, sharg::config{.short_id = 'b', .long_id = "string2"});
    parser.add_option(value2, sharg::config{.short_id = 'c', .long_id = "string3", .default_message = "Quoted"});
    parser.add_option(container_value, sharg::config{.short_id = 'd', .long_id = "string4"});
    parser.add_option(container_value, sharg::config{.short_id = 'e', .long_id = "string5", .default_message = "None"});
    parser.add_positional_option(container_value, sharg::config{/* No default_message allowed. */});

    expected = "test_parser\n"
               "===========\n\n"
               "POSITIONAL ARGUMENTS\n"
               "    ARGUMENT-1 (List of std::string)\n"
               "          Default: [\"Some\", \"other\", \"string\"]\n\n"
               "OPTIONS\n"
               "    -a, --string1 (std::string)\n"
               "          Default: \"\"\n"
               "    -b, --string2 (std::string)\n"
               "          Default: \"Some string\"\n"
               "    -c, --string3 (std::string)\n"
               "          Default: \"Quoted\"\n"
               "    -d, --string4 (List of std::string)\n"
               "          Default: [\"Some\", \"other\", \"string\"]\n"
               "    -e, --string5 (List of std::string)\n"
               "          Default: None\n\n"
             + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, quote_paths)
{
    auto parser = get_parser("-h");

    std::filesystem::path value1{};
    std::filesystem::path value2{"/some/path"};
    std::vector<std::filesystem::path> container_value{"/some", "/other", "/path"};

    parser.add_option(value1, sharg::config{.short_id = 'a', .long_id = "path1"});
    parser.add_option(value2, sharg::config{.short_id = 'b', .long_id = "path2"});
    parser.add_option(value2, sharg::config{.short_id = 'c', .long_id = "path3", .default_message = "/usr/bin/"});
    parser.add_option(container_value, sharg::config{.short_id = 'd', .long_id = "path4"});
    parser.add_option(container_value, sharg::config{.short_id = 'e', .long_id = "path5", .default_message = "None"});
    parser.add_positional_option(container_value, sharg::config{/* No default_message allowed. */});

    expected = "test_parser\n"
               "===========\n\n"
               "POSITIONAL ARGUMENTS\n"
               "    ARGUMENT-1 (List of std::filesystem::path)\n"
               "          Default: [\"/some\", \"/other\", \"/path\"]\n\n"
               "OPTIONS\n"
               "    -a, --path1 (std::filesystem::path)\n"
               "          Default: \"\"\n"
               "    -b, --path2 (std::filesystem::path)\n"
               "          Default: \"/some/path\"\n"
               "    -c, --path3 (std::filesystem::path)\n"
               "          Default: \"/usr/bin/\"\n"
               "    -d, --path4 (List of std::filesystem::path)\n"
               "          Default: [\"/some\", \"/other\", \"/path\"]\n"
               "    -e, --path5 (List of std::filesystem::path)\n"
               "          Default: None\n\n"
             + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, no_information)
{
    auto parser = get_parser("-h");

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, with_short_copyright)
{
    // Short copyright, long copyright, and citation.
    auto parser = get_parser("-h");
    parser.info.short_copyright = "short";

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str() + "\n"
             + "LEGAL\n"
               "    test_parser Copyright: short\n"
               "    SeqAn Copyright: 2006-2024 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n";
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, with_long_copyright)
{
    auto parser = get_parser("-h");
    parser.info.long_copyright = "long";

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str() + "\n"
             + "LEGAL\n"
               "    SeqAn Copyright: 2006-2024 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n"
               "    For full copyright and/or warranty information see --copyright.\n";
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, with_citation)
{
    auto parser = get_parser("-h");
    parser.info.citation = "citation";

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str() + "\n"
             + "LEGAL\n"
               "    SeqAn Copyright: 2006-2024 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n"
               "    In your academic works please cite: citation\n";
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, with_author)
{
    auto parser = get_parser("-h");
    parser.info.author = "author";

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str() + "\n"
             + "LEGAL\n"
               "    Author: author\n"
               "    SeqAn Copyright: 2006-2024 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n";
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, with_email)
{
    auto parser = get_parser("-h");
    parser.info.email = "email";

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str() + "\n"
             + "LEGAL\n"
               "    Contact: email\n"
               "    SeqAn Copyright: 2006-2024 Knut Reinert, FU-Berlin; released under the\n"
               "    3-clause BSDL.\n";
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, empty_advanced_help)
{
    auto parser = get_parser("-hh");

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, empty_version_call)
{
    auto parser = get_parser("--version");

    expected = "test_parser\n"
               "===========\n"
               "\n"
             + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, version_call)
{
    // Version call with url and options.
    auto parser = get_parser("--version");
    parser.info.url = "https://seqan.de";
    parser.add_option(option_value, sharg::config{.short_id = 'i'});
    parser.add_flag(flag_value, sharg::config{.short_id = 'f'});
    parser.add_positional_option(pos_opt_value, sharg::config{});

    expected = "test_parser\n"
               "===========\n"
               "\n"
             + version_str() + "\n"
             + "URL\n"
               "    https://seqan.de\n";
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, do_not_print_hidden_options)
{
    // Add an option and request help.
    auto parser = get_parser("-h");
    parser.add_option(option_value, sharg::config{.short_id = 'i', .hidden = true});
    parser.add_flag(flag_value, sharg::config{.short_id = 'f', .hidden = true});

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n\n"
             + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, advanced_options)
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
    auto parser = get_parser("-h");
    set_up(parser);

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n"
               "\n"
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
             + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // with -hh everything is shown
    parser = get_parser("-hh");
    set_up(parser);

    expected = "test_parser\n"
               "===========\n"
               "\nOPTIONS\n"
               "\n"
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
               "          this is a int option. Default: 2\n"
               "    -f, --flag\n"
               "          this is a flag.\n"
               "    -s, --some\n"
               "          list item.\n"
               "    some line.\n"
               "\n"
             + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

enum class foo : uint8_t
{
    one,
    two,
    three
};

auto enumeration_names(foo)
{
    return std::unordered_map<std::string_view, foo>{{"one", foo::one}, {"two", foo::two}, {"three", foo::three}};
}

TEST_F(format_help_test, full_information)
{
    int8_t required_option{};
    int8_t non_list_optional{1};
    foo enum_option_value{};

    // Add synopsis, description, short description, positional option, option, flag, and example.
    auto parser = get_parser("-h");
    parser.info.synopsis.push_back("./some_binary_name synopsis");
    parser.info.synopsis.push_back("./some_binary_name synopsis2");
    parser.info.description.push_back("description");
    parser.info.description.push_back("description2");
    parser.info.short_description = "so short";
    parser.add_option(option_value,
                      sharg::config{.short_id = 'i',
                                    .long_id = "int",
                                    .description = "this is a int option.",
                                    .default_message = "A number"});
    parser.add_option(
        enum_option_value,
        sharg::config{.short_id = 'e',
                      .long_id = "enum",
                      .description = "this is an enum option.",
                      .validator = sharg::value_list_validator{sharg::enumeration_names<foo> | std::views::values}});
    parser.add_option(required_option,
                      sharg::config{.short_id = 'r',
                                    .long_id = "required-int",
                                    .description = "this is another int option.",
                                    .required = true});
    parser.add_section("Flags");
    parser.add_subsection("SubFlags");
    parser.add_line("here come all the flags");
    parser.add_flag(flag_value, sharg::config{.short_id = 'f', .long_id = "flag", .description = "this is a flag."});
    parser.add_positional_option(non_list_optional, sharg::config{.description = "this is not a list."});
    parser.add_positional_option(pos_opt_value, sharg::config{.description = "this is a positional option."});
    parser.info.examples.push_back("example");
    parser.info.examples.push_back("example2");

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
               "          this is a positional option. Default: []\n"
               "\nOPTIONS\n"
               "    -i, --int (signed 32 bit integer)\n"
               "          this is a int option. Default: A number\n"
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
             + basic_options_str
             + "\n"
               "EXAMPLES\n"
               "    example\n"
               "\n"
               "    example2\n"
               "\n"
             + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, copyright)
{
    // Test --copyright with empty short and long copyright info.
    auto parser = get_parser("--copyright");

    expected = "================================================================================\n"
               "Copyright information for test_parser:\n"
               "--------------------------------------------------------------------------------\n"
               "test_parser copyright information not available.\n"
               "================================================================================\n"
               "This program contains SeqAn code licensed under the following terms:\n"
               "--------------------------------------------------------------------------------\n"
             + license_text;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // Test --copyright with a non-empty short copyright and an empty long copyright.
    parser = get_parser("--copyright");
    parser.info.short_copyright = "short copyright line 1\nshort copyright line 2";

    expected = "================================================================================\n"
               "Copyright information for test_parser:\n"
               "--------------------------------------------------------------------------------\n"
               "test_parser full copyright information not available. Displaying short copyright information instead:\n"
               "short copyright line 1\n"
               "short copyright line 2\n"
               "================================================================================\n"
               "This program contains SeqAn code licensed under the following terms:\n"
               "--------------------------------------------------------------------------------\n"
             + license_text;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // Test --copyright with a non-empty short copyright and a non-empty long copyright.
    parser = get_parser("--copyright");
    parser.info.long_copyright = "long copyright line 1\nlong copyright line 2";

    expected = "================================================================================\n"
               "Copyright information for test_parser:\n"
               "--------------------------------------------------------------------------------\n"
               "long copyright line 1\n"
               "long copyright line 2\n"
               "================================================================================\n"
               "This program contains SeqAn code licensed under the following terms:\n"
               "--------------------------------------------------------------------------------\n"
             + license_text;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_help_test, subcommand_parser)
{
    bool flag_value{false};
    int option_value{};

    auto parser = get_subcommand_parser({"-h"}, {"sub1", "sub2"});
    parser.info.description.push_back("description");
    parser.add_flag(flag_value, sharg::config{.short_id = 'f', .long_id = "foo", .description = "A flag."});
    parser.add_option(option_value, sharg::config{.short_id = 'o', .long_id = "option", .description = "An option."});

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
                           "    The following options belong to the top-level parser and need to be\n"
                           "    specified before the subcommand key word. Every argument after the\n"
                           "    subcommand key word is passed on to the corresponding sub-parser.\n"
                           "\nOPTIONS\n"
                           "    -f, --foo\n"
                           "          A flag.\n"
                           "    -o, --option (signed 32 bit integer)\n"
                           "          An option. Default: 0\n"
                           "\n"
                         + basic_options_str + "\n" + version_str();
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}
