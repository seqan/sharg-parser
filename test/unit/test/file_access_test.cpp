// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <sharg/test/file_access.hpp>

struct file_access_test : public ::testing::Test
{
    static std::filesystem::path create_file(char const * const file_name)
    {
        std::filesystem::path file = std::filesystem::temp_directory_path();
        file /= file_name;

        std::ofstream str{file};

        return file;
    }

    static std::filesystem::path create_directory(char const * const directory_name)
    {
        std::filesystem::path directory = std::filesystem::temp_directory_path();
        directory /= directory_name;

        std::filesystem::create_directory(directory);

        return directory;
    }

    static void remove_read_permission(std::filesystem::path const & path)
    {
        std::filesystem::permissions(path, std::filesystem::perms::owner_read, std::filesystem::perm_options::remove);
    }

    static void remove_write_permission(std::filesystem::path const & path)
    {
        std::filesystem::permissions(path, std::filesystem::perms::owner_write, std::filesystem::perm_options::remove);
    }

    // If we have root permissions, we can write to the file even when we do not have write permissions.
    static bool is_root()
    {
        std::filesystem::path tmp_file = std::filesystem::temp_directory_path();
        tmp_file /= "sharg_test_permissions_is_root.txt";

        {
            std::fstream stream{tmp_file, std::ios::out};
        }

        remove_write_permission(tmp_file);

        bool result{};

        {
            std::fstream stream;
            stream.open(tmp_file, std::ios::out);
            result = !stream.fail();
        }

        std::filesystem::remove(tmp_file);

        return result;
    }
};

TEST_F(file_access_test, file_read_access_granted)
{
    auto path = create_file("sharg_test_permissions_file_read_access_granted");
    EXPECT_TRUE(sharg::test::read_access(path));
    std::filesystem::remove(path);
}

TEST_F(file_access_test, file_read_access_revoked)
{
    auto path = create_file("sharg_test_permissions_file_read_access_revoked");
    remove_read_permission(path);
    EXPECT_EQ(sharg::test::read_access(path), is_root());
    std::filesystem::remove(path);
}

TEST_F(file_access_test, file_write_access_granted)
{
    auto path = create_file("sharg_test_permissions_file_write_access_granted");
    EXPECT_TRUE(sharg::test::write_access(path));
    std::filesystem::remove(path);
}

TEST_F(file_access_test, file_write_access_revoked)
{
    auto path = create_file("sharg_test_permissions_file_write_access_revoked");
    remove_write_permission(path);
    EXPECT_EQ(sharg::test::write_access(path), is_root());
    std::filesystem::remove(path);
}

TEST_F(file_access_test, directory_write_access_granted)
{
    auto path = create_directory("sharg_test_permissions_directory_write_access_granted");
    EXPECT_TRUE(sharg::test::write_access(path));
    std::filesystem::remove(path);
}

TEST_F(file_access_test, directory_write_access_revoked)
{
    auto path = create_directory("sharg_test_permissions_directory_write_access_revoked");
    remove_write_permission(path);
    EXPECT_EQ(sharg::test::write_access(path), is_root());
    std::filesystem::remove(path);
}
