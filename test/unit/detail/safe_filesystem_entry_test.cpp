// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

#include <gtest/gtest.h>

#include <fstream>

#include <sharg/detail/safe_filesystem_entry.hpp>
#include <sharg/test/tmp_filename.hpp>

TEST(safe_filesystem_entry, file)
{
    sharg::test::tmp_filename tmp_file{"dummy.txt"};
    std::filesystem::path const & my_file = tmp_file.get_path();
    {
        std::ofstream file{my_file};
        EXPECT_TRUE(std::filesystem::exists(my_file));
        sharg::detail::safe_filesystem_entry file_guard{my_file};
    }

    EXPECT_FALSE(std::filesystem::exists(my_file));
}

TEST(safe_filesystem_entry, file_already_removed)
{
    sharg::test::tmp_filename tmp_file{"dummy.txt"};
    std::filesystem::path const & my_file = tmp_file.get_path();
    {
        EXPECT_FALSE(std::filesystem::exists(my_file));
        sharg::detail::safe_filesystem_entry file_guard{my_file};
    }

    EXPECT_FALSE(std::filesystem::exists(my_file));
}

TEST(safe_filesystem_entry, directory)
{
    sharg::test::tmp_filename tmp_file{"dummy.txt"};
    std::filesystem::path const & my_dir = tmp_file.get_path();
    {
        std::filesystem::create_directory(my_dir);
        EXPECT_TRUE(std::filesystem::exists(my_dir));
        sharg::detail::safe_filesystem_entry dir_guard{my_dir};
    }

    EXPECT_FALSE(std::filesystem::exists(my_dir));
}

TEST(safe_filesystem_entry, directory_already_removed)
{
    sharg::test::tmp_filename tmp_file{"dummy.txt"};
    std::filesystem::path const & my_dir = tmp_file.get_path();
    {
        EXPECT_FALSE(std::filesystem::exists(my_dir));
        sharg::detail::safe_filesystem_entry dir_guard{my_dir};
    }

    EXPECT_FALSE(std::filesystem::exists(my_dir));
}

TEST(safe_filesystem_entry, remove)
{
    sharg::test::tmp_filename tmp_file{"dummy.txt"};
    std::filesystem::path const & my_file = tmp_file.get_path();
    {
        std::ofstream file{my_file};
        EXPECT_TRUE(std::filesystem::exists(my_file));
        sharg::detail::safe_filesystem_entry file_guard{my_file};
        EXPECT_TRUE(file_guard.remove());
    }

    EXPECT_FALSE(std::filesystem::exists(my_file));
}

TEST(safe_filesystem_entry, remove_all)
{
    sharg::test::tmp_filename tmp_file{"dummy.txt"};
    std::filesystem::path const & my_dir = tmp_file.get_path();
    {
        std::filesystem::create_directory(my_dir);
        EXPECT_TRUE(std::filesystem::exists(my_dir));
        sharg::detail::safe_filesystem_entry dir_guard{my_dir};
        EXPECT_TRUE(dir_guard.remove_all());
    }

    EXPECT_FALSE(std::filesystem::exists(my_dir));
}
