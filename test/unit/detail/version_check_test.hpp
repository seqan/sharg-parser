// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <sharg/parser.hpp>
#include <sharg/test/tmp_filename.hpp>

//------------------------------------------------------------------------------
// test fixtures
//------------------------------------------------------------------------------

namespace sharg::detail
{
struct test_accessor
{
    static auto & version_check_future(sharg::parser & parser)
    {
        return parser.version_check_future;
    }
};
} // namespace sharg::detail

bool wait_for(sharg::parser & parser)
{
    auto & future = sharg::detail::test_accessor::version_check_future(parser);

    if (future.valid())
        return future.get();
    return false;
}

struct version_check : public ::testing::Test
{
    char const * const OPTION_VERSION_CHECK = "--version-check";
    char const * const OPTION_OFF = "false";
    char const * const OPTION_ON = "true";

    std::string const app_name = std::string{"test_version_check"};

    // This tmp_filename will create the file "version_checker.tmpfile" in a unique folder.
    sharg::test::tmp_filename tmp_file{"version_checker.tmpfile"};

    void randomise_home_folder()
    {
        using namespace std::string_literals;
        auto tmp_directory = tmp_file.get_path().parent_path();

        int result = setenv(sharg::detail::version_checker::home_env_name, tmp_directory.c_str(), 1);
        if (result != 0)
            throw std::runtime_error{"Couldn't set environment variable 'home_env_name' (="s
                                     + sharg::detail::version_checker::home_env_name + ")"s};

        auto is_prefix_path = [](std::string const & base_path, std::string const & path)
        {
            auto && it_pair = std::mismatch(base_path.begin(), base_path.end(), path.begin(), path.end());
            return it_pair.first + 1 == base_path.end();
        };

        if (is_prefix_path(tmp_directory, app_tmp_path()))
            throw std::runtime_error{"Setting the environment variable 'home_env_name' didn't have the correct effect"
                                     " ("s
                                     + std::string{tmp_directory} + " is not a prefix of "s
                                     + std::string{app_tmp_path()} + ")"s};
    }

    void SetUp() override
    {
        ::testing::Test::SetUp();

        // set HOME environment to a random home folder before starting each test case
        randomise_home_folder();
    }

    std::filesystem::path app_tmp_path()
    {
        return sharg::detail::version_checker::get_path();
    }

    std::filesystem::path app_version_filename()
    {
        return app_tmp_path() / (app_name + ".version");
    }

    std::filesystem::path app_timestamp_filename()
    {
        return sharg::detail::version_checker{app_name, std::string{}}.timestamp_filename;
    }

    std::chrono::duration<long int>::rep current_unix_timestamp()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    std::regex timestamp_regex{"^[[:digit:]]+$"}; // only digits

    std::tuple<std::string, std::string, bool> simulate_parser(int argc, char const ** argv)
    {
        // make sure that the environment variable is not set
        std::string previous_value{};
        if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
        {
            previous_value = env;
            unsetenv("SHARG_NO_VERSION_CHECK");
        }

        bool app_call_succeeded{false};

        sharg::parser parser{app_name, argc, argv};
        parser.info.version = "2.3.4";

        // In case we don't want to specify --version-check but avoid that short help format will be set (no arguments)
        bool dummy{false};
        parser.add_flag(dummy, sharg::config{.short_id = 'f'});

        testing::internal::CaptureStdout();
        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        std::string out = testing::internal::GetCapturedStdout();
        std::string err = testing::internal::GetCapturedStderr();

        // call future.get() to artificially wait for the thread to finish and avoid
        // any interference with following tests
        app_call_succeeded = wait_for(parser);

        if (!previous_value.empty())
            setenv("SHARG_NO_VERSION_CHECK", previous_value.c_str(), 1);

        return {out, err, app_call_succeeded};
    }

    bool remove_files_from_path()
    {
        return (!std::filesystem::exists(app_version_filename()) || std::filesystem::remove(app_version_filename()))
            && (!std::filesystem::exists(app_timestamp_filename())
                || std::filesystem::remove(app_timestamp_filename()));
    }

    template <typename message_type>
    bool create_file(std::filesystem::path const & filename, message_type const & message)
    {
        std::ofstream out_file{filename};

        if (!out_file.is_open())
            return false;

        out_file << message;
        out_file.close();

        return true;
    }

    std::string read_file(std::filesystem::path const & filename)
    {
        std::ifstream in_file{filename};
        std::string line{};

        if (in_file.is_open())
        {
            std::getline(in_file, line);
            in_file.close();
        }

        return line;
    }
};

struct sanity_checks : public version_check
{};

//------------------------------------------------------------------------------
// sanity checks
//------------------------------------------------------------------------------

// even if the homedir might not be writable at least the tmp dir should be
TEST_F(sanity_checks, path_availability)
{
    EXPECT_FALSE(app_tmp_path().empty()) << "No writable directory found. All other tests cannot be trusted!";
}

TEST_F(sanity_checks, create_and_delete_files)
{
    EXPECT_TRUE(create_file(app_version_filename(), "20.5.9"));
    EXPECT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp()));

    EXPECT_TRUE(std::filesystem::exists(app_version_filename()));
    EXPECT_TRUE(std::filesystem::exists(app_timestamp_filename()));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
    EXPECT_FALSE(std::filesystem::exists(app_version_filename()));
    EXPECT_FALSE(std::filesystem::exists(app_timestamp_filename()));
}

TEST_F(sanity_checks, cookie)
{
    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_ON};
    auto [out, err, app_call_succeeded] = simulate_parser(3, argv);

    if (app_call_succeeded)
    {
        // Write out the cookie for inspection.
        // When manually executing this test, check that the cookie looks like this:
        // ```
        // UNREGISTERED_APP
        // X.X.X <-- Should reflect the latest release of sharg
        // ```
        // This should not be tested via EXPECT_EQ(..) because then sharg tests fail if the server wasn't
        // configured correctly and we want to be independent of the server.
        std::cout << "Cookie:" << std::endl;
        std::ifstream app_version_file{app_version_filename()};
        std::string line;
        std::getline(app_version_file, line);
        std::cout << line << " <-- Should be UNREGISTERED_APP!" << std::endl;
        std::getline(app_version_file, line);
        std::cout << line << " <-- Should be the latest Sharg version! "
                  << "Updates done here: https://github.com/OpenMS/usage_plots/blob/master/seqan_versions.txt"
                  << std::endl;
    }
}

//------------------------------------------------------------------------------
// version checks
//------------------------------------------------------------------------------

TEST_F(version_check, option_on)
{
    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_ON};

    auto [out, err, app_call_succeeded] = simulate_parser(3, argv);

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    // no timestamp is written since the decision was made explicitly
    if (app_call_succeeded)
    {
        EXPECT_TRUE(std::filesystem::exists(app_version_filename()));
    }
    else
    {
        std::cout << "App call did not succeed (server offline?) and could thus not be tested.\n";
    }

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

// Note that we cannot test interactiveness because google test captures std::cin and thus
// sharg::detail::input_is_terminal() is always false
TEST_F(version_check, option_implicitely_on)
{
    char const * argv[2] = {app_name.c_str(), "-f"};

    auto [out, err, app_call_succeeded] = simulate_parser(2, argv);

    EXPECT_EQ(out, "");
    EXPECT_EQ(err,
              "\n#######################################################################\n"
              "   Automatic Update Notifications\n"
              "#######################################################################\n"
              " This app performs automatic checks for updates. For more information\n"
              " see: https://docs.seqan.de/sharg/main_user/about_update_notifications.html\n"
              "#######################################################################\n\n");

    // make sure that all files now exist
    EXPECT_TRUE(std::filesystem::exists(app_timestamp_filename())) << app_timestamp_filename();
    EXPECT_TRUE(std::regex_match(read_file(app_timestamp_filename()), timestamp_regex));

    if (app_call_succeeded)
    {
        EXPECT_TRUE(std::filesystem::exists(app_version_filename()));
    }
    else
    {
        std::cout << "App call did not succeed (server offline?) and could thus not be tested.\n";
    }

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, time_out) // while implicitly on
{
    char const * argv[2] = {app_name.c_str(), "-f"};

    // create timestamp files
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp()));

    auto [out, err, app_call_succeeded] = simulate_parser(2, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    EXPECT_FALSE(std::filesystem::exists(app_version_filename()));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, environment_variable_set)
{
    // store variable for resetting it
    std::string previous_value{};
    if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
        previous_value = env;

    setenv("SHARG_NO_VERSION_CHECK", "foo", 1);

    char const * argv[2] = {app_name.c_str(), "-f"};

    sharg::parser parser{app_name, 2, argv};
    parser.info.version = "2.3.4";
    bool dummy{false};
    parser.add_flag(dummy, sharg::config{.short_id = 'f'});

    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    std::string out = testing::internal::GetCapturedStdout();
    std::string err = testing::internal::GetCapturedStderr();

    // call future.get() to artificially wait for the thread to finish and avoid
    // any interference with following tests
    wait_for(parser);

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    // if environment variable is set, no cookies are written
    EXPECT_FALSE(std::filesystem::exists(app_timestamp_filename())) << app_timestamp_filename();
    EXPECT_FALSE(std::filesystem::exists(app_version_filename()));

    if (previous_value.empty())
        unsetenv("SHARG_NO_VERSION_CHECK");
    else
        setenv("SHARG_NO_VERSION_CHECK", previous_value.c_str(), 1);

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, option_off)
{
    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_OFF};

    auto [out, err, app_call_succeeded] = simulate_parser(3, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    // no timestamp is written since the decision was made explicitly
    EXPECT_FALSE(std::filesystem::exists(app_version_filename())) << app_version_filename();
    EXPECT_FALSE(std::filesystem::exists(app_timestamp_filename())) << app_timestamp_filename();

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, option_off_with_help_page)
{
    // Version check option always needs to be parsed, even if special formats get selected
    char const * argv[4] = {app_name.c_str(), "-h", OPTION_VERSION_CHECK, OPTION_OFF};

    std::string previous_value{};
    if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
    {
        previous_value = env;
        unsetenv("SHARG_NO_VERSION_CHECK");
    }

    sharg::parser parser{app_name, 4, argv};
    parser.info.version = "2.3.4";

    EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");

    // call future.get() to artificially wait for the thread to finish and avoid
    // any interference with following tests
    EXPECT_FALSE(wait_for(parser));

    if (!previous_value.empty())
        setenv("SHARG_NO_VERSION_CHECK", previous_value.c_str(), 1);

    // no timestamp is written since the decision was made explicitly
    EXPECT_FALSE(std::filesystem::exists(app_version_filename())) << app_version_filename();

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

// case: the current parser has a smaller Sharg version than is present in the version file
#if !defined(NDEBUG)
TEST_F(version_check, smaller_sharg_version)
{
    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_ON};

    // create version file with euqal app version and a greater Sharg version than the current
    create_file(app_version_filename(), std::string{"2.3.4\n20.5.9"});

    // create timestamp file that dates one day before current to trigger a message (one day = 86400 seconds)
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401));

    auto [out, err, app_call_succeeded] = simulate_parser(3, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, sharg::detail::version_checker::message_sharg_update);

    EXPECT_TRUE(std::regex_match(read_file(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

// case: the current parser has a greater app version than is present in the version file
TEST_F(version_check, greater_app_version)
{
    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_ON};

    // create version file with equal Sharg version and a smaller app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"1.5.9\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401)); // one day = 86400 seconds

    auto [out, err, app_call_succeeded] = simulate_parser(3, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, sharg::detail::version_checker::message_registered_app_update);

    EXPECT_TRUE(std::regex_match(read_file(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, unregistered_app)
{
    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_ON};

    // create version file with equal Sharg version and a smaller app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"UNREGISTERED_APP\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401)); // one day = 86400 seconds

    auto [out, err, app_call_succeeded] = simulate_parser(3, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, sharg::detail::version_checker::message_unregistered_app);

    EXPECT_TRUE(std::regex_match(read_file(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}
#endif // !defined(NDEBUG)

// case: the current parser has a smaller app version than is present in the version file
#if defined(NDEBUG)
TEST_F(version_check, smaller_app_version)
{
    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_ON};

    // create version file with equal Sharg version and a greater app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"20.5.9\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message (one day = 86400 seconds)
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401));

    auto [out, err, app_call_succeeded] = simulate_parser(3, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, (sharg::detail::version_checker{app_name, "2.3.4"}.message_app_update));

    EXPECT_TRUE(std::regex_match(read_file(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, smaller_app_version_custom_url)
{
    std::string previous_value{};
    if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
    {
        previous_value = env;
        unsetenv("SHARG_NO_VERSION_CHECK");
    }

    char const * argv[3] = {app_name.c_str(), OPTION_VERSION_CHECK, OPTION_ON};

    // create version file with equal Sharg version and a greater app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"20.5.9\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message (one day = 86400 seconds)
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401));

    sharg::parser parser{app_name, 3, argv};
    parser.info.version = "2.3.4";
    parser.info.url = "https//foo.de";

    testing::internal::CaptureStdout();
    testing::internal::CaptureStderr();
    EXPECT_NO_THROW(parser.parse());
    std::string out = testing::internal::GetCapturedStdout();
    std::string err = testing::internal::GetCapturedStderr();

    // call future.get() to artificially wait for the thread to finish and avoid
    // any interference with following tests
    wait_for(parser);

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, (sharg::detail::version_checker{app_name, parser.info.version, parser.info.url}.message_app_update));

    EXPECT_TRUE(std::regex_match(read_file(app_timestamp_filename()), timestamp_regex));

    if (!previous_value.empty())
        setenv("SHARG_NO_VERSION_CHECK", previous_value.c_str(), 1);

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}
#endif // defined(NDEBUG)

TEST_F(version_check, user_specified_never)
{
    char const * argv[2] = {app_name.c_str(), "-f"}; // no explicit version check option

    // create timestamp files
    ASSERT_TRUE(create_file(app_timestamp_filename(), "NEVER"));

    auto [out, err, app_call_succeeded] = simulate_parser(2, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    EXPECT_FALSE(std::filesystem::exists(app_version_filename()));
    EXPECT_EQ(read_file(app_timestamp_filename()), "NEVER"); // should not be modified

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, user_specified_always)
{
    char const * argv[2] = {app_name.c_str(), "-f"}; // no explicit version check option

    // create timestamp files
    ASSERT_TRUE(create_file(app_timestamp_filename(), "ALWAYS"));

    auto [out, err, app_call_succeeded] = simulate_parser(2, argv);

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    if (app_call_succeeded)
    {
        EXPECT_TRUE(std::filesystem::exists(app_version_filename()));
    }
    else
    {
        std::cout << "App call did not succeed (server offline?) and could thus not be tested.\n";
    }

    EXPECT_EQ(read_file(app_timestamp_filename()), "ALWAYS"); // should not be modified
    EXPECT_TRUE(remove_files_from_path());                    // clear files again

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check, wrong_version_string)
{
    char const * argv[2] = {app_name.c_str(), "-f"}; // no explicit version check option

    // create a corrupted version file. Nothing should be printed, it is just ignored
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"20.wrong.9\nalso.wrong.4"}));
    ASSERT_TRUE(create_file(app_timestamp_filename(), "ALWAYS"));

    auto [out, err, app_call_succeeded] = simulate_parser(2, argv);
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}
