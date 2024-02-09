#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    // sharg::parser git_parser{"git", argc, argv, sharg::update_notifications::on, {"pull", "push", "remote"}}; // NOT ALLOWED
    sharg::parser git_parser{"git", argc, argv, sharg::update_notifications::on};

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
