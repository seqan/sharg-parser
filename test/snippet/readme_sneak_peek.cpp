// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#define main test

#include <sharg/all.hpp>

#if 0
int main(int argc, char ** argv)
{
    //                                                                           -------- Optional --------
    sharg::parser git_parser{"git", argc, argv, sharg::update_notifications::on, {"pull", "push", "remote"}};

    if (auto pull_parser = git_parser.add_subcommand("pull"))
    {
        std::string repository{};
        pull_parser->add_positional_option(repository, sharg::config{.description = "pull"});
        pull_parser->parse();
    }
    else if (auto push_parser = git_parser.add_subcommand("push"))
    {
        std::string repository{};
        push_parser->add_positional_option(repository, sharg::config{.description = "push"});
        push_parser->parse();
    }
    else if (auto remote_parser = git_parser.add_subcommand("remote"))
    {
        remote_parser->add_section("Remote options");
        // remote_parser->parse(); // Caveat
        if (auto recursive_sub_parser = remote_parser->add_subcommand("set-url"))
        {
            std::string repository{};
            recursive_sub_parser->add_positional_option(repository, sharg::config{});
            recursive_sub_parser->parse();
        }
        else if (auto recursive_sub_parser = remote_parser->add_subcommand("show"))
        {
            recursive_sub_parser->parse();
        }
        else if (auto recursive_sub_parser = remote_parser->add_subcommand("remote"))
        {
            recursive_sub_parser->parse();
        }
        else
        {
            remote_parser->parse();
        }
    }
    else
    {
        git_parser.parse();
    }

    return 0;
}
#else
int main(int argc, char ** argv)
{
    int val{};

    sharg::parser parser{"Eat-Me-App", argc, argv};
    parser.add_subsection("Eating Numbers");
    parser.add_option(val, sharg::config{.short_id = 'i', .long_id = "int", .description = "Desc."});
    parser.parse();

    return 0;
}
#endif

#undef main

int main()
{
    std::string argv1{"./Eat-Me-App"}, argv2{"-h"};
    std::array argv{argv1.data(), argv2.data()};

    return test(argv.size(), argv.data());
}
