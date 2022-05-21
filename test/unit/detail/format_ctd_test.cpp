// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <sharg/argument_parser.hpp>

// Reused global variables
struct format_ctd_test : public ::testing::Test
{
    int option_value{5};
    bool flag_value{false};
    int8_t non_list_pos_opt_value{1};
    std::vector<std::string> list_pos_opt_value{};
    std::string my_stdout{};
    const char * argv[4] = {"./format_ctd_test --version-check false", "--export-help", "ctd"};
    std::string const version_str{sharg::sharg_version_cstring};
    std::string expected =
        R"del(<?xml version="1.0" encoding="UTF-8"?>)del" "\n"
        R"del(<tool ctdVersion="1.7" version="01.01.01" name="default" docurl="" category="" >)del" "\n"
        R"del(<description><![CDATA[description)del" "\n"
        R"del(description2)del" "\n"
        R"del(]]></description>)del" "\n"
        R"del(<manual><![CDATA[description)del" "\n"
        R"del(description2)del" "\n"
        R"del(]]></manual>)del" "\n"
        R"del(<citations>)del" "\n"
        R"del(  <citation doi="" url="" />)del" "\n"
        R"del(</citations>)del" "\n"
        R"del(<PARAMETERS version="1.7.0" xsi:noNamespaceSchemaLocation="https://raw.githubusercontent.com/OpenMS/OpenMS/develop/share/OpenMS/SCHEMAS/Param_1_7_0.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">)del" "\n"
        R"del(  <ITEM name="1 (signed 8 bit integer)" value="" type="string" description="this is a positional option. " required="false" advanced="false" />)del" "\n"
        R"del(  <NODE name="2 (List of std" description="">)del" "\n"
        R"del(    <NODE name="" description="">)del" "\n"
        R"del(      <ITEM name="string)" value="" type="string" description="this is a positional option. Default: []. " required="false" advanced="false" />)del" "\n"
        R"del(    </NODE>)del" "\n"
        R"del(  </NODE>)del" "\n"
        R"del(  <NODE name="default" description="">)del" "\n"
        R"del(    <ITEM name="-h," value="" type="string" description="Prints the help page." required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="-hh," value="" type="string" description="Prints the help page including advanced options." required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--version" value="" type="string" description="Prints the version information." required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--copyright" value="" type="string" description="Prints the copyright/license information." required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--export-help" value="" type="string" description="Export the help page information. Value must be one of [html, man, ctd]." required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--version-check" value="" type="string" description="Whether to check for the newest app version. Default: true." required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--int" value="5" type="int" description="this is a int option. Default: 5. " required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--jint" value="5" type="int" description="this is a required int option. " required="true" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--flag" value="0" type="int" description="this is a flag." required="false" advanced="false" />)del" "\n"
        R"del(    <ITEM name="--kflag" value="0" type="int" description="this is a flag." required="false" advanced="false" />)del" "\n"
        R"del(  </NODE>)del" "\n"
        R"del(</NODE>)del" "\n"
        R"del(</NODE>)del" "\n"
        R"del(</PARAMETERS>)del" "\n"
        R"del(</tool>)del" "\n";

    // Full info parser initialisation
    void dummy_init(sharg::argument_parser & parser)
    {
        parser.info.date = "December 01, 1994";
        parser.info.version = "01.01.01";
        parser.info.man_page_title = "default_ctd_page_title";
        parser.info.short_description = "A short description here.";
        parser.info.synopsis.push_back("./format_ctd_test synopsis");
        parser.info.synopsis.push_back("./format_ctd_test synopsis2");
        parser.info.description.push_back("description");
        parser.info.description.push_back("description2");
        parser.add_option(option_value, 'i', "int", "this is a int option.");
        parser.add_option(option_value, 'j', "jint", "this is a required int option.", sharg::option_spec::required);
        parser.add_section("Flags");
        parser.add_subsection("SubFlags");
        parser.add_line("here come all the flags");
        parser.add_flag(flag_value, 'f', "flag", "this is a flag.");
        parser.add_flag(flag_value, 'k', "kflag", "this is a flag.");
        parser.add_positional_option(non_list_pos_opt_value, "this is a positional option.");
        parser.add_positional_option(list_pos_opt_value, "this is a positional option.");
        parser.info.examples.push_back("example");
        parser.info.examples.push_back("example2");
    }
};

TEST_F(format_ctd_test, empty_information)
{
    // Create the dummy parser.
    sharg::argument_parser parser{"default", 3, argv};
    parser.info.date = "December 01, 1994";
    parser.info.version = "0.1.0";
    parser.info.man_page_title = "default_man_page_title";
    parser.info.short_description = "A short description here.";

    std::string const version_str{sharg::sharg_version_cstring};
    std::string expected_short =
        R"(<?xml version="1.0" encoding="UTF-8"?>)" "\n"
        R"(<tool ctdVersion="1.7" version=")" + version_str + R"(" name="default" docurl="" category="" >)" "\n"
        R"(<description><![CDATA[]]></description>)" "\n"
        R"(<manual><![CDATA[]]></manual>)" "\n"
        R"(<citations>)" "\n"
        R"(  <citation doi="" url="" />)" "\n"
        R"(</citations>)" "\n"
        R"(<PARAMETERS version="1.7.0" xsi:noNamespaceSchemaLocation="https://raw.githubusercontent.com/OpenMS/OpenMS/develop/share/OpenMS/SCHEMAS/Param_1_7_0.xsd" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance">)" "\n"
        R"(  <NODE name="default" description="">)" "\n"
        R"(    <ITEM name="-h," value="" type="string" description="Prints the help page." required="false" advanced="false" />)" "\n"
        R"(    <ITEM name="-hh," value="" type="string" description="Prints the help page including advanced options." required="false" advanced="false" />)" "\n"
        R"(    <ITEM name="--version" value="" type="string" description="Prints the version information." required="false" advanced="false" />)" "\n"
        R"(    <ITEM name="--copyright" value="" type="string" description="Prints the copyright/license information." required="false" advanced="false" />)" "\n"
        R"(    <ITEM name="--export-help" value="" type="string" description="Export the help page information. Value must be one of [html, man, ctd]." required="false" advanced="false" />)" "\n"
        R"(    <ITEM name="--version-check" value="" type="string" description="Whether to check for the newest app version. Default: true." required="false" advanced="false" />)" "\n"
        R"(  </NODE>)" "\n"
        R"(</PARAMETERS>)" "\n"
        R"(</tool>)" "\n";

    // Test the dummy parser with minimal information.
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    my_stdout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(my_stdout, expected_short);
}

TEST_F(format_ctd_test, full_information)
{
    // Create the dummy parser.
    sharg::argument_parser parser{"default", 3, argv};

    // Fill out the dummy parser with options and flags and sections and subsections.
    dummy_init(parser);
    // Test the dummy parser without any copyright or citations.
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    my_stdout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(my_stdout, expected);
}

#if 0
TEST_F(format_ctd_test, full_info_short_copyright)
{
    // Create the dummy parser.
    sharg::argument_parser parser{"default", 3, argv};

    // Fill out the dummy parser with options and flags and sections and subsections.
    dummy_init(parser);
    // Add a short copyright and test the dummy parser.
    parser.info.short_copyright = "short copyright";
    expected += R"(.SH LEGAL
\fBdefault Copyright: \fRshort copyright
.br
\fBSeqAn Copyright: \fR2006-2021 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.
)";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    my_stdout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(my_stdout, expected);
}

TEST_F(format_ctd_test, full_info_short_and_citation)
{
    // Create the dummy parser.
    sharg::argument_parser parser{"default", 3, argv};

    // Fill out the dummy parser with options and flags and sections and subsections.
    dummy_init(parser);
    // Add a short copyright & citation and test the dummy parser.
    parser.info.short_copyright = "short copyright";
    parser.info.citation = "citation";
    expected += R"(.SH LEGAL
\fBdefault Copyright: \fRshort copyright
.br
\fBSeqAn Copyright: \fR2006-2021 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.
.br
\fBIn your academic works please cite: \fRcitation
)";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    my_stdout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(my_stdout, expected);
}

TEST_F(format_ctd_test, full_info_short_long_and_citation)
{
    // Create the dummy parser.
    sharg::argument_parser parser{"default", 3, argv};

    // Fill out the dummy parser with options and flags and sections and subsections.
    dummy_init(parser);
    // Add a short copyright & citation & long copyright and test the dummy parser.
    parser.info.short_copyright = "short copyright";
    parser.info.citation = "citation";
    parser.info.long_copyright = "looong copyright";
    expected += R"(.SH LEGAL
\fBdefault Copyright: \fRshort copyright
.br
\fBSeqAn Copyright: \fR2006-2021 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.
.br
\fBIn your academic works please cite: \fRcitation
.br
For full copyright and/or warranty information see \fB--copyright\fR.
)";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    my_stdout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(my_stdout, expected);
}

TEST_F(format_ctd_test, full_info_author)
{
    // Create the dummy parser.
    sharg::argument_parser parser{"default", 3, argv};

    // Fill out the dummy parser with options and flags and sections and subsections.
    dummy_init(parser);
    // Add a short copyright and test the dummy parser.
    parser.info.author = "author";
    expected += R"(.SH LEGAL
\fBAuthor: \fRauthor
.br
\fBSeqAn Copyright: \fR2006-2021 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.
)";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    my_stdout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(my_stdout, expected);
}

TEST_F(format_ctd_test, full_info_email)
{
    // Create the dummy parser.
    sharg::argument_parser parser{"default", 3, argv};

    // Fill out the dummy parser with options and flags and sections and subsections.
    dummy_init(parser);
    // Add a short copyright and test the dummy parser.
    parser.info.email = "email";
    expected += R"(.SH LEGAL
\fBContact: \fRemail
.br
\fBSeqAn Copyright: \fR2006-2021 Knut Reinert, FU-Berlin; released under the 3-clause BSDL.
)";
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    my_stdout = testing::internal::GetCapturedStdout();
    EXPECT_EQ(my_stdout, expected);
}
#endif
