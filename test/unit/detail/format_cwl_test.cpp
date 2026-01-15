// SPDX-FileCopyrightText: 2006-2026, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <sharg/parser.hpp>
#include <sharg/test/test_fixture.hpp>

class format_cwl_test : public sharg::test::test_fixture
{};

#if !SHARG_HAS_TDL
TEST_F(format_cwl_test, skipped)
{
    GTEST_SKIP() << "TDL is not available.";
}
#else
TEST_F(format_cwl_test, empty_information)
{
    // Create the dummy parser.
    auto parser = get_parser("--export-help", "cwl");
    parser.info.date = "December 01, 1994";
    parser.info.version = "1.2.0-rc.2";
    parser.info.man_page_title = "default_man_page_title";
    parser.info.short_description = "A short description here.";

    std::string expected_short = "label: test_parser\n"
                                 "doc: \"\"\n"
                                 "inputs:\n"
                                 "  []\n"
                                 "outputs:\n"
                                 "  []\n"
                                 "cwlVersion: v1.2\n"
                                 "class: CommandLineTool\n"
                                 "baseCommand:\n"
                                 "  - test_parser\n";

    // Test the dummy parser with minimal information.
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected_short);
}

TEST_F(format_cwl_test, full_information)
{
    // Create variables for the arguments
    int option_value{5};
    bool flag_value{false};
    int8_t non_list_pos_opt_value{1};
    auto list_pos_opt_value = std::vector<std::string>{};

    // Create the dummy parser.
    auto parser = get_parser("--export-help", "cwl");
    parser.info.date = "December 01, 1994";
    parser.info.version = "01.01.01";
    parser.info.man_page_title = "default_ctd_page_title";
    parser.info.short_description = "A short description here.";
    parser.info.synopsis = {"./format_cwl_test synopsis"};
    parser.info.synopsis.push_back("./format_cwl_test synopsis2");
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
    parser.add_positional_option(non_list_pos_opt_value, sharg::config{.description = "this is a positional option."});
    parser.add_positional_option(list_pos_opt_value, sharg::config{.description = "this is a positional option."});
    parser.info.examples.push_back("example");
    parser.info.examples.push_back("example2");

    std::string expected_short = "label: test_parser\n"
                                 "doc: \"description\\n"
                                 "description2\\n"
                                 "\"\n"
                                 "inputs:\n"
                                 "  positional_0:\n"
                                 "    doc: this is a positional option.\n"
                                 "    type: string\n"
                                 "  positional_1:\n"
                                 "    doc: \"this is a positional option. Default: []\"\n"
                                 "    type: string[]?\n"
                                 "  int:\n"
                                 "    doc: \"this is a int option. Default: 5\"\n"
                                 "    type: long?\n"
                                 "    inputBinding:\n"
                                 "      prefix: --int\n"
                                 "  jint:\n"
                                 "    doc: this is a required int option.\n"
                                 "    type: long\n"
                                 "    inputBinding:\n"
                                 "      prefix: --jint\n"
                                 "  flag:\n"
                                 "    doc: this is a flag.\n"
                                 "    type: boolean?\n"
                                 "  kflag:\n"
                                 "    doc: this is a flag.\n"
                                 "    type: boolean?\n"
                                 "outputs:\n"
                                 "  []\n"
                                 "cwlVersion: v1.2\n"
                                 "class: CommandLineTool\n"
                                 "baseCommand:\n"
                                 "  - test_parser\n";

    // Test the dummy parser with minimal information.
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected_short);
}

TEST_F(format_cwl_test, subparser)
{
    // Create variables for the arguments
    int option_value{5};
    float option_value_float{0};
    std::string option_value_string{};
    std::filesystem::path option_value_path{};
    bool flag_value{false};
    int8_t non_list_pos_opt_value{1};
    auto list_pos_opt_value = std::vector<std::string>{};

    // Create the dummy parser.
    auto parser = get_subcommand_parser({"index", "--export-help", "cwl"}, {"index"});
    parser.info.date = "December 01, 1994";
    parser.info.version = "01.01.01";
    parser.info.man_page_title = "default_ctd_page_title";
    parser.info.short_description = "A short description here.";
    parser.info.synopsis = {"./format_cwl_test synopsis"};
    parser.info.synopsis.push_back("./format_cwl_test synopsis2");
    parser.info.description.push_back("description");
    parser.info.description.push_back("description2");

    EXPECT_NO_THROW(parser.parse());

    auto & sub_parser = parser.get_sub_parser();
    ASSERT_EQ(sub_parser.info.app_name, "test_parser-index");
    sub_parser.add_option(option_value, sharg::config{'i', "int", "this is a int option."});
    sub_parser.add_option(option_value,
                          sharg::config{.short_id = 'j',
                                        .long_id = "jint",
                                        .description = "this is a required int option.",
                                        .required = true});
    sub_parser.add_option(option_value_float,
                          sharg::config{.short_id = 'p',
                                        .long_id = "percent",
                                        .description = "this is a required float option.",
                                        .required = true});
    sub_parser.add_option(option_value_string,
                          sharg::config{.short_id = 's',
                                        .long_id = "string",
                                        .description = "this is a string option (advanced).",
                                        .advanced = true,
                                        .required = false});
    sub_parser.add_option(option_value_string,
                          sharg::config{.short_id = '\0',
                                        .long_id = "hide",
                                        .description = "a hidden option, that doesn't show up.",
                                        .hidden = true});
    sub_parser.add_option(option_value_path,
                          sharg::config{.short_id = '\0',
                                        .long_id = "path01",
                                        .description = "a normal file.",
                                        .validator = sharg::input_file_validator{}});
    sub_parser.add_option(option_value_path,
                          sharg::config{.short_id = '\0',
                                        .long_id = "path02",
                                        .description = "a normal file with a valid file extension.",
                                        .validator = sharg::input_file_validator{{".fa", ".fasta"}}});
    sub_parser.add_option(option_value_path,
                          sharg::config{.short_id = '\0',
                                        .long_id = "path03",
                                        .description = "a input directory.",
                                        .validator = sharg::input_directory_validator{}});
    sub_parser.add_option(option_value_path,
                          sharg::config{.short_id = '\0',
                                        .long_id = "path04",
                                        .description = "a output file.",
                                        .validator = sharg::output_file_validator{}});
    sub_parser.add_option(option_value_path,
                          sharg::config{.short_id = '\0',
                                        .long_id = "path05",
                                        .description = "a output directory.",
                                        .validator = sharg::output_directory_validator{}});

    sub_parser.add_section("Flags");
    sub_parser.add_subsection("SubFlags");
    sub_parser.add_line("here come all the flags");
    sub_parser.add_flag(flag_value, sharg::config{'f', "flag", "this is a flag."});
    sub_parser.add_flag(flag_value, sharg::config{'k', "kflag", "this is a flag."});
    sub_parser.add_positional_option(non_list_pos_opt_value,
                                     sharg::config{.description = "this is a positional option."});
    sub_parser.add_positional_option(list_pos_opt_value, sharg::config{.description = "this is a positional option."});
    sub_parser.info.examples.push_back("example");
    sub_parser.info.examples.push_back("example2");

    std::string expected_short =
        "label: test_parser-index\n"
        "doc: \"\"\n"
        "inputs:\n"
        "  positional_0:\n"
        "    doc: this is a positional option.\n"
        "    type: string\n"
        "  positional_1:\n"
        "    doc: \"this is a positional option. Default: []\"\n"
        "    type: string[]?\n"
        "  int:\n"
        "    doc: \"this is a int option. Default: 5\"\n"
        "    type: long?\n"
        "    inputBinding:\n"
        "      prefix: --int\n"
        "  jint:\n"
        "    doc: this is a required int option.\n"
        "    type: long\n"
        "    inputBinding:\n"
        "      prefix: --jint\n"
        "  percent:\n"
        "    doc: this is a required float option.\n"
        "    type: double\n"
        "    inputBinding:\n"
        "      prefix: --percent\n"
        "  string:\n"
        "    doc: \"this is a string option (advanced). Default: \\\"\\\"\"\n"
        "    type: string?\n"
        "    inputBinding:\n"
        "      prefix: --string\n"
        "  path01:\n"
        "    doc: \"a normal file. Default: \\\"\\\". The input file must exist and read permissions must be "
        "granted.\"\n"
        "    type: File?\n"
        "    inputBinding:\n"
        "      prefix: --path01\n"
        "  path02:\n"
        "    doc: \"a normal file with a valid file extension. Default: \\\"\\\". The input file must exist and read "
        "permissions must be granted. Valid file extensions are: [.fa, .fasta].\"\n"
        "    type: File?\n"
        "    inputBinding:\n"
        "      prefix: --path02\n"
        "  path03:\n"
        "    doc: \"a input directory. Default: \\\"\\\". An existing, readable path for the input directory.\"\n"
        "    type: Directory?\n"
        "    inputBinding:\n"
        "      prefix: --path03\n"
        "  path04:\n"
        "    doc: \"a output file. Default: \\\"\\\". The output file must not exist already and write permissions "
        "must be granted.\"\n"
        "    type: string?\n"
        "    inputBinding:\n"
        "      prefix: --path04\n"
        "  path05:\n"
        "    doc: \"a output directory. Default: \\\"\\\". A valid path for the output directory.\"\n"
        "    type: string?\n"
        "    inputBinding:\n"
        "      prefix: --path05\n"
        "  flag:\n"
        "    doc: this is a flag.\n"
        "    type: boolean?\n"
        "  kflag:\n"
        "    doc: this is a flag.\n"
        "    type: boolean?\n"
        "outputs:\n"
        "  path04:\n"
        "    type: File?\n"
        "    outputBinding:\n"
        "      glob: $(inputs.path04)\n"
        "  path05:\n"
        "    type: Directory?\n"
        "    outputBinding:\n"
        "      glob: $(inputs.path05)\n"
        "cwlVersion: v1.2\n"
        "class: CommandLineTool\n"
        "baseCommand:\n"
        "  - test_parser\n"
        "  - index\n";
    EXPECT_EQ(get_parse_cout_on_exit(sub_parser), expected_short);
}

TEST_F(format_cwl_test, subsubparser)
{
    // Create variables for the arguments
    int option_value{5};
    std::string option_value_string{};
    std::filesystem::path option_value_path{};

    // Create the dummy parser.
    auto parser = get_subcommand_parser({"index", "show", "--export-help", "cwl"}, {"index"});

    EXPECT_NO_THROW(parser.parse());

    auto & sub_parser = parser.get_sub_parser();
    ASSERT_EQ(sub_parser.info.app_name, "test_parser-index");

    sub_parser.add_subcommands({"show"});
    EXPECT_NO_THROW(sub_parser.parse());

    auto & sub_sub_parser = sub_parser.get_sub_parser();
    sub_sub_parser.add_option(option_value,
                              sharg::config{.short_id = 'j',
                                            .long_id = "jint",
                                            .description = "this is a required int option.",
                                            .required = true});
    sub_sub_parser.add_option(option_value_string,
                              sharg::config{.short_id = 's',
                                            .long_id = "string",
                                            .description = "this is a string option (advanced).",
                                            .advanced = true,
                                            .required = false});
    sub_sub_parser.add_option(option_value_path,
                              sharg::config{.short_id = '\0',
                                            .long_id = "path04",
                                            .description = "a output file.",
                                            .validator = sharg::output_file_validator{}});

    std::string expected_short =
        "label: test_parser-index-show\n"
        "doc: \"\"\n"
        "inputs:\n"
        "  jint:\n"
        "    doc: this is a required int option.\n"
        "    type: long\n"
        "    inputBinding:\n"
        "      prefix: --jint\n"
        "  string:\n"
        "    doc: \"this is a string option (advanced). Default: \\\"\\\"\"\n"
        "    type: string?\n"
        "    inputBinding:\n"
        "      prefix: --string\n"
        "  path04:\n"
        "    doc: \"a output file. Default: \\\"\\\". The output file must not exist already and write permissions "
        "must be granted.\"\n"
        "    type: string?\n"
        "    inputBinding:\n"
        "      prefix: --path04\n"
        "outputs:\n"
        "  path04:\n"
        "    type: File?\n"
        "    outputBinding:\n"
        "      glob: $(inputs.path04)\n"
        "cwlVersion: v1.2\n"
        "class: CommandLineTool\n"
        "baseCommand:\n"
        "  - test_parser\n"
        "  - index\n"
        "  - show\n";
    EXPECT_EQ(get_parse_cout_on_exit(sub_sub_parser), expected_short);
}
#endif
