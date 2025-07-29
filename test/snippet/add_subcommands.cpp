// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

void run(std::vector<std::string> const & arguments)
{
    sharg::parser git_parser{"git", arguments, sharg::update_notifications::off, {"pull", "push"}};
    git_parser.add_subcommands({"remote"});
    git_parser.parse();

    sharg::parser & sub_parser = git_parser.get_sub_parser();

    if (sub_parser.info.app_name == std::string_view{"git-pull"})
    {
        auto & pull_parser = sub_parser;
        std::string repository{};
        pull_parser.add_positional_option(repository, sharg::config{});
        pull_parser.parse();
    }
    else if (sub_parser.info.app_name == std::string_view{"git-push"})
    {
        auto & push_parser = sub_parser;
        std::string repository{};
        push_parser.add_positional_option(repository, sharg::config{});
        push_parser.parse();
    }
    else if (sub_parser.info.app_name == std::string_view{"git-remote"})
    {
        auto & remote_parser = sub_parser;
        remote_parser.add_subcommands({"set-url", "show"});
        remote_parser.parse();

        sharg::parser & recursive_sub_parser = remote_parser.get_sub_parser();

        if (recursive_sub_parser.info.app_name == std::string_view{"git-remote-set-url"})
        {
            auto & set_url_parser = recursive_sub_parser;
            std::string repository{};
            set_url_parser.add_positional_option(repository, sharg::config{});
            set_url_parser.parse();
        }
        else if (recursive_sub_parser.info.app_name == std::string_view{"git-remote-show"})
        {
            auto & show_parser = recursive_sub_parser;
            show_parser.parse();
        }
    }
}

int main(int argc, char ** argv)
{
    try
    {
        run({argv, argv + argc});
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "[Error] " << ext.what() << '\n';
        std::exit(-1);
    }

    return 0;
}
