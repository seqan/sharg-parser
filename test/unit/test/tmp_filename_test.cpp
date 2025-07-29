// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <sharg/test/file_access.hpp>
#include <sharg/test/tmp_filename.hpp>

namespace fs = std::filesystem;

// aggregate initialization
TEST(tmp_filename_aggr, aggr)
{
    sharg::test::tmp_filename t1{"aggr_test"};
    sharg::test::tmp_filename t2("aggr_test");
    EXPECT_NE(t1.get_path(), t2.get_path());
    EXPECT_TRUE(fs::exists(t1.get_path().parent_path()));
    EXPECT_TRUE(fs::exists(t2.get_path().parent_path()));

    EXPECT_TRUE(fs::equivalent(fs::temp_directory_path(), t1.get_path().parent_path().parent_path()));
    EXPECT_TRUE(fs::equivalent(fs::temp_directory_path(), t2.get_path().parent_path().parent_path()));
}

// nullptr as filename
TEST(tmp_filename_nullptr, null_ptr)
{
    EXPECT_THROW(sharg::test::tmp_filename t1{nullptr}, fs::filesystem_error);
    EXPECT_THROW(sharg::test::tmp_filename t1(nullptr), fs::filesystem_error);
}

// move construction
TEST(tmp_filename_mv_ctr, mv_ctr)
{
    sharg::test::tmp_filename t1{"mv_ctr_test"};
    sharg::test::tmp_filename t2{"mv_ctr_test"};
    sharg::test::tmp_filename t3{std::move(t2)};
    EXPECT_NE(t1.get_path(), t3.get_path());
    sharg::test::tmp_filename t4(std::move(t1));
    EXPECT_NE(t3.get_path(), t4.get_path());
}

// move assignment
TEST(tmp_filename_mv_assign, mv_assign)
{
    sharg::test::tmp_filename t1{"mv_ctr_test"};
    sharg::test::tmp_filename t2{"mv_ctr_test"};
    sharg::test::tmp_filename t3 = std::move(t2);
    EXPECT_NE(t1.get_path(), t3.get_path());
}

// destructor
TEST(tmp_filename_dtr, dtr)
{
    auto t1 = std::make_unique<sharg::test::tmp_filename>("delete_test");
    auto path = t1->get_path();
    std::ofstream os{path, std::ios::out};
    os << "delete_test";
    os.close();
    EXPECT_TRUE(fs::exists(path));
    EXPECT_TRUE(fs::exists(path.parent_path()));
    t1.reset();
    EXPECT_FALSE(fs::exists(path));
    EXPECT_FALSE(fs::exists(path.parent_path()));
}

//!\brief A wrapper class to handle construction, destruction and permission handling of a temporary directory.
class read_only_directory
{
public:
    read_only_directory() = delete;                                        //< Deleted.
    read_only_directory(read_only_directory const &) = delete;             //< Deleted.
    read_only_directory(read_only_directory &&) = default;                 //< Defaulted.
    read_only_directory & operator=(read_only_directory const &) = delete; //< Deleted.
    read_only_directory & operator=(read_only_directory &&) = default;     //< Defaulted.

    /*!\brief Construct a directory from a given path.
     * \throws if std::filesystem::create_directory throws.
     * \throws std::filesystem::filesystem_error if the directory already exists.
     * \throws if std::filesystem::permissions throws.
     */
    explicit read_only_directory(std::filesystem::path path) : directory_path(std::move(path))
    {
        if (!std::filesystem::create_directory(directory_path))
        {
            throw std::filesystem::filesystem_error{"The read_only_directory path already exists",
                                                    directory_path,
                                                    std::make_error_code(std::errc::file_exists)};
        }
        std::filesystem::permissions(directory_path,
                                     std::filesystem::perms::owner_write,
                                     std::filesystem::perm_options::remove);
    }

    //!\brief Destructor tries to delete the directory.
    ~read_only_directory()
    {
        [[maybe_unused]] std::error_code ec;
        release_impl(ec);
    }

    /*!\brief Tries to remove the directory.
     * \throws if std::filesystem::permissions throws.
     * \throws if std::filesystem::remove throws.
     * \throws std::filesystem::filesystem_error if the directory does not exist.
     */
    void release() const
    {
        std::error_code ec;
        release_impl(ec);

        if (ec.value())
            throw std::filesystem::filesystem_error("Error while removing directory", directory_path, ec);
    }

private:
    //!/brief The path of the managed directory.
    std::filesystem::path directory_path;

    /*!\brief Tries to remove the directory.
     */
    void release_impl(std::error_code & ec) const noexcept
    {
        std::filesystem::permissions(directory_path,
                                     std::filesystem::perms::owner_write,
                                     std::filesystem::perm_options::add,
                                     ec);
        std::filesystem::remove(directory_path, ec);
    }
};

TEST(tmp_filename_throw, directory_not_writeable)
{
    try
    {
        // Create a directory in the temporary directory.
        std::filesystem::path test_path = std::filesystem::temp_directory_path();
        test_path /= "sharg_tmp_filename_throw";
        read_only_directory test_directory{test_path};

        // Set TMPDIR. This is the first env var that is looked up for `temp_directory_path` inside the `tmp_filename`.
        setenv("TMPDIR", test_path.c_str(), 1); // name, value, overwrite

        if (!sharg::test::write_access(test_path)) // Do not execute with root permissions.
        {
            EXPECT_THROW(sharg::test::tmp_filename t1{"throw"}, std::filesystem::filesystem_error);
        }

        // Remove directory.
        test_directory.release();
    }
    catch (std::exception const & e)
    {
        FAIL() << e.what();
    }
}
