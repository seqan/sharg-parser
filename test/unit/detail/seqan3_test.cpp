// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <sharg/parser.hpp>
#include <sharg/test/test_fixture.hpp>

#if !__has_include(<seqan3/version.hpp>)
#    error "seqan3/version.hpp is not available"
#endif

std::string const basic_options_str = "  Common options\n"
                                      "    -h, --help\n"
                                      "          Prints the help page.\n"
                                      "    -hh, --advanced-help\n"
                                      "          Prints the help page including advanced options.\n"
                                      "    --version\n"
                                      "          Prints the version information.\n"
                                      "    --copyright\n"
                                      "          Prints the copyright/license information.\n"
                                      "    --export-help (std::string)\n"
                                      "          Export the help page information. Value must be one of "
#if SHARG_HAS_TDL
                                      "[html, man,\n          ctd, cwl].\n";
#else
                                      "[html, man].\n";
#endif

std::string const basic_version_str = "VERSION\n"
                                      "    Last update:\n"
                                      "    test_parser version:\n"
                                      "    Sharg version: "
                                    + std::string{sharg::sharg_version_cstring}
                                    + "\n"
                                      "    SeqAn version: "
                                    + std::string{seqan3::seqan3_version_cstring} + "\n";

class seqan3_test : public sharg::test::test_fixture
{};

TEST_F(seqan3_test, version_string)
{
    auto parser = get_parser("-h");
    std::string expected = "test_parser\n"
                           "===========\n"
                           "\nOPTIONS\n\n"
                         + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}
