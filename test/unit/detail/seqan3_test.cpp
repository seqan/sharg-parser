// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <sharg/parser.hpp>

std::string const basic_options_str = "  Common options:\n"
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
                                    + std::string{sharg::sharg_version_cstring}
                                    + "\n"
                                      "    SeqAn version: "
                                    + std::string{seqan3::seqan3_version_cstring} + "\n";

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

TEST(help_page_printing, no_information)
{
    char const * argv[] = {"./help_add_test --version-check false", "-h"};
    sharg::parser parser1{"test_parser", 2, argv};
    sharg::detail::test_accessor::set_terminal_width(parser1, 80);
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser1.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string std_cout = testing::internal::GetCapturedStdout();
    std::string expected = "test_parser\n"
                           "===========\n"
                           "\nOPTIONS\n\n"
                         + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(std_cout, expected);
}
