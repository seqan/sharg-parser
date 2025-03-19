// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \brief Provides sharg::test::test_fixture.
 * \author Enrico Seiler <enrico.seiler AT fu-berlin.de>
 */

#pragma once

#include <gtest/gtest.h>

#include <sharg/parser.hpp>

namespace sharg::detail
{

struct test_accessor
{
    static void set_terminal_width(sharg::parser & parser, unsigned terminal_width)
    {
        auto visit_fn = [terminal_width]<typename format_t>(format_t & format)
        {
            if constexpr (std::same_as<format_t, sharg::detail::format_help>)
                format.layout = sharg::detail::format_help::console_layout_struct{terminal_width};
        };

        std::visit(std::move(visit_fn), parser.format);
    }

    static std::vector<std::string> & executable_name(sharg::parser & parser)
    {
        return parser.executable_name;
    }

    static auto & version_check_future(sharg::parser & parser)
    {
        return parser.version_check_future;
    }
};

} // namespace sharg::detail

namespace sharg::test
{

class test_fixture : public ::testing::Test
{
private:
    friend class early_exit_guardian;

    static sharg::parser impl(std::vector<std::string> arguments, std::vector<std::string> subcommands = {})
    {
        sharg::parser parser{"test_parser",
                             std::move(arguments),
                             sharg::update_notifications::off,
                             std::move(subcommands)};
        sharg::detail::test_accessor::set_terminal_width(parser, 80u);
        return parser;
    }

    static void toggle_guardian();

protected:
    template <typename... arg_ts>
    static sharg::parser get_parser(arg_ts &&... arguments)
    {
        return impl(std::vector<std::string>{"./test_parser", std::forward<arg_ts>(arguments)...});
    }

    static sharg::parser get_subcommand_parser(std::vector<std::string> arguments, std::vector<std::string> subcommands)
    {
        arguments.insert(arguments.begin(), "./test_parser");
        return impl(std::move(arguments), std::move(subcommands));
    }

    static std::string get_parse_cout_on_exit(sharg::parser & parser)
    {
        testing::internal::CaptureStdout();
        // EXPECT_EXIT will create a new thread via clone() and the destructor of the cloned early_exit_guardian will
        // be called. So we need to toggle the guardian to prevent the check inside the cloned thread, and toggle
        // it back after the EXPECT_EXIT call.
        toggle_guardian();
        EXPECT_EXIT(parser.parse(), ::testing::ExitedWithCode(EXIT_SUCCESS), "");
        toggle_guardian();
        return testing::internal::GetCapturedStdout();
    }

    static inline std::string basic_options_str = "  Common options\n"
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

    static inline std::string version_str(std::string_view const subcommand_with_dash)
    {
        return std::string{"VERSION\n"}
             + "    Last update:\n"
               "    test_parser"
             + subcommand_with_dash.data()
             + " version:\n"
               "    Sharg version: "
             + sharg::sharg_version_cstring + '\n';
    }

    static inline std::string version_str()
    {
        return version_str("");
    }
};

class early_exit_guardian
{
public:
    early_exit_guardian()
    {
        ::testing::AddGlobalTestEnvironment(new test_environment{this});
    }
    early_exit_guardian(early_exit_guardian const &) = delete;
    early_exit_guardian(early_exit_guardian &&) = delete;
    early_exit_guardian & operator=(early_exit_guardian const &) = delete;
    early_exit_guardian & operator=(early_exit_guardian &&) = delete;

    ~early_exit_guardian()
    {
        restore_stderr();
        check_all_tests_ran();
    }

private:
    //!\brief test_fixture::toggle_guardian is allowed to toggle `active` for `EXPECT_EXIT` tests.
    friend void test_fixture::toggle_guardian();
    //!\brief Flag to indicate that all tests are done.
    bool active{false};
    //!\brief Original file descriptor of stderr.
    int const stored_fd = dup(2);

    /*!\brief Pointer to the current test unit.
     * GetInstance() creates the object on the first call and just return that pointer on subsequent calls (static).
     * It is important that this first call takes place on the "main" thread, and not in the destructor call.
     */
    testing::UnitTest const * const unit_test = testing::UnitTest::GetInstance();

    void activate()
    {
        assert(!active);
        active = true;
    }

    void deactivate()
    {
        assert(active);
        active = false;
    }

    /*!\brief Restore the original file descriptor of stderr.
     * testing::internal::CaptureStderr() manipulates the file descriptor of stderr.
     * There is no API (neither exposed nor internal) to check whether stderr is captured or not, and
     * testing::internal::GetCapturedStderr() is UB if stderr is not captured.
     * Therefore, we restore the original file descriptor of stderr by ourself.
     */
    void restore_stderr() const
    {
        dup2(stored_fd, 2);
    }

    /*!\brief Check if all tests were run.
     * Exits with EXIT_FAILURE if `sharg::test::early_exit_guard.deactivate()` was not called.
     */
    void check_all_tests_ran() const
    {
        if (!active)
            return;

        // LCOV_EXCL_START
        std::cerr << "\nNot all test cases were run!\n"
                  << "The following test unexpectedly terminated the execution:\n"
                  << get_current_test_name() << '\n';

        std::exit(EXIT_FAILURE);
        // LCOV_EXCL_STOP
    }

    // LCOV_EXCL_START
    std::string get_current_test_name() const
    {
        assert(unit_test && "This should never be a nullptr?!");

        std::string result{};

        if (::testing::TestSuite const * const test_suite = unit_test->current_test_suite())
        {
            result = test_suite->name();
            result += '.';
        }

        if (::testing::TestInfo const * const test_info = unit_test->current_test_info())
            result += test_info->name();

        return result;
    }
    // LCOV_EXCL_STOP

    // See https://github.com/google/googletest/blob/main/docs/advanced.md#global-set-up-and-tear-down
    class test_environment : public ::testing::Environment
    {
    private:
        friend class early_exit_guardian;
        early_exit_guardian * guardian{nullptr};

        test_environment(early_exit_guardian * guardian) : guardian{guardian}
        {}

    public:
        void SetUp() override
        {
            assert(guardian);
            guardian->activate();
        }

        void TearDown() override
        {
            assert(guardian);
            guardian->deactivate();
        }
    };
};

early_exit_guardian early_exit_guard{}; // NOLINT(misc-definitions-in-headers)

inline void test_fixture::toggle_guardian()
{
    early_exit_guard.active = !early_exit_guard.active;
}

} // namespace sharg::test
