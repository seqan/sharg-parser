// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>
#include <sharg/test/file_access.hpp>
#include <sharg/test/tmp_filename.hpp>

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
                                    + std::string{sharg::sharg_version_cstring} + "\n";

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

TEST(validator_test, fullfill_concept)
{
    EXPECT_FALSE(sharg::validator<int>);

    EXPECT_TRUE(sharg::validator<sharg::detail::default_validator>);

    EXPECT_TRUE(sharg::validator<sharg::arithmetic_range_validator<int>>);
    EXPECT_TRUE(sharg::validator<sharg::value_list_validator<double>>);
    EXPECT_TRUE(sharg::validator<sharg::value_list_validator<std::string>>);
    EXPECT_TRUE(sharg::validator<sharg::input_file_validator>);
    EXPECT_TRUE(sharg::validator<sharg::output_file_validator>);
    EXPECT_TRUE(sharg::validator<sharg::input_directory_validator>);
    EXPECT_TRUE(sharg::validator<sharg::output_directory_validator>);
    EXPECT_TRUE(sharg::validator<sharg::regex_validator>);

    EXPECT_TRUE(sharg::validator<decltype(sharg::input_file_validator{{"t"}} | sharg::regex_validator{".*"})>);
}

TEST(validator_test, input_file)
{
    sharg::test::tmp_filename tmp_name{"testbox.fasta"};
    sharg::test::tmp_filename tmp_name_2{"testbox_2.fasta"};
    sharg::test::tmp_filename tmp_name_hidden{".testbox.fasta"};
    sharg::test::tmp_filename tmp_name_multiple{"testbox.fasta.txt"};

    std::vector formats{std::string{"fa"}, std::string{"sam"}, std::string{"fasta"}, std::string{"fasta.txt"}};

    std::ofstream tmp_file(tmp_name.get_path());
    std::ofstream tmp_file_2(tmp_name_2.get_path());
    std::ofstream tmp_file_hidden(tmp_name_hidden.get_path());
    std::ofstream tmp_file_multiple(tmp_name_multiple.get_path());

    { // single file

        { // empty list of file.
            sharg::input_file_validator my_validator{};
            EXPECT_NO_THROW(my_validator(tmp_name.get_path()));
        }

        { // file already exists.
            std::filesystem::path does_not_exist{tmp_name.get_path()};
            does_not_exist.replace_extension(".bam");
            sharg::input_file_validator my_validator{formats};
            EXPECT_THROW(my_validator(does_not_exist), sharg::validation_error);
        }

        { // file has wrong format.
            sharg::input_file_validator my_validator{std::vector{std::string{"sam"}}};
            EXPECT_THROW(my_validator(tmp_name.get_path()), sharg::validation_error);
        }

        { // file has no extension.
            std::filesystem::path does_not_exist{tmp_name.get_path()};
            does_not_exist.replace_extension();
            sharg::input_file_validator my_validator{formats};
            EXPECT_THROW(my_validator(does_not_exist), sharg::validation_error);
        }

        { // filename starts with dot.
            sharg::input_file_validator my_validator{formats};
            EXPECT_NO_THROW(my_validator(tmp_name_hidden.get_path()));
        }

        { // file has multiple extensions.
            sharg::input_file_validator my_validator{formats};
            EXPECT_NO_THROW(my_validator(tmp_name_multiple.get_path()));
        }

        std::filesystem::path file_in_path;

        // option
        std::string const & path = tmp_name.get_path().string();
        char const * argv[] = {"./parser_test", "-i", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(file_in_path,
                          sharg::config{.short_id = 'i', .validator = sharg::input_file_validator{formats}});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(file_in_path.string(), path);
    }

    { // file list.
        std::vector<std::filesystem::path> input_files;

        // option
        std::string const & path = tmp_name.get_path().string();
        std::string const & path_2 = tmp_name_2.get_path().string();

        char const * argv[] = {"./parser_test", path.c_str(), path_2.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(input_files, sharg::config{.validator = sharg::input_file_validator{formats}});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(input_files.size(), 2u);
        EXPECT_EQ(input_files[0].string(), path);
        EXPECT_EQ(input_files[1].string(), path_2);
    }

    { // get help page message
        std::filesystem::path path;
        char const * argv[] = {"./parser_test", "-h"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(
            path,
            sharg::config{.description = "desc", .validator = sharg::input_file_validator{formats}});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string my_stdout = testing::internal::GetCapturedStdout();
        std::string expected =
            std::string{"test_parser\n"
                        "===========\n"
                        "\n"
                        "POSITIONAL ARGUMENTS\n"
                        "    ARGUMENT-1 (std::filesystem::path)\n"
                        "          desc. The input file must exist and read permissions must be\n"
                        "          granted. Valid file extensions are: [fa, sam, fasta, fasta.txt].\n"
                        "\nOPTIONS\n\n"}
            + basic_options_str + "\n" + basic_version_str;
        EXPECT_EQ(my_stdout, expected);
    }

    { // get help page message (file extensions)
        sharg::input_file_validator validator1{formats};
        EXPECT_EQ(validator1.get_help_page_message(),
                  "The input file must exist and read permissions must be granted. "
                  "Valid file extensions are: [fa, sam, fasta, fasta.txt].");

        sharg::input_file_validator validator2{std::vector<std::string>{}};
        EXPECT_EQ(validator2.get_help_page_message(),
                  "The input file must exist and read permissions must be granted.");
    }
}

TEST(validator_test, output_file)
{
    sharg::test::tmp_filename tmp_name{"testbox.fasta"};
    std::filesystem::path not_existing_path{tmp_name.get_path()};
    sharg::test::tmp_filename tmp_name_2{"testbox_2.fasta"};
    std::ofstream tmp_file_2(tmp_name_2.get_path()); // create file
    std::filesystem::path existing_path{tmp_name_2.get_path()};
    sharg::test::tmp_filename tmp_name_3{"testbox_3.fa"};
    sharg::test::tmp_filename hidden_name{".testbox.fasta"};

    std::vector formats{std::string{"fa"}, std::string{"sam"}, std::string{"fasta"}, std::string{"fasta.txt"}};

    { // single file

        { // file does not exist (& no formats given)
            sharg::output_file_validator my_validator{sharg::output_file_open_options::open_or_create};
            EXPECT_NO_THROW(my_validator(not_existing_path));
            sharg::output_file_validator my_validator2{sharg::output_file_open_options::create_new};
            EXPECT_NO_THROW(my_validator2(not_existing_path));
            sharg::output_file_validator my_validator3{}; // default: create_new
            EXPECT_NO_THROW(my_validator3(not_existing_path));
            sharg::output_file_validator my_validator4{std::vector<std::string>{}}; // empty formats -> no formats
            EXPECT_NO_THROW(my_validator4(not_existing_path));
            sharg::output_file_validator my_validator5{""}; // empty formats -> no formats
            EXPECT_NO_THROW(my_validator5(not_existing_path));
        }

        { // filepath is a directory. Checking writeability would delete content of directory.
            sharg::output_file_validator my_validator{sharg::output_file_open_options::open_or_create};
            EXPECT_THROW(my_validator(std::filesystem::temp_directory_path()), sharg::validation_error);
        }

        { // file does exist & overwriting is prohibited
            sharg::output_file_validator my_validator{sharg::output_file_open_options::create_new, formats};
            EXPECT_THROW(my_validator(existing_path), sharg::validation_error);
        }

        { // file does exist but allow to overwrite it
            sharg::output_file_validator my_validator{sharg::output_file_open_options::open_or_create, formats};
            EXPECT_NO_THROW(my_validator(existing_path));
        }

        { // file has wrong format.
            sharg::output_file_validator my_validator{sharg::output_file_open_options::create_new,
                                                      std::vector{std::string{"sam"}}};
            EXPECT_THROW(my_validator(tmp_name.get_path()), sharg::validation_error);
        }

        { // file has no extension.
            std::filesystem::path no_extension{tmp_name.get_path()};
            no_extension.replace_extension();
            sharg::output_file_validator my_validator{sharg::output_file_open_options::create_new, formats};
            EXPECT_THROW(my_validator(no_extension), sharg::validation_error);
        }

        { // filename is shorter than extension.
            std::filesystem::path filename{tmp_name.get_path()};
            std::vector<std::string> long_extension{"super_duper_long_extension_longer_than_seqan_tmp_filename"};
            sharg::output_file_validator my_validator{sharg::output_file_open_options::create_new, long_extension};
            EXPECT_THROW(my_validator(filename), sharg::validation_error);
        }

        { // filename starts with dot.
            sharg::output_file_validator my_validator{sharg::output_file_open_options::create_new, formats};
            EXPECT_NO_THROW(my_validator(hidden_name.get_path()));
        }

        { // file has multiple extensions.
            std::filesystem::path multiple_extension{tmp_name.get_path()};
            multiple_extension.replace_extension("fasta.txt");
            sharg::output_file_validator my_validator{sharg::output_file_open_options::create_new, formats};
            EXPECT_NO_THROW(my_validator(multiple_extension));
        }

        std::filesystem::path file_out_path;

        // option
        std::string const & path = tmp_name.get_path().string();
        char const * argv[] = {"./parser_test", "-o", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(
            file_out_path,
            sharg::config{.short_id = 'o',
                          .validator =
                              sharg::output_file_validator{sharg::output_file_open_options::create_new, formats}});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(file_out_path.string(), path);
    }

    { // file list.
        std::vector<std::filesystem::path> output_files;

        // option
        std::string const & path = tmp_name.get_path().string();
        std::string const & path_3 = tmp_name_3.get_path().string();

        char const * argv[] = {"./parser_test", path.c_str(), path_3.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(
            output_files,
            sharg::config{.validator =
                              sharg::output_file_validator{sharg::output_file_open_options::create_new, formats}});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(output_files.size(), 2u);
        EXPECT_EQ(output_files[0].string(), path);
        EXPECT_EQ(output_files[1].string(), path_3);
    }

    // get help page message (overwriting prohibited)
    {
        std::filesystem::path path;
        char const * argv[] = {"./parser_test", "-h"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(
            path,
            sharg::config{.description = "desc",
                          .validator =
                              sharg::output_file_validator{sharg::output_file_open_options::create_new, formats}});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string my_stdout = testing::internal::GetCapturedStdout();
        std::string expected =
            std::string{"test_parser\n"
                        "===========\n"
                        "\n"
                        "POSITIONAL ARGUMENTS\n"
                        "    ARGUMENT-1 (std::filesystem::path)\n"
                        "          desc. The output file must not exist already and write permissions\n"
                        "          must be granted. Valid file extensions are: [fa, sam, fasta,\n"
                        "          fasta.txt].\n"
                        "\nOPTIONS\n\n"}
            + basic_options_str + "\n" + basic_version_str;
        EXPECT_EQ(my_stdout, expected);
    }

    // get help page message (overwriting allowed)
    {
        std::filesystem::path path;
        char const * argv[] = {"./parser_test", "-h"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(
            path,
            sharg::config{.description = "desc",
                          .validator =
                              sharg::output_file_validator{sharg::output_file_open_options::open_or_create, formats}});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string my_stdout = testing::internal::GetCapturedStdout();
        std::string expected =
            std::string{"test_parser\n"
                        "===========\n"
                        "\n"
                        "POSITIONAL ARGUMENTS\n"
                        "    ARGUMENT-1 (std::filesystem::path)\n"
                        "          desc. Write permissions must be granted. Valid file extensions are:\n"
                        "          [fa, sam, fasta, fasta.txt].\n"
                        "\nOPTIONS\n\n"}
            + basic_options_str + "\n" + basic_version_str;
        EXPECT_EQ(my_stdout, expected);
    }

    { // get help page message (file extensions)
        sharg::output_file_validator validator1{sharg::output_file_open_options::create_new, formats};
        EXPECT_EQ(validator1.get_help_page_message(),
                  "The output file must not exist already and write permissions "
                  "must be granted. Valid file extensions are: "
                  "[fa, sam, fasta, fasta.txt].");

        sharg::output_file_validator validator2{sharg::output_file_open_options::create_new,
                                                std::vector<std::string>{}};
        EXPECT_EQ(validator2.get_help_page_message(),
                  "The output file must not exist already and write permissions "
                  "must be granted.");
    }

    { // parameter pack constructor - extensions only
        sharg::output_file_validator validator1{"fa", "sam", "fasta", "fasta.txt"};
        sharg::output_file_validator validator2{formats};
        sharg::output_file_validator validator3{sharg::output_file_open_options::create_new, formats};
        EXPECT_EQ(validator1.get_help_page_message(), validator2.get_help_page_message());
        EXPECT_EQ(validator2.get_help_page_message(), validator3.get_help_page_message());
    }

    { // parameter pack constructor - mode + extensions
        sharg::output_file_validator validator1{sharg::output_file_open_options::open_or_create,
                                                "fa",
                                                "sam",
                                                "fasta",
                                                "fasta.txt"};
        sharg::output_file_validator validator2{sharg::output_file_open_options::open_or_create, formats};
        EXPECT_EQ(validator1.get_help_page_message(), validator2.get_help_page_message());
    }
}

TEST(validator_test, input_directory)
{
    sharg::test::tmp_filename tmp_name{"testbox.fasta"};

    {// directory

     {// has filename
      std::ofstream tmp_dir(tmp_name.get_path());
    sharg::input_directory_validator my_validator{};
    EXPECT_THROW(my_validator(tmp_name.get_path()), sharg::validation_error);
}

{ // read directory
    std::filesystem::path p = tmp_name.get_path();
    p.remove_filename();
    std::ofstream tmp_dir(p);
    sharg::input_directory_validator my_validator{};
    my_validator(p);
    EXPECT_NO_THROW(my_validator(p));

    std::filesystem::path dir_in_path;

    // option
    std::string const & path = p.string();
    char const * argv[] = {"./parser_test", "-i", path.c_str()};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_option(dir_in_path, sharg::config{.short_id = 'i', .validator = sharg::input_directory_validator{}});

    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(path, dir_in_path.string());
}
}

{
    // get help page message
    std::filesystem::path path;
    char const * argv[] = {"./parser_test", "-h"};
    sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_positional_option(path,
                                 sharg::config{.description = "desc", .validator = sharg::input_directory_validator{}});

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string my_stdout = testing::internal::GetCapturedStdout();
    std::string expected = std::string{"test_parser\n"
                                       "===========\n"
                                       "\n"
                                       "POSITIONAL ARGUMENTS\n"
                                       "    ARGUMENT-1 (std::filesystem::path)\n"
                                       "          desc. An existing, readable path for the input directory.\n"
                                       "\nOPTIONS\n\n"}
                         + basic_options_str + "\n" + basic_version_str;

    EXPECT_EQ(my_stdout, expected);
}
}

TEST(validator_test, output_directory)
{
    sharg::test::tmp_filename tmp_name{"testbox.fasta"};

    { // read directory
        std::filesystem::path p = tmp_name.get_path();
        p.remove_filename();
        sharg::output_directory_validator my_validator{};
        my_validator(p);
        EXPECT_NO_THROW();

        std::filesystem::path dir_out_path;

        // option
        std::string const & path = p.string();
        char const * argv[] = {"./parser_test", "-o", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(dir_out_path,
                          sharg::config{.short_id = 'o', .validator = sharg::output_directory_validator{}});

        EXPECT_NO_THROW(parser.parse());
        EXPECT_EQ(path, dir_out_path.string());
    }

    { // Parent path exists and is writable.
        sharg::test::tmp_filename tmp_child_name{"dir/child_dir"};
        std::filesystem::path tmp_child_dir{tmp_child_name.get_path()};
        std::filesystem::path tmp_parent_path{tmp_child_dir.parent_path()};

        std::filesystem::create_directory(tmp_parent_path);

        EXPECT_TRUE(std::filesystem::exists(tmp_parent_path));
        EXPECT_NO_THROW(sharg::output_directory_validator{}(tmp_child_dir));
    }

    {
        // get help page message
        std::filesystem::path path;
        char const * argv[] = {"./parser_test", "-h"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(
            path,
            sharg::config{.description = "desc", .validator = sharg::output_directory_validator{}});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string my_stdout = testing::internal::GetCapturedStdout();
        std::string expected = std::string{"test_parser\n"
                                           "===========\n"
                                           "\n"
                                           "POSITIONAL ARGUMENTS\n"
                                           "    ARGUMENT-1 (std::filesystem::path)\n"
                                           "          desc. A valid path for the output directory.\n"
                                           "\nOPTIONS\n\n"}
                             + basic_options_str + "\n" + basic_version_str;

        EXPECT_EQ(my_stdout, expected);
    }
}

TEST(validator_test, inputfile_not_readable)
{
    sharg::test::tmp_filename tmp_name{"my_file.test"};
    std::filesystem::path tmp_file{tmp_name.get_path()};
    std::ofstream str{tmp_name.get_path()};

    EXPECT_NO_THROW(sharg::input_file_validator{}(tmp_file));

    std::filesystem::permissions(tmp_file,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::read_access(tmp_file)) // Do not execute with root permissions.
    {
        EXPECT_THROW(sharg::input_file_validator{}(tmp_file), sharg::validation_error);
    }

    std::filesystem::permissions(tmp_file,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::add);
}

TEST(validator_test, inputfile_not_regular)
{
    sharg::test::tmp_filename tmp{"my_file.test"};
    std::filesystem::path filename = tmp.get_path();
    mkfifo(filename.c_str(), 0644);

    EXPECT_THROW(sharg::input_file_validator{}(filename), sharg::validation_error);
}

TEST(validator_test, inputdir_not_existing)
{
    sharg::test::tmp_filename tmp_name{"dir"};
    std::filesystem::path not_existing_dir{tmp_name.get_path()};

    EXPECT_THROW(sharg::input_directory_validator{}(not_existing_dir), sharg::validation_error);
}

TEST(validator_test, inputdir_not_readable)
{
    sharg::test::tmp_filename tmp_name{"dir"};
    std::filesystem::path tmp_dir{tmp_name.get_path()};

    std::filesystem::create_directory(tmp_dir);

    EXPECT_NO_THROW(sharg::input_directory_validator{}(tmp_dir));

    std::filesystem::permissions(tmp_dir,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::read_access(tmp_dir)) // Do not execute with root permissions.
    {
        EXPECT_THROW(sharg::input_directory_validator{}(tmp_dir), sharg::validation_error);
    }

    std::filesystem::permissions(tmp_dir,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::add);
}

TEST(validator_test, outputfile_not_writable)
{
    sharg::test::tmp_filename tmp_name{"my_file.test"};
    std::filesystem::path tmp_file{tmp_name.get_path()};

    EXPECT_NO_THROW(sharg::output_file_validator{sharg::output_file_open_options::create_new}(tmp_file));

    // Parent path is not writable.
    std::filesystem::permissions(tmp_file.parent_path(),
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::write_access(tmp_file)) // Do not execute with root permissions.
    {
        EXPECT_THROW(sharg::output_file_validator{sharg::output_file_open_options::create_new}(tmp_file),
                     sharg::validation_error);
    }

    // make sure we can remove the directory.
    std::filesystem::permissions(tmp_file.parent_path(),
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::add);
}

TEST(validator_test, outputdir_not_writable)
{
    { // parent dir is not writable.
        sharg::test::tmp_filename tmp_name{"dir"};
        std::filesystem::path tmp_dir{tmp_name.get_path()};

        EXPECT_NO_THROW(sharg::output_file_validator{sharg::output_file_open_options::create_new}(tmp_dir));
        EXPECT_FALSE(std::filesystem::exists(tmp_dir));

        // parent dir does not exist
        sharg::test::tmp_filename tmp_child_name{"dir/child_dir"};
        std::filesystem::path tmp_child_dir{tmp_child_name.get_path()};
        std::filesystem::path tmp_parent_dir{tmp_child_dir.parent_path()};

        EXPECT_THROW(sharg::output_directory_validator{}(tmp_child_dir), sharg::validation_error);

        // Directory exists but is not writable.
        std::filesystem::create_directory(tmp_dir);
        std::filesystem::permissions(tmp_dir,
                                     std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                         | std::filesystem::perms::others_write,
                                     std::filesystem::perm_options::remove);

        EXPECT_TRUE(std::filesystem::exists(tmp_dir));
        if (!sharg::test::write_access(tmp_dir)) // Do not execute with root permissions.
        {
            EXPECT_THROW(sharg::output_directory_validator{}(tmp_dir), sharg::validation_error);
        }

        // Parent path is not writable.
        std::filesystem::permissions(tmp_dir.parent_path(),
                                     std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                         | std::filesystem::perms::others_write,
                                     std::filesystem::perm_options::remove);

        if (!sharg::test::write_access(tmp_dir)) // Do not execute with root permissions.
        {
            EXPECT_THROW(sharg::output_file_validator{sharg::output_file_open_options::create_new}(tmp_dir),
                         sharg::validation_error);
        }

        // make sure we can remove the directories.
        std::filesystem::permissions(tmp_dir,
                                     std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                         | std::filesystem::perms::others_write,
                                     std::filesystem::perm_options::add);
        std::filesystem::permissions(tmp_dir.parent_path(),
                                     std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                         | std::filesystem::perms::others_write,
                                     std::filesystem::perm_options::add);
    }

    { // this dir is not writable
        sharg::test::tmp_filename tmp_name{"dir"};
        std::filesystem::path tmp_dir{tmp_name.get_path()};

        std::filesystem::create_directory(tmp_dir);
        EXPECT_NO_THROW(sharg::output_directory_validator{}(tmp_dir));

        // This path exists but is not writable.
        std::filesystem::permissions(tmp_dir,
                                     std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                         | std::filesystem::perms::others_write,
                                     std::filesystem::perm_options::remove);

        if (!sharg::test::write_access(tmp_dir)) // Do not execute with root permissions.
        {
            EXPECT_THROW(sharg::output_file_validator{sharg::output_file_open_options::create_new}(tmp_dir),
                         sharg::validation_error);
        }

        // make sure we can remove the directory.
        std::filesystem::permissions(tmp_dir,
                                     std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                         | std::filesystem::perms::others_write,
                                     std::filesystem::perm_options::add);
    }
}

TEST(validator_test, arithmetic_range_validator_success)
{
    int option_value{0};
    std::vector<int> option_vector{};

    // option
    char const * argv[] = {"./parser_test", "-i", "10"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_option(option_value,
                      sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, 10);

    // option - negative values
    char const * argv2[] = {"./parser_test", "-i", "-10"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser2, 80);
    parser2.add_option(option_value,
                       sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-20, 20}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, -10);

    // positional option
    char const * argv3[] = {"./parser_test", "10"};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser3, 80);
    parser3.add_positional_option(option_value, sharg::config{.validator = sharg::arithmetic_range_validator{1, 20}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser3.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, 10);

    // positional option - negative values
    char const * argv4[] = {"./parser_test", "--", "-10"};
    sharg::parser parser4{"test_parser", 3, argv4, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser4, 80);
    parser4.add_positional_option(option_value, sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser4.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, -10);

    // option - vector
    char const * argv5[] = {"./parser_test", "-i", "-10", "-i", "48"};
    sharg::parser parser5{"test_parser", 5, argv5, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser5, 80);
    parser5.add_option(option_vector,
                       sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-50, 50}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser5.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_vector[0], -10);
    EXPECT_EQ(option_vector[1], 48);

    // positional option - vector
    option_vector.clear();
    char const * argv6[] = {"./parser_test", "--", "-10", "1"};
    sharg::parser parser6{"test_parser", 4, argv6, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser6, 80);
    parser6.add_positional_option(option_vector,
                                  sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser6.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_vector[0], -10);
    EXPECT_EQ(option_vector[1], 1);

    // get help page message
    option_vector.clear();
    char const * argv7[] = {"./parser_test", "-h"};
    sharg::parser parser7{"test_parser", 2, argv7, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser7, 80);
    parser7.add_positional_option(
        option_vector,
        sharg::config{.description = "desc", .validator = sharg::arithmetic_range_validator{-20, 20}});

    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser7.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string my_stdout = testing::internal::GetCapturedStdout();
    std::string expected = std::string("test_parser\n"
                                       "===========\n"
                                       "\n"
                                       "POSITIONAL ARGUMENTS\n"
                                       "    ARGUMENT-1 (List of signed 32 bit integer)\n"
                                       "          desc Default: []. Value must be in range [-20,20].\n"
                                       "\nOPTIONS\n\n"
                                       + basic_options_str + "\n" + basic_version_str);
    EXPECT_EQ(my_stdout, expected);

    // option - double value
    double double_option_value;
    char const * argv8[] = {"./parser_test", "-i", "10.9"};
    sharg::parser parser8{"test_parser", 3, argv8, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser8, 80);
    parser8.add_option(double_option_value,
                       sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser8.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_FLOAT_EQ(double_option_value, 10.9);
}

TEST(validator_test, arithmetic_range_validator_error)
{
    int option_value;
    std::vector<int> option_vector;

    // option - above max
    char const * argv[] = {"./parser_test", "-i", "30"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_option(option_value,
                      sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});

    EXPECT_THROW(parser.parse(), sharg::validation_error);

    // option - below min
    char const * argv2[] = {"./parser_test", "-i", "-21"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser2, 80);
    parser2.add_option(option_value,
                       sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-20, 20}});

    EXPECT_THROW(parser2.parse(), sharg::validation_error);

    // positional option - above max
    char const * argv3[] = {"./parser_test", "30"};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser3, 80);
    parser3.add_positional_option(option_value, sharg::config{.validator = sharg::arithmetic_range_validator{1, 20}});

    EXPECT_THROW(parser3.parse(), sharg::validation_error);

    // positional option - below min
    char const * argv4[] = {"./parser_test", "--", "-21"};
    sharg::parser parser4{"test_parser", 3, argv4, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser4, 80);
    parser4.add_positional_option(option_value, sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});

    EXPECT_THROW(parser4.parse(), sharg::validation_error);

    // option - vector
    char const * argv5[] = {"./parser_test", "-i", "-100"};
    sharg::parser parser5{"test_parser", 3, argv5, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser5, 80);
    parser5.add_option(option_vector,
                       sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-50, 50}});

    EXPECT_THROW(parser5.parse(), sharg::validation_error);

    // positional option - vector
    option_vector.clear();
    char const * argv6[] = {"./parser_test", "--", "-10", "100"};
    sharg::parser parser6{"test_parser", 4, argv6, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser6, 80);
    parser6.add_positional_option(option_vector,
                                  sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});

    EXPECT_THROW(parser6.parse(), sharg::validation_error);

    // option - double value
    double double_option_value;
    char const * argv7[] = {"./parser_test", "-i", "0.9"};
    sharg::parser parser7{"test_parser", 3, argv7, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser7, 80);
    parser7.add_option(double_option_value,
                       sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});

    EXPECT_THROW(parser7.parse(), sharg::validation_error);
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

TEST(validator_test, value_list_validator_success)
{
    // type deduction
    // --------------
    // all arithmetic types are deduced to their common type in order to easily allow chaining of arithmetic validators
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<int>, decltype(sharg::value_list_validator{1})>));
    // except char
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<char>, decltype(sharg::value_list_validator{'c'})>));
    // The same holds for a range of arithmetic types
    std::vector v{1, 2, 3};
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<int>, decltype(sharg::value_list_validator{v})>));
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<int>,
                              decltype(sharg::value_list_validator{v | std::views::take(2)})>));
    std::vector v_char{'1', '2', '3'};
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<char>, decltype(sharg::value_list_validator{v_char})>));
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<char>,
                              decltype(sharg::value_list_validator{v_char | std::views::take(2)})>));
    // const char * is deduced to std::string
    std::vector v2{"ha", "ba", "ma"};
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<std::string>, decltype(sharg::value_list_validator{"ha"})>));
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<std::string>,
                              decltype(sharg::value_list_validator{"ha", "ba", "ma"})>));
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<std::string>, decltype(sharg::value_list_validator{v2})>));
    EXPECT_TRUE((std::same_as<sharg::value_list_validator<std::string>,
                              decltype(sharg::value_list_validator{v2 | std::views::take(2)})>));
    // custom types are used as is
    EXPECT_TRUE(
        (std::same_as<sharg::value_list_validator<foo>, decltype(sharg::value_list_validator{foo::one, foo::two})>));

    // usage
    // -----
    std::string option_value;
    int option_value_int;
    std::vector<std::string> option_vector;
    std::vector<int> option_vector_int;

    // option
    std::vector<std::string> valid_str_values{"ha", "ba", "ma"};
    char const * argv[] = {"./parser_test", "-s", "ba"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_option(option_value,
                      sharg::config{.short_id = 's',
                                    .validator = sharg::value_list_validator{valid_str_values | std::views::take(2)}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "ba");

    // option with integers
    char const * argv2[] = {"./parser_test", "-i", "-21"};
    sharg::parser parser2{"test_parser", 3, argv2, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser2, 80);
    parser2.add_option(option_value_int,
                       sharg::config{.short_id = 'i', .validator = sharg::value_list_validator<int>{0, -21, 10}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value_int, -21);

    // positional option
    char const * argv3[] = {"./parser_test", "ma"};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser3, 80);
    parser3.add_positional_option(option_value,
                                  sharg::config{.validator = sharg::value_list_validator{valid_str_values}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser3.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "ma");

    // positional option - vector
    char const * argv4[] = {"./parser_test", "ha", "ma"};
    sharg::parser parser4{"test_parser", 3, argv4, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser4, 80);
    parser4.add_positional_option(option_vector,
                                  sharg::config{.validator = sharg::value_list_validator{"ha", "ba", "ma"}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser4.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_vector[0], "ha");
    EXPECT_EQ(option_vector[1], "ma");

    // option - vector
    char const * argv5[] = {"./parser_test", "-i", "-10", "-i", "48"};
    sharg::parser parser5{"test_parser", 5, argv5, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser5, 80);
    parser5.add_option(option_vector_int,
                       sharg::config{.short_id = 'i', .validator = sharg::value_list_validator<int>{-10, 48, 50}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser5.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_vector_int[0], -10);
    EXPECT_EQ(option_vector_int[1], 48);

    // get help page message
    option_vector_int.clear();
    char const * argv7[] = {"./parser_test", "-h"};
    sharg::parser parser7{"test_parser", 2, argv7, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser7, 80);
    parser7.add_option(option_vector_int,
                       sharg::config{.short_id = 'i',
                                     .long_id = "int-option",
                                     .description = "desc",
                                     .validator = sharg::value_list_validator<int>{-10, 48, 50}});

    option_vector_int.clear();
    testing::internal::CaptureStdout();
    EXPECT_EXIT(parser7.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
    std::string my_stdout = testing::internal::GetCapturedStdout();
    std::string expected = std::string("test_parser\n"
                                       "===========\n"
                                       "\nOPTIONS\n"
                                       "    -i, --int-option (List of signed 32 bit integer)\n"
                                       "          desc Default: []. Value must be one of [-10, 48, 50].\n\n"
                                       + basic_options_str + "\n" + basic_version_str);
    EXPECT_EQ(my_stdout, expected);
}

TEST(validator_test, value_list_validator_error)
{
    std::string option_value;
    int option_value_int;
    std::vector<std::string> option_vector;
    std::vector<int> option_vector_int;

    // option
    char const * argv[] = {"./parser_test", "-s", "sa"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_option(option_value,
                      sharg::config{.short_id = 's', .validator = sharg::value_list_validator{"ha", "ba", "ma"}});

    EXPECT_THROW(parser.parse(), sharg::validation_error);

    // positional option
    char const * argv3[] = {"./parser_test", "30"};
    sharg::parser parser3{"test_parser", 2, argv3, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser3, 80);
    parser3.add_positional_option(option_value_int, sharg::config{.validator = sharg::value_list_validator{0, 5, 10}});

    EXPECT_THROW(parser3.parse(), sharg::validation_error);

    // positional option - vector
    char const * argv4[] = {"./parser_test", "fo", "ma"};
    sharg::parser parser4{"test_parser", 3, argv4, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser4, 80);
    parser4.add_positional_option(option_vector,
                                  sharg::config{.validator = sharg::value_list_validator{"ha", "ba", "ma"}});

    EXPECT_THROW(parser4.parse(), sharg::validation_error);

    // option - vector
    char const * argv5[] = {"./parser_test", "-i", "-10", "-i", "488"};
    sharg::parser parser5{"test_parser", 5, argv5, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser5, 80);
    parser5.add_option(option_vector_int,
                       sharg::config{.short_id = 'i', .validator = sharg::value_list_validator<int>{-10, 48, 50}});

    EXPECT_THROW(parser5.parse(), sharg::validation_error);
}

// https://github.com/seqan/sharg-parser/issues/178
TEST(validator_test, value_list_validator_issue178)
{
    std::filesystem::path option_value;
    std::vector<std::filesystem::path> option_vector;
    constexpr std::array valid_values{"ha", "ba", "ma"};

    // option
    char const * argv[] = {"./parser_test", "-s", "ba"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_option(option_value,
                      sharg::config{.short_id = 's', .validator = sharg::value_list_validator{valid_values}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_value, "ba");

    // option - vector
    char const * argv2[] = {"./parser_test", "-s", "ha", "-s", "ba"};
    sharg::parser parser2{"test_parser", 5, argv2, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser2, 80);
    parser2.add_option(option_vector,
                       sharg::config{.short_id = 's', .validator = sharg::value_list_validator{valid_values}});

    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser2.parse());
    EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
    EXPECT_EQ(option_vector[0], "ha");
    EXPECT_EQ(option_vector[1], "ba");
}

TEST(validator_test, regex_validator_success)
{
    std::string option_value;
    std::vector<std::string> option_vector;
    sharg::regex_validator email_validator("[a-zA-Z]+@[a-zA-Z]+\\.com");
    sharg::regex_validator email_vector_validator("[a-zA-Z]+@[a-zA-Z]+\\.com");

    { // option
        char const * argv[] = {"./parser_test", "-s", "ballo@rollo.com"};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(option_value, sharg::config{.short_id = 's', .validator = email_validator});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_value, "ballo@rollo.com");
    }

    { // positional option
        char const * argv[] = {"./parser_test", "chr1"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(option_value, sharg::config{.validator = sharg::regex_validator{"^chr[0-9]+"}});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_value, "chr1");
    }

    { // positional option - vector
        char const * argv[] = {"./parser_test", "rollo", "bollo", "lollo"};
        sharg::parser parser{"test_parser", 4, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_positional_option(option_vector, sharg::config{.validator = sharg::regex_validator{".*oll.*"}});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_vector[0], "rollo");
        EXPECT_EQ(option_vector[1], "bollo");
        EXPECT_EQ(option_vector[2], "lollo");
    }

    { // option - vector
        option_vector.clear();
        char const * argv[] = {"./parser_test", "-s", "rita@rambo.com", "-s", "tina@rambo.com"};
        sharg::parser parser{"test_parser", 5, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(option_vector, sharg::config{.short_id = 's', .validator = email_vector_validator});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_vector[0], "rita@rambo.com");
        EXPECT_EQ(option_vector[1], "tina@rambo.com");
    }

    { // option - std::filesystem::path
        std::filesystem::path path_option;
        char const * argv[] = {"./parser_test", "-s", "rita@rambo.com"};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(path_option, sharg::config{.short_id = 's', .validator = email_vector_validator});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(path_option, "rita@rambo.com");
    }

    { // get help page message
        option_vector.clear();
        char const * argv[] = {"./parser_test", "-h"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(option_vector,
                          sharg::config{.short_id = 's',
                                        .long_id = "string-option",
                                        .description = "desc",
                                        .validator = email_vector_validator});

        option_vector.clear();
        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string my_stdout = testing::internal::GetCapturedStdout();
        std::string expected = std::string("test_parser\n"
                                           "===========\n"
                                           "\nOPTIONS\n"
                                           "    -s, --string-option (List of std::string)\n"
                                           "          desc Default: []. Value must match the pattern\n"
                                           "          '[a-zA-Z]+@[a-zA-Z]+\\.com'.\n"
                                           "\n"
                                           + basic_options_str + "\n" + basic_version_str);
        EXPECT_EQ(my_stdout, expected);
    }
}

TEST(validator_test, regex_validator_error)
{
    std::string option_value;
    std::vector<std::string> option_vector;

    // option
    char const * argv[] = {"./parser_test", "--string-option", "sally"};
    sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser, 80);
    parser.add_option(option_value,
                      sharg::config{.long_id = "string-option", .validator = sharg::regex_validator{"tt"}});

    EXPECT_THROW(parser.parse(), sharg::validation_error);

    // positional option
    char const * argv2[] = {"./parser_test", "jessy"};
    sharg::parser parser2{"test_parser", 2, argv2, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser2, 80);
    parser2.add_positional_option(option_value, sharg::config{.validator = sharg::regex_validator{"[0-9]"}});

    EXPECT_THROW(parser2.parse(), sharg::validation_error);

    // positional option - vector
    char const * argv3[] = {"./parser_test", "rollo", "bttllo", "lollo"};
    sharg::parser parser3{"test_parser", 4, argv3, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser3, 80);
    parser3.add_positional_option(option_vector, sharg::config{.validator = sharg::regex_validator{".*oll.*"}});

    EXPECT_THROW(parser3.parse(), sharg::validation_error);

    // option - vector
    option_vector.clear();
    char const * argv4[] = {"./parser_test", "-s", "gh", "-s", "tt"};
    sharg::parser parser4{"test_parser", 5, argv4, sharg::update_notifications::off};
    sharg::detail::test_accessor::set_terminal_width(parser4, 80);
    parser4.add_option(option_vector, sharg::config{.short_id = 's', .validator = sharg::regex_validator{"tt"}});

    EXPECT_THROW(parser4.parse(), sharg::validation_error);
}

TEST(validator_test, chaining_validators_common_type)
{
    // chaining integral options stay integral
    {
        int max_int = std::numeric_limits<int>::max();
        std::vector v_int{1, 2, 3, max_int};
        std::vector v_unsigned{4u, static_cast<unsigned>(max_int)};

        EXPECT_TRUE((std::same_as<std::vector<int>, decltype(v_int)>));
        EXPECT_TRUE((std::same_as<std::vector<unsigned>, decltype(v_unsigned)>));

        sharg::value_list_validator validator_int{v_int};
        sharg::value_list_validator validator_unsigned{v_unsigned};

        EXPECT_TRUE((std::same_as<sharg::value_list_validator<int>, decltype(validator_int)>));
        EXPECT_TRUE((std::same_as<int, decltype(validator_int)::option_value_type>));

        EXPECT_TRUE((std::same_as<sharg::value_list_validator<unsigned>, decltype(validator_unsigned)>));
        EXPECT_TRUE((std::same_as<unsigned, decltype(validator_unsigned)::option_value_type>));

        auto validator = validator_int | validator_unsigned;

        EXPECT_TRUE((std::same_as<unsigned, std::common_type_t<int, unsigned>>));
        EXPECT_TRUE((std::same_as<unsigned, decltype(validator)::option_value_type>));

        // max_int is part of both validators
        EXPECT_NO_THROW(validator_int(max_int));
        EXPECT_NO_THROW(validator_unsigned(max_int));
        EXPECT_NO_THROW(validator(max_int));
    }

    // chaining mixed arithmetic options will be highest common arithmetic type
    {
        // note: this number is not representable by double and multiple integer values represent the same double value
        int64_t max_int64 = std::numeric_limits<int64_t>::max();
        EXPECT_EQ(static_cast<double>(max_int64), static_cast<double>(max_int64 - 1));

        std::vector<int64_t> v_int64{1, 2, 3, max_int64};
        std::vector<uint64_t> v_uint64{4u, static_cast<uint64_t>(max_int64)};
        std::vector<double> v_double{4.0, static_cast<double>(max_int64)};

        sharg::value_list_validator validator_int64{v_int64};
        sharg::value_list_validator validator_uint64{v_uint64};
        sharg::value_list_validator validator_double{v_double};

        EXPECT_TRUE((std::same_as<sharg::value_list_validator<int64_t>, decltype(validator_int64)>));
        EXPECT_TRUE((std::same_as<int64_t, decltype(validator_int64)::option_value_type>));

        EXPECT_TRUE((std::same_as<sharg::value_list_validator<uint64_t>, decltype(validator_uint64)>));
        EXPECT_TRUE((std::same_as<uint64_t, decltype(validator_uint64)::option_value_type>));

        EXPECT_TRUE((std::same_as<sharg::value_list_validator<double>, decltype(validator_double)>));
        EXPECT_TRUE((std::same_as<double, decltype(validator_double)::option_value_type>));

        auto validator = validator_int64 | validator_uint64 | validator_double;

        EXPECT_TRUE((std::same_as<double, std::common_type_t<int64_t, uint64_t, double>>));
        EXPECT_TRUE((std::same_as<double, decltype(validator)::option_value_type>));

        // max_int64 is an exact match for the two integral validators
        // note: double will decay the integer to a smaller double value,
        //       but this is consistent, because it is the same given value
        // note: chained validator passes the value as it is through,
        //       so validator_[u]int64 will be called with the integer value
        EXPECT_NO_THROW(validator_int64(max_int64));
        EXPECT_NO_THROW(validator_uint64(max_int64));
        EXPECT_NO_THROW(validator_double(max_int64));
        EXPECT_NO_THROW(validator(max_int64));

        // integers have exact match
        // note: double accepts that value, even though it is not within that list.
        EXPECT_THROW(validator_int64(max_int64 - 1), sharg::validation_error);
        EXPECT_THROW(validator_uint64(max_int64 - 1), sharg::validation_error);
        EXPECT_NO_THROW(validator_double(max_int64 - 1));
        EXPECT_THROW(validator(max_int64 - 1), sharg::validation_error);
    }
}

TEST(validator_test, chaining_validators)
{
    std::string option_value{};
    std::vector<std::string> option_vector{};
    sharg::regex_validator absolute_path_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"};
    sharg::output_file_validator my_file_ext_validator{sharg::output_file_open_options::create_new, {"sa", "so"}};

    sharg::test::tmp_filename tmp_name{"file.sa"};
    std::filesystem::path invalid_extension{tmp_name.get_path()};
    invalid_extension.replace_extension(".invalid");

    // option
    {
        std::string const & path = tmp_name.get_path().string();
        char const * argv[] = {"./parser_test", "-s", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(option_value,
                          sharg::config{.short_id = 's', .validator = absolute_path_validator | my_file_ext_validator});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_value, path);
    }

    {
        auto rel_path = tmp_name.get_path().relative_path().string();
        char const * argv[] = {"./parser_test", "-s", rel_path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(option_value,
                          sharg::config{.short_id = 's', .validator = absolute_path_validator | my_file_ext_validator});

        EXPECT_THROW(parser.parse(), sharg::validation_error);
    }

    {
        std::string const & path = invalid_extension.string();
        char const * argv[] = {"./parser_test", "-s", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(option_value,
                          sharg::config{.short_id = 's', .validator = absolute_path_validator | my_file_ext_validator});

        EXPECT_THROW(parser.parse(), sharg::validation_error);
    }

    // with temporary validators
    {
        std::string const & path = tmp_name.get_path().string();
        char const * argv[] = {"./parser_test", "-s", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(
            option_value,
            sharg::config{
                .short_id = 's',
                .validator = sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                           | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_value, path);
    }

    // three validators
    {
        std::string const & path = tmp_name.get_path().string();
        char const * argv[] = {"./parser_test", "-s", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(
            option_value,
            sharg::config{.short_id = 's',
                          .validator =
                              sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                              | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}
                              | sharg::regex_validator{".*"}});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_value, path);
    }

    // help page message
    {
        option_value.clear();
        char const * argv[] = {"./parser_test", "-h"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(
            option_value,
            sharg::config{.short_id = 's',
                          .long_id = "string-option",
                          .description = "desc",
                          .validator =
                              sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                              | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}
                              | sharg::regex_validator{".*"}});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string my_stdout = testing::internal::GetCapturedStdout();
        std::string expected =
            std::string{"test_parser\n"
                        "===========\n"
                        "\nOPTIONS\n"
                        "    -s, --string-option (std::string)\n"
                        "          desc Default: \"\". Value must match the pattern\n"
                        "          '(/[^/]+)+/.*\\.[^/\\.]+$'. The output file must not exist already and\n"
                        "          write permissions must be granted. Valid file extensions are: [sa,\n"
                        "          so]. Value must match the pattern '.*'.\n"
                        "\n"}
            + basic_options_str + "\n" + basic_version_str;
        EXPECT_EQ(my_stdout, expected);
    }

    // help page message (allow overwriting)
    {
        option_value.clear();
        char const * argv[] = {"./parser_test", "-h"};
        sharg::parser parser{"test_parser", 2, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(
            option_value,
            sharg::config{
                .short_id = 's',
                .long_id = "string-option",
                .description = "desc",
                .validator = sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                           | sharg::output_file_validator{sharg::output_file_open_options::open_or_create, {"sa", "so"}}
                           | sharg::regex_validator{".*"}});

        testing::internal::CaptureStdout();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        std::string my_stdout = testing::internal::GetCapturedStdout();
        std::string expected =
            std::string{"test_parser\n"
                        "===========\n"
                        "\nOPTIONS\n"
                        "    -s, --string-option (std::string)\n"
                        "          desc Default: \"\". Value must match the pattern\n"
                        "          '(/[^/]+)+/.*\\.[^/\\.]+$'. Write permissions must be granted. Valid\n"
                        "          file extensions are: [sa, so]. Value must match the pattern '.*'.\n"
                        "\n"}
            + basic_options_str + "\n" + basic_version_str;
        EXPECT_EQ(my_stdout, expected);
    }

    // chaining with a container option value type
    {
        std::vector<std::string> option_list_value{};
        std::string const & path = tmp_name.get_path().string();
        char const * argv[] = {"./parser_test", "-s", path.c_str()};
        sharg::parser parser{"test_parser", 3, argv, sharg::update_notifications::off};
        sharg::detail::test_accessor::set_terminal_width(parser, 80);
        parser.add_option(
            option_list_value,
            sharg::config{
                .short_id = 's',
                .validator = sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                           | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}});

        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        EXPECT_TRUE((testing::internal::GetCapturedStderr()).empty());
        EXPECT_EQ(option_list_value[0], path);
    }
}
