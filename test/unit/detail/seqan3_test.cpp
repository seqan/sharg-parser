// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <sharg/parser.hpp>
#include <sharg/test/test_fixture.hpp>

#if !__has_include(<seqan3/version.hpp>)
#    error "seqan3/version.hpp is not available"
#endif

class seqan3_test : public sharg::test::test_fixture
{
protected:
    static inline std::string version_str_with_seqan3 =
        version_str() + "    SeqAn version: " + seqan3::seqan3_version_cstring + '\n';
};

TEST_F(seqan3_test, version_string)
{
    auto parser = get_parser("-h");
    std::string expected = "test_parser\n"
                           "===========\n"
                           "\nOPTIONS\n\n"
                         + basic_options_str + '\n' + version_str_with_seqan3;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}
