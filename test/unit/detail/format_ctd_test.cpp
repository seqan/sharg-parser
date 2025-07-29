// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <sharg/parser.hpp>
#include <sharg/test/test_fixture.hpp>

// Reused global variables
class format_ctd_test : public sharg::test::test_fixture
{
protected:
    int option_value{5};
    bool flag_value{false};
    int8_t non_list_pos_opt_value{1};
    std::vector<std::string> list_pos_opt_value{};
    std::string my_stdout{};
    std::string const version_str{sharg::sharg_version_cstring};
    std::string const expected =
        R"del(<?xml version="1.0" encoding="UTF-8"?>)del"
        "\n"
        R"del(<tool ctdVersion="1.7" version="01.01.01" name="test_parser">)del"
        "\n"
        R"del(    <description><![CDATA[description)del"
        "\n"
        R"del(description2)del"
        "\n"
        R"del(]]></description>)del"
        "\n"
        R"del(    <manual><![CDATA[description)del"
        "\n"
        R"del(description2)del"
        "\n"
        R"del(]]></manual>)del"
        "\n"
        R"del(    <executableName><![CDATA[./test_parser]]></executableName>)del"
        "\n"
        R"del(    <citations />)del"
        "\n"
        R"del(    <PARAMETERS version="1.7.0" xsi:noNamespaceSchemaLocation="https://raw.githubusercontent.com/OpenMS/OpenMS/develop/share/OpenMS/SCHEMAS/Param_1_7_0.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">)del"
        "\n"
        R"del(        <ITEM name="positional_0" value="" type="string" description="this is a positional option." required="true" advanced="false" />)del"
        "\n"
        R"del(        <ITEMLIST name="positional_1" type="string" description="this is a positional option. Default: []" required="false" advanced="false" />)del"
        "\n"
        R"del(        <ITEM name="int" value="5" type="int" description="this is a int option. Default: 5" required="false" advanced="false" />)del"
        "\n"
        R"del(        <ITEM name="jint" value="5" type="int" description="this is a required int option." required="true" advanced="false" />)del"
        "\n"
        R"del(        <ITEM name="flag" value="false" type="bool" description="this is a flag." required="false" advanced="false" />)del"
        "\n"
        R"del(        <ITEM name="kflag" value="false" type="bool" description="this is a flag." required="false" advanced="false" />)del"
        "\n"
        R"del(    </PARAMETERS>)del"
        "\n"
        R"del(    <cli>)del"
        "\n"
        R"del(        <clielement optionIdentifier="--int">)del"
        "\n"
        R"del(            <mapping referenceName="int" />)del"
        "\n"
        R"del(        </clielement>)del"
        "\n"
        R"del(        <clielement optionIdentifier="--jint">)del"
        "\n"
        R"del(            <mapping referenceName="jint" />)del"
        "\n"
        R"del(        </clielement>)del"
        "\n"
        R"del(    </cli>)del"
        "\n"
        R"del(</tool>)del"
        "\n";

    // Full info parser initialisation
    void dummy_init(sharg::parser & parser)
    {
        parser.info.date = "December 01, 1994";
        parser.info.version = "01.01.01";
        parser.info.man_page_title = "default_ctd_page_title";
        parser.info.short_description = "A short description here.";
        parser.info.synopsis.push_back("./format_ctd_test synopsis");
        parser.info.synopsis.push_back("./format_ctd_test synopsis2");
        parser.info.description.push_back("description");
        parser.info.description.push_back("description2");
        parser.add_option(option_value, sharg::config{'i', "int", "this is a int option."});
        parser.add_option(option_value,
                          sharg::config{.short_id = 'j',
                                        .long_id = "jint",
                                        .description = "this is a required int option.",
                                        .required = true});
        parser.add_section("Flags");
        parser.add_subsection("SubFlags");
        parser.add_line("here come all the flags");
        parser.add_flag(flag_value, sharg::config{'f', "flag", "this is a flag."});
        parser.add_flag(flag_value, sharg::config{'k', "kflag", "this is a flag."});
        parser.add_positional_option(non_list_pos_opt_value,
                                     sharg::config{.description = "this is a positional option."});
        parser.add_positional_option(list_pos_opt_value, sharg::config{.description = "this is a positional option."});
        parser.info.examples.push_back("example");
        parser.info.examples.push_back("example2");
    }
};

#if !SHARG_HAS_TDL
TEST_F(format_ctd_test, skipped)
{
    GTEST_SKIP() << "TDL is not available.";
}
#else
TEST_F(format_ctd_test, empty_information)
{
    // Create the dummy parser.
    auto parser = get_parser("--export-help", "ctd");
    parser.info.date = "December 01, 1994";
    parser.info.version = "1.2.0-rc.1";
    parser.info.man_page_title = "default_man_page_title";
    parser.info.short_description = "A short description here.";

    std::string const version_str{sharg::sharg_version_cstring};
    std::string expected_short =
        R"(<?xml version="1.0" encoding="UTF-8"?>)"
        "\n"
        R"(<tool ctdVersion="1.7" version=")"
        + version_str
        + R"(" name="test_parser">)"
          "\n"
          R"(    <executableName><![CDATA[./test_parser]]></executableName>)"
          "\n"
          R"(    <citations />)"
          "\n"
          R"(    <PARAMETERS version="1.7.0" xsi:noNamespaceSchemaLocation="https://raw.githubusercontent.com/OpenMS/OpenMS/develop/share/OpenMS/SCHEMAS/Param_1_7_0.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" />)"
          "\n"
          R"(</tool>)"
          "\n";

    // Test the dummy parser with minimal information.
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected_short);
}

TEST_F(format_ctd_test, full_information)
{
    // Create the dummy parser.
    auto parser = get_parser("--export-help", "ctd");

    // Fill out the dummy parser with options and flags and sections and subsections.
    dummy_init(parser);

    // Test the dummy parser without any copyright or citations.
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}
#endif
