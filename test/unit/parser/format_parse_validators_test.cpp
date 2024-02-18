// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <ranges>

#include <sharg/parser.hpp>
#include <sharg/test/file_access.hpp>
#include <sharg/test/test_fixture.hpp>
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

class validator_test : public sharg::test::test_fixture
{};

// clang-format off
// First argument cannot have comma, the second one can.
#define EXPECT_SAME_TYPE(a, b, ...) EXPECT_TRUE((std::same_as<a, b __VA_OPT__(,) __VA_ARGS__>))
// clang-format on

TEST_F(validator_test, fullfill_concept)
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

TEST_F(validator_test, input_file)
{
    sharg::test::tmp_filename const tmp_name{"testbox.fasta"};
    sharg::test::tmp_filename const tmp_name_2{"testbox_2.fasta"};
    sharg::test::tmp_filename const tmp_name_hidden{".testbox.fasta"};
    sharg::test::tmp_filename const tmp_name_multiple{"testbox.fasta.txt"};

    std::filesystem::path const tmp_path{tmp_name.get_path()};
    std::filesystem::path const tmp_path_2{tmp_name_2.get_path()};
    std::filesystem::path const tmp_path_hidden{tmp_name_hidden.get_path()};
    std::filesystem::path const tmp_path_multiple{tmp_name_multiple.get_path()};

    { // Open fstream to create files.
        std::ofstream tmp_file{tmp_path};
        std::ofstream tmp_file_2{tmp_path_2};
        std::ofstream tmp_file_hidden{tmp_path_hidden};
        std::ofstream tmp_file_multiple{tmp_path_multiple};
    }

    std::vector<std::string> const formats{"fa", "sam", "fasta", "fasta.txt"};
    sharg::input_file_validator my_validator{formats};

    // filename starts with dot.
    EXPECT_NO_THROW(my_validator(tmp_path_hidden));

    // file has multiple extensions.
    EXPECT_NO_THROW(my_validator(tmp_path_multiple));

    // file does not exist.
    std::filesystem::path test_path = tmp_path;
    EXPECT_THROW(my_validator(test_path.replace_extension(".bam")), sharg::validation_error);

    // file has no extension.
    EXPECT_THROW(my_validator(test_path.replace_extension()), sharg::validation_error);

    // file has wrong format.
    my_validator = sharg::input_file_validator{std::vector{std::string{"sam"}}};
    EXPECT_THROW(my_validator(tmp_path), sharg::validation_error);

    // empty list of formats.
    my_validator = sharg::input_file_validator{};
    EXPECT_NO_THROW(my_validator(tmp_path));

    // Parser with option.
    my_validator = sharg::input_file_validator{formats};
    std::filesystem::path path_in{};
    std::string const tmp_path_string = tmp_path.string();

    auto parser = get_parser("-i", tmp_path_string);
    parser.add_option(path_in, sharg::config{.short_id = 'i', .validator = my_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(path_in.string(), tmp_path_string);

    // Parser with list option.
    std::vector<std::filesystem::path> input_files{};
    std::string const tmp_path_2_string = tmp_path_2.string();

    parser = get_parser(tmp_path_string, tmp_path_2_string);
    parser.add_positional_option(input_files, sharg::config{.validator = my_validator});

    EXPECT_NO_THROW(parser.parse());
    ASSERT_EQ(input_files.size(), 2u);
    EXPECT_EQ(input_files[0], tmp_path);
    EXPECT_EQ(input_files[1], tmp_path_2);

    // get help page message
    parser = get_parser("-h");
    parser.add_positional_option(path_in, sharg::config{.description = "desc", .validator = my_validator});
    std::string expected = std::string{"test_parser\n"
                                       "===========\n"
                                       "\n"
                                       "POSITIONAL ARGUMENTS\n"
                                       "    ARGUMENT-1 (std::filesystem::path)\n"
                                       "          desc. The input file must exist and read permissions must be\n"
                                       "          granted. Valid file extensions are: [fa, sam, fasta, fasta.txt].\n"
                                       "\nOPTIONS\n\n"}
                         + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // get help page message (file extensions)
    my_validator = sharg::input_file_validator{formats};
    EXPECT_EQ(my_validator.get_help_page_message(),
              "The input file must exist and read permissions must be granted. "
              "Valid file extensions are: [fa, sam, fasta, fasta.txt].");

    my_validator = sharg::input_file_validator{};
    EXPECT_EQ(my_validator.get_help_page_message(), "The input file must exist and read permissions must be granted.");
}

TEST_F(validator_test, output_file)
{
    sharg::test::tmp_filename const tmp_name{"testbox.fasta"};
    sharg::test::tmp_filename const tmp_name_2{"testbox_2.fasta"};
    sharg::test::tmp_filename const tmp_name_3{"testbox_3.fa"};
    sharg::test::tmp_filename const hidden_name{".testbox.fasta"};

    std::filesystem::path const not_existing_path{tmp_name.get_path()};
    std::filesystem::path const existing_path{tmp_name_2.get_path()};

    { // Open fstream to create files.
        std::ofstream tmp_file{existing_path};
    }

    using options = sharg::output_file_open_options;

    std::vector const formats{std::string{"fa"}, std::string{"sam"}, std::string{"fasta"}, std::string{"fasta.txt"}};
    sharg::output_file_validator my_validator{/* options::create_new */};

    // file does not exist (& no formats given)
    EXPECT_NO_THROW(my_validator(not_existing_path));

    my_validator = sharg::output_file_validator{options::open_or_create};
    EXPECT_NO_THROW(my_validator(not_existing_path));

    my_validator = sharg::output_file_validator{options::create_new};
    EXPECT_NO_THROW(my_validator(not_existing_path));

    my_validator = sharg::output_file_validator{std::vector<std::string>{}}; // Empty formats -> no formats
    EXPECT_NO_THROW(my_validator(not_existing_path));

    my_validator = sharg::output_file_validator{""}; // Empty formats
    EXPECT_NO_THROW(my_validator(not_existing_path));

    // filepath is a directory. Checking writeability would delete content of directory.
    my_validator = sharg::output_file_validator{options::open_or_create};
    EXPECT_THROW(my_validator(std::filesystem::temp_directory_path()), sharg::validation_error);

    // file has wrong format.
    my_validator = sharg::output_file_validator{options::create_new, std::vector{std::string{"sam"}}};
    EXPECT_THROW(my_validator(existing_path), sharg::validation_error);

    // file does exist & overwriting is prohibited
    my_validator = sharg::output_file_validator{options::create_new, formats};
    EXPECT_THROW(my_validator(existing_path), sharg::validation_error);

    // file does exist but allow to overwrite it
    my_validator = sharg::output_file_validator{options::open_or_create, formats};
    EXPECT_TRUE(std::filesystem::exists(existing_path)); // Todo: Expected?
    EXPECT_NO_THROW(my_validator(existing_path));
    EXPECT_FALSE(std::filesystem::exists(existing_path)); // Todo: Expected?

    // file has no extension.
    std::filesystem::path test_path = not_existing_path;
    EXPECT_THROW(my_validator(test_path.replace_extension()), sharg::validation_error);

    // filename is shorter than extension.
    std::string long_extension = not_existing_path.string() + ".longer.than.path";
    my_validator = sharg::output_file_validator{options::create_new, std::vector{long_extension}};
    EXPECT_THROW(my_validator(not_existing_path), sharg::validation_error);

    long_extension = existing_path.string() + ".longer.than.path";
    my_validator = sharg::output_file_validator{options::create_new, std::vector{long_extension}};
    EXPECT_THROW(my_validator(existing_path), sharg::validation_error);

    // filename starts with dot.
    my_validator = sharg::output_file_validator{options::create_new, formats};
    EXPECT_NO_THROW(my_validator(hidden_name.get_path()));

    // file has multiple extensions.
    test_path = not_existing_path;
    test_path.replace_extension("fasta.txt");
    EXPECT_NO_THROW(my_validator(test_path));

    // Parser with option.
    std::filesystem::path path_out{};
    std::string const not_existing_path_string = not_existing_path.string();

    auto parser = get_parser("-o", not_existing_path_string);
    parser.add_option(path_out, sharg::config{.short_id = 'o', .validator = my_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(path_out.string(), not_existing_path_string);

    // Parser with list option.
    std::vector<std::filesystem::path> output_files{};
    std::string const not_existing_path_string_2 = tmp_name_3.get_path().string();

    parser = get_parser(not_existing_path_string, not_existing_path_string_2);
    parser.add_positional_option(output_files, sharg::config{.validator = my_validator});

    EXPECT_NO_THROW(parser.parse());
    ASSERT_EQ(output_files.size(), 2u);
    EXPECT_EQ(output_files[0], not_existing_path);
    EXPECT_EQ(output_files[1], tmp_name_3.get_path());

    // get help page message (overwriting prohibited)
    parser = get_parser("-h");
    parser.add_positional_option(path_out, sharg::config{.description = "desc", .validator = my_validator});
    std::string expected = std::string{"test_parser\n"
                                       "===========\n"
                                       "\n"
                                       "POSITIONAL ARGUMENTS\n"
                                       "    ARGUMENT-1 (std::filesystem::path)\n"
                                       "          desc. The output file must not exist already and write permissions\n"
                                       "          must be granted. Valid file extensions are: [fa, sam, fasta,\n"
                                       "          fasta.txt].\n"
                                       "\nOPTIONS\n\n"}
                         + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // get help page message (overwriting allowed)
    my_validator = sharg::output_file_validator{options::open_or_create, formats};
    parser = get_parser("-h");
    parser.add_positional_option(path_out, sharg::config{.description = "desc", .validator = my_validator});
    expected = std::string{"test_parser\n"
                           "===========\n"
                           "\n"
                           "POSITIONAL ARGUMENTS\n"
                           "    ARGUMENT-1 (std::filesystem::path)\n"
                           "          desc. Write permissions must be granted. Valid file extensions are:\n"
                           "          [fa, sam, fasta, fasta.txt].\n"
                           "\nOPTIONS\n\n"}
             + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // get help page message (file extensions)
    my_validator = sharg::output_file_validator{options::create_new, formats};
    EXPECT_EQ(my_validator.get_help_page_message(),
              "The output file must not exist already and write permissions "
              "must be granted. Valid file extensions are: "
              "[fa, sam, fasta, fasta.txt].");

    my_validator = sharg::output_file_validator{options::create_new, std::vector<std::string>{}};
    EXPECT_EQ(my_validator.get_help_page_message(),
              "The output file must not exist already and write permissions "
              "must be granted.");

    // parameter pack constructor - extensions only
    my_validator = sharg::output_file_validator{options::create_new, formats};
    sharg::output_file_validator validator1{"fa", "sam", "fasta", "fasta.txt"};
    sharg::output_file_validator validator2{formats};
    EXPECT_EQ(validator1.get_help_page_message(), validator2.get_help_page_message());
    EXPECT_EQ(validator2.get_help_page_message(), my_validator.get_help_page_message());

    // parameter pack constructor - mode + extensions
    validator1 = sharg::output_file_validator{options::open_or_create, "fa", "sam", "fasta", "fasta.txt"};
    validator2 = sharg::output_file_validator{options::open_or_create, formats};
    EXPECT_EQ(validator1.get_help_page_message(), validator2.get_help_page_message());
}

TEST_F(validator_test, input_directory)
{
    sharg::test::tmp_filename const tmp_name{"testbox.fasta"};
    std::filesystem::path const tmp_path{tmp_name.get_path()};

    { // Open fstream to create files.
        std::ofstream tmp_file{tmp_path};
    }

    sharg::input_directory_validator my_validator{};

    // Input is a file.
    EXPECT_THROW(my_validator(tmp_path), sharg::validation_error);

    // Input is a directory.
    std::filesystem::path test_path = tmp_path;
    test_path.remove_filename();
    EXPECT_NO_THROW(my_validator(test_path));

    // Parser with option.
    std::filesystem::path path_in{};
    std::string const test_path_string = test_path.string();

    auto parser = get_parser("-i", test_path_string);
    parser.add_option(path_in, sharg::config{.short_id = 'i', .validator = my_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(path_in.string(), test_path_string);

    // get help page message
    parser = get_parser("-h");
    parser.add_positional_option(path_in, sharg::config{.description = "desc", .validator = my_validator});
    std::string expected = std::string{"test_parser\n"
                                       "===========\n"
                                       "\n"
                                       "POSITIONAL ARGUMENTS\n"
                                       "    ARGUMENT-1 (std::filesystem::path)\n"
                                       "          desc. An existing, readable path for the input directory.\n"
                                       "\nOPTIONS\n\n"}
                         + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(validator_test, output_directory)
{
    sharg::test::tmp_filename const tmp_name{"testbox.fasta"};
    std::filesystem::path const tmp_path{tmp_name.get_path()};

    { // Open fstream to create files.
        std::ofstream tmp_file{tmp_path};
    }

    sharg::output_directory_validator my_validator{};

    // Output is a file.
    EXPECT_THROW(my_validator(tmp_path), sharg::validation_error);

    // Output is a directory.
    std::filesystem::path test_path = tmp_path;
    test_path.remove_filename();
    EXPECT_NO_THROW(my_validator(test_path));

    // Parser with option.
    std::filesystem::path path_out{};
    std::string const test_path_string = test_path.string();

    auto parser = get_parser("-o", test_path_string);
    parser.add_option(path_out, sharg::config{.short_id = 'o', .validator = my_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(path_out.string(), test_path_string);

    // Parent path exists and is writable.
    sharg::test::tmp_filename tmp_child_name{"dir/child_dir"};
    std::filesystem::path tmp_child_dir{tmp_child_name.get_path()};
    std::filesystem::path tmp_parent_path{tmp_child_dir.parent_path()};

    EXPECT_FALSE(std::filesystem::exists(tmp_parent_path));
    EXPECT_THROW(my_validator(tmp_child_dir), sharg::validation_error);

    std::filesystem::create_directory(tmp_parent_path);
    EXPECT_TRUE(std::filesystem::exists(tmp_parent_path));
    EXPECT_NO_THROW(my_validator(tmp_child_dir));

    // get help page message
    parser = get_parser("-h");
    parser.add_positional_option(path_out, sharg::config{.description = "desc", .validator = my_validator});
    std::string expected = std::string{"test_parser\n"
                                       "===========\n"
                                       "\n"
                                       "POSITIONAL ARGUMENTS\n"
                                       "    ARGUMENT-1 (std::filesystem::path)\n"
                                       "          desc. A valid path for the output directory.\n"
                                       "\nOPTIONS\n\n"}
                         + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(validator_test, inputfile_not_readable)
{
    sharg::test::tmp_filename const tmp_name{"my_file.test"};
    std::filesystem::path const tmp_path{tmp_name.get_path()};

    { // Open fstream to create files.
        std::ofstream tmp_file{tmp_path};
    }

    sharg::input_file_validator my_validator{};

    EXPECT_NO_THROW(my_validator(tmp_path));

    std::filesystem::permissions(tmp_path,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::read_access(tmp_path)) // Do not execute with root permissions.
    {
        EXPECT_THROW(my_validator(tmp_path), sharg::validation_error);
    }

    std::filesystem::permissions(tmp_path,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::add);
}

TEST_F(validator_test, inputfile_not_regular)
{
    sharg::test::tmp_filename const tmp{"my_file.test"};
    std::filesystem::path const filename = tmp.get_path();
    mkfifo(filename.c_str(), 0644);

    EXPECT_THROW(sharg::input_file_validator{}(filename), sharg::validation_error);
}

TEST_F(validator_test, inputdir_not_existing)
{
    sharg::test::tmp_filename const tmp_name{"dir"};
    std::filesystem::path const not_existing_dir{tmp_name.get_path()};

    EXPECT_THROW(sharg::input_directory_validator{}(not_existing_dir), sharg::validation_error);
}

TEST_F(validator_test, inputdir_not_readable)
{
    sharg::test::tmp_filename const tmp_name{"dir"};
    std::filesystem::path const tmp_dir{tmp_name.get_path()};

    std::filesystem::create_directory(tmp_dir);

    sharg::input_directory_validator my_validator{};

    EXPECT_NO_THROW(my_validator(tmp_dir));

    std::filesystem::permissions(tmp_dir,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::read_access(tmp_dir)) // Do not execute with root permissions.
    {
        EXPECT_THROW(my_validator(tmp_dir), sharg::validation_error);
    }

    std::filesystem::permissions(tmp_dir,
                                 std::filesystem::perms::owner_read | std::filesystem::perms::group_read
                                     | std::filesystem::perms::others_read,
                                 std::filesystem::perm_options::add);
}

TEST_F(validator_test, outputfile_not_writable)
{
    sharg::test::tmp_filename const tmp_name{"my_file.test"};
    std::filesystem::path const tmp_file{tmp_name.get_path()};

    sharg::output_file_validator my_validator{sharg::output_file_open_options::create_new};

    EXPECT_NO_THROW(my_validator(tmp_file));

    // Parent path is not writable.
    std::filesystem::permissions(tmp_file.parent_path(),
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::write_access(tmp_file)) // Do not execute with root permissions.
    {
        EXPECT_THROW(my_validator(tmp_file), sharg::validation_error);
    }

    // make sure we can remove the directory.
    std::filesystem::permissions(tmp_file.parent_path(),
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::add);
}

TEST_F(validator_test, output_parent_dir_not_writable)
{
    sharg::test::tmp_filename const tmp_name{"dir"};
    std::filesystem::path const tmp_dir{tmp_name.get_path()};

    sharg::output_file_validator my_validator{};

    // Directory exists but is not writable.
    std::filesystem::create_directory(tmp_dir);
    std::filesystem::permissions(tmp_dir,
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::remove);

    EXPECT_TRUE(std::filesystem::exists(tmp_dir));
    if (!sharg::test::write_access(tmp_dir)) // Do not execute with root permissions.
    {
        EXPECT_THROW(my_validator(tmp_dir), sharg::validation_error);
    }

    // Parent path is not writable.
    std::filesystem::permissions(tmp_dir.parent_path(),
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::write_access(tmp_dir)) // Do not execute with root permissions.
    {
        EXPECT_THROW(my_validator(tmp_dir), sharg::validation_error);
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

TEST_F(validator_test, outputdir_not_writable)
{
    sharg::test::tmp_filename const tmp_name{"dir"};
    std::filesystem::path const tmp_dir{tmp_name.get_path()};

    std::filesystem::create_directory(tmp_dir);

    EXPECT_NO_THROW(sharg::output_directory_validator{}(tmp_dir));

    // This path exists but is not writable.
    std::filesystem::permissions(tmp_dir,
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::remove);

    if (!sharg::test::write_access(tmp_dir)) // Do not execute with root permissions.
    {
        EXPECT_THROW(sharg::output_file_validator{}(tmp_dir), sharg::validation_error);
    }

    // make sure we can remove the directory.
    std::filesystem::permissions(tmp_dir,
                                 std::filesystem::perms::owner_write | std::filesystem::perms::group_write
                                     | std::filesystem::perms::others_write,
                                 std::filesystem::perm_options::add);
}

TEST_F(validator_test, arithmetic_range_validator_success)
{
    int value{};
    double value2{};
    std::vector<int> vector{};

    // option
    auto parser = get_parser("-i", "10");
    parser.add_option(value, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, 10);

    // option - negative values
    parser = get_parser("-i", "-10");
    parser.add_option(value, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-20, 20}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, -10);

    // positional option
    parser = get_parser("10");
    parser.add_positional_option(value, sharg::config{.validator = sharg::arithmetic_range_validator{1, 20}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, 10);

    // positional option - negative values
    parser = get_parser("--", "-10");
    parser.add_positional_option(value, sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, -10);

    // option - vector
    parser = get_parser("-i", "-10", "-i", "48");
    parser.add_option(vector, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-50, 50}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(vector.size(), 2u);
    EXPECT_EQ(vector[0], -10);
    EXPECT_EQ(vector[1], 48);

    // positional option - vector
    parser = get_parser("--", "-10", "1");
    parser.add_positional_option(vector, sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(vector.size(), 2u);
    EXPECT_EQ(vector[0], -10);
    EXPECT_EQ(vector[1], 1);

    // get help page message
    vector.clear();
    parser = get_parser("-h");
    parser.add_positional_option(
        vector,
        sharg::config{.description = "desc", .validator = sharg::arithmetic_range_validator{-20, 20}});
    std::string expected = std::string("test_parser\n"
                                       "===========\n"
                                       "\n"
                                       "POSITIONAL ARGUMENTS\n"
                                       "    ARGUMENT-1 (List of signed 32 bit integer)\n"
                                       "          desc Default: []. Value must be in range [-20,20].\n"
                                       "\nOPTIONS\n\n"
                                       + basic_options_str + "\n" + basic_version_str);
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // option - double value
    parser = get_parser("-i", "10.9");
    parser.add_option(value2, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_FLOAT_EQ(value2, 10.9);
}

TEST_F(validator_test, arithmetic_range_validator_error)
{
    int value{};
    double value2{};
    std::vector<int> vector{};

    // option - above max
    auto parser = get_parser("-i", "30");
    parser.add_option(value, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, 30);

    // option - below min
    parser = get_parser("-i", "-21");
    parser.add_option(value, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-20, 20}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, -21);

    // positional option - above max
    parser = get_parser("30");
    parser.add_positional_option(value, sharg::config{.validator = sharg::arithmetic_range_validator{1, 20}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, 30);

    // positional option - below min
    parser = get_parser("--", "-21");
    parser.add_positional_option(value, sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, -21);

    // option - vector
    parser = get_parser("-i", "-100");
    parser.add_option(vector, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{-50, 50}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_TRUE(vector.size() == 1u);
    EXPECT_EQ(vector[0], -100);

    // positional option - vector
    parser = get_parser("--", "-10", "100");
    parser.add_positional_option(vector, sharg::config{.validator = sharg::arithmetic_range_validator{-20, 20}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_TRUE(vector.size() == 2u);
    EXPECT_EQ(vector[0], -10);
    EXPECT_EQ(vector[1], 100);

    // option - double value
    parser = get_parser("-i", "0.9");
    parser.add_option(value2, sharg::config{.short_id = 'i', .validator = sharg::arithmetic_range_validator{1, 20}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_FLOAT_EQ(value2, 0.9);
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

TEST_F(validator_test, value_list_validator_success)
{
    // type deduction
    // --------------
    // all arithmetic types are deduced to their common type in order to easily allow chaining of arithmetic validators
    EXPECT_SAME_TYPE(sharg::value_list_validator<int>, decltype(sharg::value_list_validator{1}));

    // except char
    EXPECT_SAME_TYPE(sharg::value_list_validator<char>, decltype(sharg::value_list_validator{'c'}));

    // The same holds for a range of arithmetic types
    std::vector v{1, 2, 3};
    EXPECT_SAME_TYPE(sharg::value_list_validator<int>, decltype(sharg::value_list_validator{v}));
    EXPECT_SAME_TYPE(sharg::value_list_validator<int>, decltype(sharg::value_list_validator{v | std::views::take(2)}));

    std::vector v2{'1', '2', '3'};
    EXPECT_SAME_TYPE(sharg::value_list_validator<char>, decltype(sharg::value_list_validator{v2}));
    EXPECT_SAME_TYPE(sharg::value_list_validator<char>,
                     decltype(sharg::value_list_validator{v2 | std::views::take(2)}));

    // const char * is deduced to std::string
    std::vector v3{"ha", "ba", "ma"};
    EXPECT_SAME_TYPE(sharg::value_list_validator<std::string>, decltype(sharg::value_list_validator{"ha"}));
    EXPECT_SAME_TYPE(sharg::value_list_validator<std::string>, decltype(sharg::value_list_validator{"ha", "ba", "ma"}));
    EXPECT_SAME_TYPE(sharg::value_list_validator<std::string>, decltype(sharg::value_list_validator{v3}));
    EXPECT_SAME_TYPE(sharg::value_list_validator<std::string>,
                     decltype(sharg::value_list_validator{v3 | std::views::take(2)}));

    // custom types are used as is
    EXPECT_SAME_TYPE(sharg::value_list_validator<foo>, decltype(sharg::value_list_validator{foo::one, foo::two}));

    // usage
    // -----
    std::string str_value{};
    int int_value{};
    std::vector<std::string> str_vector{};
    std::vector<int> int_vector{};
    std::vector<std::string> const valid_str_values{"ha", "ba", "ma"};

    // option
    auto parser = get_parser("-s", "ba");
    parser.add_option(str_value,
                      sharg::config{.short_id = 's',
                                    .validator = sharg::value_list_validator{valid_str_values | std::views::take(2)}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(str_value, "ba");

    // option with integers
    parser = get_parser("-i", "-21");
    parser.add_option(int_value,
                      sharg::config{.short_id = 'i', .validator = sharg::value_list_validator<int>{0, -21, 10}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(int_value, -21);

    // positional option
    parser = get_parser("ma");
    parser.add_positional_option(str_value, sharg::config{.validator = sharg::value_list_validator{valid_str_values}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(str_value, "ma");

    // positional option - vector
    parser = get_parser("ha", "ma");
    parser.add_positional_option(str_vector, sharg::config{.validator = sharg::value_list_validator{"ha", "ba", "ma"}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(str_vector.size(), 2u);
    EXPECT_EQ(str_vector[0], "ha");
    EXPECT_EQ(str_vector[1], "ma");

    // option - vector
    parser = get_parser("-i", "-10", "-i", "48");
    parser.add_option(int_vector,
                      sharg::config{.short_id = 'i', .validator = sharg::value_list_validator<int>{-10, 48, 50}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(int_vector.size(), 2u);
    EXPECT_EQ(int_vector[0], -10);
    EXPECT_EQ(int_vector[1], 48);

    // get help page message
    int_vector.clear();
    parser = get_parser("-h");
    parser.add_option(int_vector,
                      sharg::config{.short_id = 'i',
                                    .long_id = "int-option",
                                    .description = "desc",
                                    .validator = sharg::value_list_validator<int>{-10, 48, 50}});
    std::string expected = std::string("test_parser\n"
                                       "===========\n"
                                       "\nOPTIONS\n"
                                       "    -i, --int-option (List of signed 32 bit integer)\n"
                                       "          desc Default: []. Value must be one of [-10, 48, 50].\n\n"
                                       + basic_options_str + "\n" + basic_version_str);
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(validator_test, value_list_validator_error)
{
    std::string str_value{};
    int int_value{};
    std::vector<std::string> str_vector{};
    std::vector<int> int_vector{};

    // option
    auto parser = get_parser("-s", "sa");
    parser.add_option(str_value,
                      sharg::config{.short_id = 's', .validator = sharg::value_list_validator{"ha", "ba", "ma"}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(str_value, "sa");

    // positional option
    parser = get_parser("30");
    parser.add_positional_option(int_value, sharg::config{.validator = sharg::value_list_validator{0, 5, 10}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(int_value, 30);

    // positional option - vector
    parser = get_parser("fo", "ma");
    parser.add_positional_option(str_vector, sharg::config{.validator = sharg::value_list_validator{"ha", "ba", "ma"}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(str_vector.size(), 2u);
    EXPECT_EQ(str_vector[0], "fo");
    EXPECT_EQ(str_vector[1], "ma");

    // option - vector
    parser = get_parser("-i", "-10", "-i", "488");
    parser.add_option(int_vector,
                      sharg::config{.short_id = 'i', .validator = sharg::value_list_validator<int>{-10, 48, 50}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(int_vector.size(), 2u);
    EXPECT_EQ(int_vector[0], -10);
    EXPECT_EQ(int_vector[1], 488);
}

// https://github.com/seqan/sharg-parser/issues/178
TEST_F(validator_test, value_list_validator_issue178)
{
    std::filesystem::path value{};
    std::vector<std::filesystem::path> vector{};
    constexpr std::array valid_values{"ha", "ba", "ma"};

    // option
    auto parser = get_parser("-s", "ba");
    parser.add_option(value, sharg::config{.short_id = 's', .validator = sharg::value_list_validator{valid_values}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "ba");

    // option - vector
    parser = get_parser("-s", "ha", "-s", "ba");
    parser.add_option(vector, sharg::config{.short_id = 's', .validator = sharg::value_list_validator{valid_values}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(vector.size(), 2u);
    EXPECT_EQ(vector[0], "ha");
    EXPECT_EQ(vector[1], "ba");
}

TEST_F(validator_test, regex_validator_success)
{
    std::string value{};
    std::filesystem::path path{};
    std::vector<std::string> vector{};
    sharg::regex_validator email_validator("[a-zA-Z]+@[a-zA-Z]+\\.com");
    sharg::regex_validator email_vector_validator("[a-zA-Z]+@[a-zA-Z]+\\.com");

    // option
    auto parser = get_parser("-s", "ballo@rollo.com");
    parser.add_option(value, sharg::config{.short_id = 's', .validator = email_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "ballo@rollo.com");

    // positional option
    parser = get_parser("chr1");
    parser.add_positional_option(value, sharg::config{.validator = sharg::regex_validator{"^chr[0-9]+"}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, "chr1");

    // positional option - vector
    parser = get_parser("rollo", "bollo", "lollo");
    parser.add_positional_option(vector, sharg::config{.validator = sharg::regex_validator{".*oll.*"}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(vector.size(), 3u);
    EXPECT_EQ(vector[0], "rollo");
    EXPECT_EQ(vector[1], "bollo");
    EXPECT_EQ(vector[2], "lollo");

    // option - vector
    parser = get_parser("-s", "rita@rambo.com", "-s", "tina@rambo.com");
    parser.add_option(vector, sharg::config{.short_id = 's', .validator = email_vector_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(vector.size(), 2u);
    EXPECT_EQ(vector[0], "rita@rambo.com");
    EXPECT_EQ(vector[1], "tina@rambo.com");

    // option - std::filesystem::path
    parser = get_parser("-s", "rita@rambo.com");
    parser.add_option(path, sharg::config{.short_id = 's', .validator = email_vector_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(path, "rita@rambo.com");

    // get help page message
    vector.clear();
    parser = get_parser("-h");
    parser.add_option(vector,
                      sharg::config{.short_id = 's',
                                    .long_id = "string-option",
                                    .description = "desc",
                                    .validator = email_vector_validator});
    std::string expected = std::string("test_parser\n"
                                       "===========\n"
                                       "\nOPTIONS\n"
                                       "    -s, --string-option (List of std::string)\n"
                                       "          desc Default: []. Value must match the pattern\n"
                                       "          '[a-zA-Z]+@[a-zA-Z]+\\.com'.\n"
                                       "\n"
                                       + basic_options_str + "\n" + basic_version_str);
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);
}

TEST_F(validator_test, regex_validator_error)
{
    std::string value{};
    std::vector<std::string> vector{};

    // option
    auto parser = get_parser("--string-option", "sally");
    parser.add_option(value, sharg::config{.long_id = "string-option", .validator = sharg::regex_validator{"tt"}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, "sally");

    // positional option
    parser = get_parser("jessy");
    parser.add_positional_option(value, sharg::config{.validator = sharg::regex_validator{"[0-9]"}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, "jessy");

    // positional option - vector
    parser = get_parser("rollo", "bttllo", "lollo");
    parser.add_positional_option(vector, sharg::config{.validator = sharg::regex_validator{".*oll.*"}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(vector.size(), 3u);
    EXPECT_EQ(vector[0], "rollo");
    EXPECT_EQ(vector[1], "bttllo");
    EXPECT_EQ(vector[2], "lollo");

    // option - vector
    parser = get_parser("-s", "gh", "-s", "tt");
    parser.add_option(vector, sharg::config{.short_id = 's', .validator = sharg::regex_validator{"tt"}});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(vector.size(), 2u);
    EXPECT_EQ(vector[0], "gh");
    EXPECT_EQ(vector[1], "tt");
}

TEST_F(validator_test, chaining_validators_common_type)
{
    // chaining integral options stay integral
    constexpr int max_int = std::numeric_limits<int>::max();

    std::vector v_int{1, 2, 3, max_int};
    EXPECT_SAME_TYPE(std::vector<int>, decltype(v_int));

    std::vector v_unsigned{4u, static_cast<unsigned>(max_int)};
    EXPECT_SAME_TYPE(std::vector<unsigned>, decltype(v_unsigned));

    sharg::value_list_validator validator_int{v_int};
    EXPECT_SAME_TYPE(sharg::value_list_validator<int>, decltype(validator_int));
    EXPECT_SAME_TYPE(int, decltype(validator_int)::option_value_type);

    sharg::value_list_validator validator_unsigned{v_unsigned};
    EXPECT_SAME_TYPE(sharg::value_list_validator<unsigned>, decltype(validator_unsigned));
    EXPECT_SAME_TYPE(unsigned, decltype(validator_unsigned)::option_value_type);

    auto validator = validator_int | validator_unsigned;
    EXPECT_SAME_TYPE(unsigned, std::common_type_t<int, unsigned>);
    EXPECT_SAME_TYPE(unsigned, decltype(validator)::option_value_type);

    // max_int is part of both validators
    EXPECT_NO_THROW(validator_int(max_int));
    EXPECT_NO_THROW(validator_unsigned(max_int));
    EXPECT_NO_THROW(validator(max_int));

    // chaining mixed arithmetic options will be highest common arithmetic type
    constexpr int64_t max_int64 = std::numeric_limits<int64_t>::max();

    // note: this number is not representable by double and multiple integer values represent the same double value
    EXPECT_EQ(static_cast<double>(max_int64), static_cast<double>(max_int64 - 1));

    std::vector<int64_t> v_int64{1, 2, 3, max_int64};
    sharg::value_list_validator validator_int64{v_int64};
    EXPECT_SAME_TYPE(sharg::value_list_validator<int64_t>, decltype(validator_int64));
    EXPECT_SAME_TYPE(int64_t, decltype(validator_int64)::option_value_type);

    std::vector<uint64_t> v_uint64{4u, static_cast<uint64_t>(max_int64)};
    sharg::value_list_validator validator_uint64{v_uint64};
    EXPECT_SAME_TYPE(sharg::value_list_validator<uint64_t>, decltype(validator_uint64));
    EXPECT_SAME_TYPE(uint64_t, decltype(validator_uint64)::option_value_type);

    std::vector<double> v_double{4.0, static_cast<double>(max_int64)};
    sharg::value_list_validator validator_double{v_double};
    EXPECT_SAME_TYPE(sharg::value_list_validator<double>, decltype(validator_double));
    EXPECT_SAME_TYPE(double, decltype(validator_double)::option_value_type);

    auto validator2 = validator_int64 | validator_uint64 | validator_double;
    EXPECT_SAME_TYPE(double, std::common_type_t<int64_t, uint64_t, double>);
    EXPECT_SAME_TYPE(double, decltype(validator2)::option_value_type);

    // max_int64 is an exact match for the two integral validators
    // note: double will decay the integer to a smaller double value,
    //       but this is consistent, because it is the same given value
    // note: chained validator passes the value as it is through,
    //       so validator_[u]int64 will be called with the integer value
    EXPECT_NO_THROW(validator_int64(max_int64));
    EXPECT_NO_THROW(validator_uint64(max_int64));
    EXPECT_NO_THROW(validator_double(static_cast<double>(max_int64)));
    EXPECT_NO_THROW(validator2(max_int64));

    // integers have exact match
    // note: double accepts that value, even though it is not within that list.
    EXPECT_THROW(validator_int64(max_int64 - 1), sharg::validation_error);
    EXPECT_THROW(validator_uint64(max_int64 - 1), sharg::validation_error);
    EXPECT_NO_THROW(validator_double(static_cast<double>(max_int64 - 1)));
    EXPECT_THROW(validator2(max_int64 - 1), sharg::validation_error);
}

TEST_F(validator_test, chaining_validators)
{
    std::string value{};
    std::vector<std::string> option_vector{};
    sharg::regex_validator absolute_path_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"};
    sharg::output_file_validator my_file_ext_validator{sharg::output_file_open_options::create_new, {"sa", "so"}};

    sharg::test::tmp_filename const tmp_name{"file.sa"};
    std::string const tmp_string{tmp_name.get_path().string()};
    std::string const tmp_relative_string{tmp_name.get_path().relative_path().string()};
    std::filesystem::path invalid_extension{tmp_name.get_path()};
    invalid_extension.replace_extension(".invalid");
    std::string const invalid_extension_string{invalid_extension.string()};

    // option
    auto option_validator = absolute_path_validator | my_file_ext_validator;

    auto parser = get_parser("-s", tmp_string);
    parser.add_option(value, sharg::config{.short_id = 's', .validator = option_validator});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, tmp_string);

    parser = get_parser("-s", tmp_relative_string);
    parser.add_option(value, sharg::config{.short_id = 's', .validator = option_validator});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, tmp_relative_string);

    parser = get_parser("-s", invalid_extension_string);
    parser.add_option(value, sharg::config{.short_id = 's', .validator = option_validator});
    EXPECT_THROW(parser.parse(), sharg::validation_error);
    EXPECT_EQ(value, invalid_extension_string);

    // with temporary validators
    parser = get_parser("-s", tmp_string);
    parser.add_option(
        value,
        sharg::config{.short_id = 's',
                      .validator =
                          sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                          | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, tmp_string);

    // three validators
    parser = get_parser("-s", tmp_string);
    parser.add_option(
        value,
        sharg::config{.short_id = 's',
                      .validator =
                          sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                          | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}
                          | sharg::regex_validator{".*"}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(value, tmp_string);

    // help page message
    value.clear();
    parser = get_parser("-h");
    parser.add_option(
        value,
        sharg::config{.short_id = 's',
                      .long_id = "string-option",
                      .description = "desc",
                      .validator =
                          sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                          | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}
                          | sharg::regex_validator{".*"}});
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
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // help page message (allow overwriting)
    parser = get_parser("-h");
    parser.add_option(
        value,
        sharg::config{.short_id = 's',
                      .long_id = "string-option",
                      .description = "desc",
                      .validator =
                          sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                          | sharg::output_file_validator{sharg::output_file_open_options::open_or_create, {"sa", "so"}}
                          | sharg::regex_validator{".*"}});
    expected = std::string{"test_parser\n"
                           "===========\n"
                           "\nOPTIONS\n"
                           "    -s, --string-option (std::string)\n"
                           "          desc Default: \"\". Value must match the pattern\n"
                           "          '(/[^/]+)+/.*\\.[^/\\.]+$'. Write permissions must be granted. Valid\n"
                           "          file extensions are: [sa, so]. Value must match the pattern '.*'.\n"
                           "\n"}
             + basic_options_str + "\n" + basic_version_str;
    EXPECT_EQ(get_parse_cout_on_exit(parser), expected);

    // chaining with a container option value type
    parser = get_parser("-s", tmp_string);
    parser.add_option(
        option_vector,
        sharg::config{.short_id = 's',
                      .validator =
                          sharg::regex_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"}
                          | sharg::output_file_validator{sharg::output_file_open_options::create_new, {"sa", "so"}}});
    EXPECT_NO_THROW(parser.parse());
    EXPECT_EQ(option_vector.size(), 1u);
    EXPECT_EQ(option_vector[0], tmp_string);
}
