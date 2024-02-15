// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <sharg/parser.hpp>
#include <sharg/test/test_fixture.hpp>

class format_html_test : public sharg::test::test_fixture
{};

TEST_F(format_html_test, empty_information)
{
    // Empty html help page.
    auto parser = get_parser("--version-check", "false", "--export-help", "html");

    std::string expected =
        std::string("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" http://www.w3.org/TR/html4/strict.dtd\">\n"
                    "<html lang=\"en\">\n"
                    "<head>\n"
                    "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
                    "<title>test_parser &mdash; </title>\n"
                    "</head>\n"
                    "<body>\n"
                    "<h1>test_parser</h1>\n"
                    "<div></div>\n"
                    "<h2>Options</h2>\n"
                    "<h3>Common options</h3>\n"
                    "<dl>\n"
                    "<dt><strong>-h</strong>, <strong>--help</strong></dt>\n"
                    "<dd>Prints the help page.</dd>\n"
                    "<dt><strong>-hh</strong>, <strong>--advanced-help</strong></dt>\n"
                    "<dd>Prints the help page including advanced options.</dd>\n"
                    "<dt><strong>--version</strong></dt>\n"
                    "<dd>Prints the version information.</dd>\n"
                    "<dt><strong>--copyright</strong></dt>\n"
                    "<dd>Prints the copyright/license information.</dd>\n"
                    "<dt><strong>--export-help</strong> (std::string)</dt>\n"
                    "<dd>Export the help page information. Value must be one of "
#if SHARG_HAS_TDL
                    "[html, man, ctd, cwl].</dd>\n"
#else
                    "[html, man].</dd>\n"
#endif
                    "</dl>\n"
                    "<h2>Version</h2>\n"
                    "<p>\n"
                    "<strong>Last update: </strong>\n"
                    "<br>\n"
                    "<strong>test_parser version: </strong>\n"
                    "<br>\n"
                    "<strong>Sharg version: </strong>"
                    + std::string{sharg::sharg_version_cstring}
                    + "\n"
                      "<br>\n"
                      "</p>\n"
                      "</body></html>");
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    parser = get_parser("--version-check", "false", "--export-help=html");
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_html_test, full_information_information)
{
    int option_value{5};
    bool flag_value{false};
    int8_t non_list_pos_opt_value{1};
    std::vector<std::string> list_pos_opt_value{};

    // Full html help page.
    auto parser = get_parser("--version-check", "false", "--export-help", "html");
    parser.info.synopsis.push_back("./some_binary_name synopsis");
    parser.info.synopsis.push_back("./some_binary_name synopsis2");
    parser.info.description.push_back("description");
    parser.info.description.push_back("description2");
    parser.info.short_description = "short description";
    parser.info.url = "https://seqan.de";
    parser.info.short_copyright = "short copyright";
    parser.info.long_copyright = "long_copyright";
    parser.info.citation = "citation";
    parser.info.author = "author";
    parser.info.email = "email";
    parser.add_option(option_value,
                      sharg::config{.short_id = 'i',
                                    .long_id = "int",
                                    .description = "this is a int option.",
                                    .default_message = "A number"});
    parser.add_option(option_value,
                      sharg::config{.short_id = 'j',
                                    .long_id = "jint",
                                    .description = "this is a required int option.",
                                    .required = true});
    parser.add_flag(flag_value, sharg::config{.short_id = 'f', .long_id = "flag", .description = "this is a flag."});
    parser.add_flag(flag_value, sharg::config{.short_id = 'k', .long_id = "kflag", .description = "this is a flag."});
    parser.add_positional_option(non_list_pos_opt_value, sharg::config{.description = "this is a positional option."});
    parser.add_positional_option(list_pos_opt_value, sharg::config{.description = "this is a positional option."});
    parser.info.examples.push_back("example");
    parser.info.examples.push_back("example2");

    std::string expected = std::string(
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" http://www.w3.org/TR/html4/strict.dtd\">\n"
        "<html lang=\"en\">\n"
        "<head>\n"
        "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
        "<title>test_parser &mdash; short description</title>\n"
        "</head>\n"
        "<body>\n"
        "<h1>test_parser</h1>\n"
        "<div>short description</div>\n"
        "<h2>Synopsis</h2>\n"
        "<p>\n"
        "<strong>./some_binary_name</strong> synopsis\n"
        "<br>\n"
        "<strong>./some_binary_name</strong> synopsis2\n"
        "<br>\n"
        "</p>\n"
        "<h2>Description</h2>\n"
        "<p>\n"
        "description\n"
        "</p>\n"
        "<p>\n"
        "description2\n"
        "</p>\n"
        "<h2>Positional Arguments</h2>\n"
        "<dl>\n"
        "<dt><strong>ARGUMENT-1</strong> (<em>signed 8 bit integer</em>)</dt>\n"
        "<dd>this is a positional option.</dd>\n"
        "<dt><strong>ARGUMENT-2</strong> (<em>List</em> of <em>std::string</em>)</dt>\n"
        "<dd>this is a positional option. Default: []</dd>\n"
        "</dl>\n"
        "<h2>Options</h2>\n"
        "<dl>\n"
        "<dt><strong>-i</strong>, <strong>--int</strong> (<em>signed 32 bit integer</em>)</dt>\n"
        "<dd>this is a int option. Default: A number</dd>\n"
        "<dt><strong>-j</strong>, <strong>--jint</strong> (<em>signed 32 bit integer</em>)</dt>\n"
        "<dd>this is a required int option.</dd>\n"
        "<dt><strong>-f</strong>, <strong>--flag</strong></dt>\n"
        "<dd>this is a flag.</dd>\n"
        "<dt><strong>-k</strong>, <strong>--kflag</strong></dt>\n"
        "<dd>this is a flag.</dd>\n"
        "</dl>\n"
        "<h3>Common options</h3>\n"
        "<dl>\n"
        "<dt><strong>-h</strong>, <strong>--help</strong></dt>\n"
        "<dd>Prints the help page.</dd>\n"
        "<dt><strong>-hh</strong>, <strong>--advanced-help</strong></dt>\n"
        "<dd>Prints the help page including advanced options.</dd>\n"
        "<dt><strong>--version</strong></dt>\n"
        "<dd>Prints the version information.</dd>\n"
        "<dt><strong>--copyright</strong></dt>\n"
        "<dd>Prints the copyright/license information.</dd>\n"
        "<dt><strong>--export-help</strong> (std::string)</dt>\n"
        "<dd>Export the help page information. Value must be one of "
#if SHARG_HAS_TDL
        "[html, man, ctd, cwl].</dd>\n"
#else
        "[html, man].</dd>\n"
#endif
        "</dl>\n"
        "<h2>Examples</h2>\n"
        "<p>\n"
        "example\n"
        "</p>\n"
        "<p>\n"
        "example2\n"
        "</p>\n"
        "<h2>Version</h2>\n"
        "<p>\n"
        "<strong>Last update: </strong>\n"
        "<br>\n"
        "<strong>test_parser version: </strong>\n"
        "<br>\n"
        "<strong>Sharg version: </strong>"
        + std::string{sharg::sharg_version_cstring}
        + "\n"
          "<br>\n"
          "</p>\n"
          "<h2>Url</h2>\n"
          "<p>\n"
          "https://seqan.de\n"
          "<br>\n"
          "</p>\n"
          "<h2>Legal</h2>\n"
          "<p>\n"
          "<strong>test_parser Copyright: </strong>short copyright\n"
          "<br>\n"
          "<strong>Author: </strong>author\n"
          "<br>\n"
          "<strong>Contact: </strong>email\n"
          "<br>\n"
          "<strong>SeqAn Copyright: </strong>2006-2024 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.\n"
          "<br>\n"
          "<strong>In your academic works please cite: </strong>citation\n"
          "<br>\n"
          "For full copyright and/or warranty information see <strong>--copyright</strong>.\n"
          "<br>\n"
          "</p>\n"
          "</body></html>");
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(format_html_test, parse_error)
{
    // no value after --export-help
    auto parser = get_parser("./help_add_test", "--export-help");
    EXPECT_THROW(parser.parse(), sharg::parser_error);

    // wrong value after --export-help=
    parser = get_parser("./help_add_test", "--export-help=atml");
    EXPECT_THROW(parser.parse(), sharg::validation_error);

    // wrong value after --export-help
    parser = get_parser("./help_add_test", "--export-help", "atml");
    EXPECT_THROW(parser.parse(), sharg::validation_error);

    // Currently not checking for `=`
    parser = get_parser("./help_add_test", "--export-help#html");
    EXPECT_NE(get_parse_cout_on_exit(parser), "");
}
