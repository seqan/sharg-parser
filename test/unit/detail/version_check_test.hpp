// SPDX-FileCopyrightText: 2006-2026, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <thread>

#include <sharg/parser.hpp>
#include <sharg/test/test_fixture.hpp>
#include <sharg/test/tmp_filename.hpp>

//------------------------------------------------------------------------------
// test fixtures
//------------------------------------------------------------------------------

class version_check_test : public sharg::test::test_fixture
{
protected:
    std::string const OPTION_VERSION_CHECK = "--version-check";
    std::string const OPTION_OFF = "false";
    std::string const OPTION_ON = "true";

    std::string const app_name = "test_version_check";

    // This tmp_filename will create the file "version_checker.tmpfile" in a unique folder.
    sharg::test::tmp_filename tmp_file{"version_checker.tmpfile"};

    void randomise_home_folder()
    {
        auto const tmp_directory = tmp_file.get_path().parent_path();
        std::string const base_path = tmp_directory.string();

        if (setenv(sharg::detail::version_checker::home_env_name, base_path.data(), 1))
            throw std::runtime_error{"Couldn't set environment variable 'home_env_name' (="
                                     + std::string{sharg::detail::version_checker::home_env_name} + ")"};

        std::string const path = app_tmp_path().string(); // Must be string because app_tmp_path() returns a temporary.

        if (!path.starts_with(base_path))
            throw std::runtime_error{"Setting the environment variable 'home_env_name' didn't have the correct effect"
                                     " ("
                                     + base_path + " is not a prefix of " + path + ")"};
    }

    static bool wait_for(sharg::parser & parser)
    {
        auto & future = sharg::detail::test_accessor::version_check_future(parser);

        if (future.valid())
            return future.get();
        return false;
    }

    // Some tests change TMPDIR, which is restored after each test case.
    std::optional<std::string> cached_tmpdir{};

    void SetUp() override
    {
        if (char const * tmpdir = std::getenv("TMPDIR"))
            cached_tmpdir = tmpdir;

        // set HOME environment to a random home folder before starting each test case
        randomise_home_folder();
    }

    void TearDown() override
    {
        if (cached_tmpdir.has_value())
            setenv("TMPDIR", cached_tmpdir->c_str(), 1);
        else
            unsetenv("TMPDIR");
    }

    static std::filesystem::path app_tmp_path()
    {
        return sharg::detail::version_checker::get_path();
    }

    std::filesystem::path app_version_filename() const
    {
        return app_tmp_path() / (app_name + ".version");
    }

    std::filesystem::path app_timestamp_filename() const
    {
        return sharg::detail::version_checker{app_name, std::string{}}.timestamp_filename;
    }

    static auto current_unix_timestamp()
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    static inline std::regex const timestamp_regex{"^[[:digit:]]+$"}; // only digits

    template <typename... arg_ts>
    std::tuple<std::string, std::string, bool> simulate_parser(arg_ts &&... args)
    {
        bool app_call_succeeded{false};
        bool dummy_flag{false};

        // make sure that the environment variable is not set
        std::string const cached_env_var = []()
        {
            std::string result{};
            if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
            {
                result = env;
                unsetenv("SHARG_NO_VERSION_CHECK");
            }
            return result;
        }();

        std::vector<std::string> arguments{app_name, std::forward<arg_ts>(args)...};
        sharg::parser parser{app_name, std::move(arguments), sharg::update_notifications::on};
        parser.info.version = "2.3.4";

        // In case we don't want to specify --version-check but avoid that short help format will be set (no arguments)
        parser.add_flag(dummy_flag, sharg::config{.short_id = 'f'});

        testing::internal::CaptureStdout();
        testing::internal::CaptureStderr();
        EXPECT_NO_THROW(parser.parse());
        std::string out = testing::internal::GetCapturedStdout();
        std::string err = testing::internal::GetCapturedStderr();

        // call future.get() to artificially wait for the thread to finish and avoid
        // any interference with following tests
        app_call_succeeded = wait_for(parser);

        if (!cached_env_var.empty())
            setenv("SHARG_NO_VERSION_CHECK", cached_env_var.c_str(), 1);

        return {out, err, app_call_succeeded};
    }

    bool remove_files_from_path() const
    {
        return (!std::filesystem::exists(app_version_filename()) || std::filesystem::remove(app_version_filename()))
            && (!std::filesystem::exists(app_timestamp_filename())
                || std::filesystem::remove(app_timestamp_filename()));
    }

    template <typename message_type>
    static bool create_file(std::filesystem::path const & filename, message_type const & message)
    {
        std::ofstream out_file{filename};

        if (!out_file.is_open())
            return false;

        out_file << message;
        out_file.close();

        return true;
    }

    static std::string read_first_line(std::filesystem::path const & filename)
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

//------------------------------------------------------------------------------
// sanity checks
//------------------------------------------------------------------------------

// even if the homedir might not be writable at least the tmp dir should be
TEST_F(version_check_test, sanity_path_availability)
{
    EXPECT_FALSE(app_tmp_path().empty()) << "No writable directory found. All other tests cannot be trusted!";
}

TEST_F(version_check_test, sanity_create_and_delete_files)
{
    EXPECT_TRUE(create_file(app_version_filename(), "20.5.9"));
    EXPECT_TRUE(std::filesystem::exists(app_version_filename()));

    EXPECT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp()));
    EXPECT_TRUE(std::filesystem::exists(app_timestamp_filename()));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
    EXPECT_FALSE(std::filesystem::exists(app_version_filename()));
    EXPECT_FALSE(std::filesystem::exists(app_timestamp_filename()));
}

TEST_F(version_check_test, sanity_cookie)
{
    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");

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
        std::string line{};
        std::cout << "Cookie:\n";

        std::ifstream app_version_file{app_version_filename()};
        std::getline(app_version_file, line);
        std::cout << line << " <-- Should be UNREGISTERED_APP!\n";

        std::cout << app_version_filename().string() << '\n';

        std::getline(app_version_file, line);
        std::cout << line << " <-- Should be the latest Sharg version! "
                  << "Updates done here: https://github.com/OpenMS/usage_plots/blob/master/seqan_versions.txt\n";
    }
}

//------------------------------------------------------------------------------
// version checks
//------------------------------------------------------------------------------

TEST_F(version_check_test, option_on)
{
    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    // no timestamp is written since the decision was made explicitly
    if (app_call_succeeded)
        EXPECT_TRUE(std::filesystem::exists(app_version_filename()));
    else
        GTEST_SKIP() << "App call did not succeed (server offline?) and could thus not be tested.";

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

// Note that we cannot test interactiveness because google test captures std::cin and thus
// sharg::detail::input_is_terminal() is always false
TEST_F(version_check_test, option_implicitely_on)
{
    auto [out, err, app_call_succeeded] = simulate_parser("-f");

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    // make sure that all files now exist
    EXPECT_TRUE(std::filesystem::exists(app_timestamp_filename())) << app_timestamp_filename();
    EXPECT_TRUE(std::regex_match(read_first_line(app_timestamp_filename()), timestamp_regex));

    EXPECT_FALSE(app_call_succeeded);

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, time_out) // while implicitly on
{
    // create timestamp files
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp()));

    auto [out, err, app_call_succeeded] = simulate_parser("-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    EXPECT_FALSE(std::filesystem::exists(app_version_filename()));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, environment_variable_set)
{
    bool dummy{false};

    // store variable for resetting it
    std::string const cached_env_var = []()
    {
        if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
            return std::string{env};
        return std::string{};
    }();

    setenv("SHARG_NO_VERSION_CHECK", "foo", 1);

    sharg::parser parser{app_name, {app_name, "-f"}, sharg::update_notifications::on};
    parser.info.version = "2.3.4";
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

    if (cached_env_var.empty())
        unsetenv("SHARG_NO_VERSION_CHECK");
    else
        setenv("SHARG_NO_VERSION_CHECK", cached_env_var.c_str(), 1);

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, option_off)
{
    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_OFF, "-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    // no timestamp is written since the decision was made explicitly
    EXPECT_FALSE(std::filesystem::exists(app_version_filename())) << app_version_filename();
    EXPECT_FALSE(std::filesystem::exists(app_timestamp_filename())) << app_timestamp_filename();

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, option_off_with_help_page)
{
    // Version check option always needs to be parsed, even if special formats get selected
    std::string const cached_env_var = []()
    {
        std::string result{};
        if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
        {
            result = env;
            unsetenv("SHARG_NO_VERSION_CHECK");
        }
        return result;
    }();

    sharg::parser parser{app_name, {app_name, "-h", OPTION_VERSION_CHECK, OPTION_OFF}, sharg::update_notifications::on};
    parser.info.version = "2.3.4";

    // flag = true;
    std::string const help_message = get_parse_cout_on_exit(parser);
    // flag = false;
    EXPECT_NE(help_message, "");

    // call future.get() to artificially wait for the thread to finish and avoid
    // any interference with following tests
    EXPECT_FALSE(wait_for(parser));

    if (!cached_env_var.empty())
        setenv("SHARG_NO_VERSION_CHECK", cached_env_var.c_str(), 1);

    // no timestamp is written since the decision was made explicitly
    EXPECT_FALSE(std::filesystem::exists(app_version_filename())) << app_version_filename();

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

// case: the current parser has a smaller Sharg version than is present in the version file
#if !defined(NDEBUG)
TEST_F(version_check_test, smaller_sharg_version)
{
    // create version file with euqal app version and a greater Sharg version than the current
    create_file(app_version_filename(), std::string{"2.3.4\n20.5.9"});

    // create timestamp file that dates one day before current to trigger a message (one day = 86400 seconds)
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401));

    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, sharg::detail::version_checker::message_sharg_update);

    EXPECT_TRUE(std::regex_match(read_first_line(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

// case: the current parser has a greater app version than is present in the version file
TEST_F(version_check_test, greater_app_version)
{
    // create version file with equal Sharg version and a smaller app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"1.5.9\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401)); // one day = 86400 seconds

    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, sharg::detail::version_checker::message_registered_app_update);

    EXPECT_TRUE(std::regex_match(read_first_line(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, unregistered_app)
{
    // create version file with equal Sharg version and a smaller app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"UNREGISTERED_APP\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401)); // one day = 86400 seconds

    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, sharg::detail::version_checker::message_unregistered_app);

    EXPECT_TRUE(std::regex_match(read_first_line(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}
#endif // !defined(NDEBUG)

// case: the current parser has a smaller app version than is present in the version file
#if defined(NDEBUG)
TEST_F(version_check_test, smaller_app_version)
{
    // create version file with equal Sharg version and a greater app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"20.5.9\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message (one day = 86400 seconds)
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401));

    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, (sharg::detail::version_checker{app_name, "2.3.4"}.message_app_update));

    EXPECT_TRUE(std::regex_match(read_first_line(app_timestamp_filename()), timestamp_regex));

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, smaller_app_version_custom_url)
{
    bool dummy_flag{false};
    std::string previous_value{};
    if (char * env = std::getenv("SHARG_NO_VERSION_CHECK"))
    {
        previous_value = env;
        unsetenv("SHARG_NO_VERSION_CHECK");
    }

    // create version file with equal Sharg version and a greater app version than the current
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"20.5.9\n"} + sharg::sharg_version_cstring));

    // create timestamp file that dates one day before current to trigger a message (one day = 86400 seconds)
    ASSERT_TRUE(create_file(app_timestamp_filename(), current_unix_timestamp() - 100401));

    sharg::parser parser{app_name, {app_name, OPTION_VERSION_CHECK, OPTION_ON, "-f"}};
    parser.add_flag(dummy_flag, sharg::config{.short_id = 'f'});
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

    EXPECT_TRUE(std::regex_match(read_first_line(app_timestamp_filename()), timestamp_regex));

    if (!previous_value.empty())
        setenv("SHARG_NO_VERSION_CHECK", previous_value.c_str(), 1);

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}
#endif // defined(NDEBUG)

TEST_F(version_check_test, user_specified_never)
{
    // create timestamp files
    ASSERT_TRUE(create_file(app_timestamp_filename(), "NEVER"));

    auto [out, err, app_call_succeeded] = simulate_parser("-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    EXPECT_FALSE(std::filesystem::exists(app_version_filename()));
    EXPECT_EQ(read_first_line(app_timestamp_filename()), "NEVER"); // should not be modified

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, user_specified_always)
{
    // create timestamp files
    ASSERT_TRUE(create_file(app_timestamp_filename(), "ALWAYS"));

    auto [out, err, app_call_succeeded] = simulate_parser("-f");

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

    EXPECT_EQ(read_first_line(app_timestamp_filename()), "ALWAYS"); // should not be modified
    EXPECT_TRUE(remove_files_from_path());                          // clear files again

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

TEST_F(version_check_test, wrong_version_string)
{
    // create a corrupted version file. Nothing should be printed, it is just ignored
    ASSERT_TRUE(create_file(app_version_filename(), std::string{"20.wrong.9\nalso.wrong.4"}));
    ASSERT_TRUE(create_file(app_timestamp_filename(), "ALWAYS"));

    auto [out, err, app_call_succeeded] = simulate_parser("-f");
    (void)app_call_succeeded;

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}

//------------------------------------------------------------------------------
// directory for the timestamp and version files
//------------------------------------------------------------------------------

TEST_F(version_check_test, no_writable_directory)
{
    // HOME/.config cannot be created and the fallback TMPDIR is not a directory.
    std::filesystem::path const missing = tmp_file.get_path().parent_path() / "missing";
    ASSERT_EQ(setenv(sharg::detail::version_checker::home_env_name, missing.c_str(), 1), 0);
    ASSERT_EQ(setenv("TMPDIR", missing.c_str(), 1), 0);
    ASSERT_TRUE(app_tmp_path().empty());

    {
        auto [out, err, app_call_succeeded] = simulate_parser("-f");

        EXPECT_EQ(out, "");
        EXPECT_EQ(err, "");
        EXPECT_FALSE(app_call_succeeded);
        // Without a directory, the timestamp file would be written to the current working directory.
        EXPECT_FALSE(std::filesystem::exists(app_timestamp_filename())) << app_timestamp_filename();
    }

    {
        auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");

        EXPECT_EQ(out, "");
        EXPECT_EQ(err, "");
        EXPECT_FALSE(app_call_succeeded);
        // Without a directory, the version file would be downloaded to the current working directory.
        EXPECT_FALSE(std::filesystem::exists(app_version_filename())) << app_version_filename();
    }
}

TEST_F(version_check_test, tmp_directory_fallback)
{
    // HOME/.config cannot be created, hence the temporary directory is used.
    std::filesystem::path const missing = tmp_file.get_path().parent_path() / "missing";
    std::filesystem::path const tmp_directory = tmp_file.get_path().parent_path() / "tmp";
    std::filesystem::create_directory(tmp_directory);
    ASSERT_EQ(setenv(sharg::detail::version_checker::home_env_name, missing.c_str(), 1), 0);
    ASSERT_EQ(setenv("TMPDIR", tmp_directory.c_str(), 1), 0);

    EXPECT_EQ(app_tmp_path(), tmp_directory);

    auto [out, err, app_call_succeeded] = simulate_parser("-f");

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");
    EXPECT_FALSE(app_call_succeeded);
    EXPECT_TRUE(std::filesystem::exists(tmp_directory / app_timestamp_filename().filename()));
}

TEST_F(version_check_test, option_off_no_directory_created)
{
    std::filesystem::path const home = tmp_file.get_path().parent_path() / "home";
    std::filesystem::create_directory(home);
    ASSERT_EQ(setenv(sharg::detail::version_checker::home_env_name, home.c_str(), 1), 0);

    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_OFF, "-f");

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");
    EXPECT_FALSE(app_call_succeeded);
    // The directory for the timestamp and version files is only determined (and created) if needed.
    EXPECT_FALSE(std::filesystem::exists(home / ".config"));
}

//------------------------------------------------------------------------------
// shell quoting
//------------------------------------------------------------------------------

#if !defined(_WIN32)
TEST_F(version_check_test, quote_argument)
{
    std::filesystem::path const tmp_directory = tmp_file.get_path().parent_path();
    std::filesystem::path const out_file = tmp_directory / "quoted.out";
    std::string const marker = (tmp_directory / "injected").string();

    for (std::string const & argument : {std::string{"with space"},
                                         std::string{"it's"},
                                         std::string{"''"},
                                         "a;touch " + marker + ";b",
                                         "$(touch " + marker + ")",
                                         "`touch " + marker + "`",
                                         std::string{"\"double\" \\back\\slash $HOME *"},
                                         std::string{"line\nbreak"}})
    {
        std::string const command = "printf '%s' " + sharg::detail::quote_argument(argument) + " > "
                                  + sharg::detail::quote_argument(out_file.string());
        ASSERT_EQ(std::system(command.c_str()), 0) << command;

        std::ifstream file{out_file};
        EXPECT_EQ((std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}}), argument);
    }

    EXPECT_FALSE(std::filesystem::exists(marker));
}
#endif // !defined(_WIN32)

// The path of the version file is derived from the home directory and passed to the shell.
TEST_F(version_check_test, home_with_shell_metacharacters)
{
    // Without wget or curl, no command is executed and the test would pass regardless of the quoting.
    if (std::system("/usr/bin/env -i wget --version > /dev/null 2>&1")
        && std::system("/usr/bin/env -i curl --version > /dev/null 2>&1"))
        GTEST_SKIP() << "Neither wget nor curl is available.";

    std::filesystem::path const tmp_directory = tmp_file.get_path().parent_path();
    std::filesystem::path const marker = tmp_directory / "injected";
    std::filesystem::path const home = tmp_directory / ("home;touch " + marker.string() + ";x");

    // The home directory must exist, otherwise the tmp directory is used.
    std::filesystem::create_directories(home);
    ASSERT_EQ(setenv(sharg::detail::version_checker::home_env_name, home.c_str(), 1), 0);
    ASSERT_TRUE(app_tmp_path().string().starts_with(home.string())) << app_tmp_path();

    auto [out, err, app_call_succeeded] = simulate_parser(OPTION_VERSION_CHECK, OPTION_ON, "-f");

    EXPECT_EQ(out, "");
    EXPECT_EQ(err, "");
    EXPECT_FALSE(std::filesystem::exists(marker));

    if (app_call_succeeded)
    {
        EXPECT_TRUE(std::filesystem::exists(app_version_filename()));
    }

    EXPECT_TRUE(remove_files_from_path()); // clear files again
}
