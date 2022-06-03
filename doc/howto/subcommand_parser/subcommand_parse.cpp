#include <sharg/all.hpp>

// =====================================================================================================================
// pull
// =====================================================================================================================

struct pull_arguments
{
    std::string repository{};
    std::string branch{};
    bool progress{false};
};

int run_git_pull(sharg::parser & parser)
{
    pull_arguments args{};

    parser.add_positional_option(args.repository, "The repository name to pull from.");
    parser.add_positional_option(args.branch, "The branch name to pull from.");

    try
    {
        parser.parse();
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "[Error git pull] " << ext.what() << "\n";
        return -1;
    }

    std::cerr << "Git pull with repository " << args.repository << " and branch " << args.branch << '\n';

    return 0;
}

// =====================================================================================================================
// push
// =====================================================================================================================

struct push_arguments
{
    std::string repository{};
    std::vector<std::string> branches{};
    bool push_all{false};
};

int run_git_push(sharg::parser & parser)
{
    push_arguments args{};

    parser.add_positional_option(args.repository, "The repository name to push to.");
    parser.add_positional_option(args.branches, "The branch names to push (if none are given, push current).");

    try
    {
        parser.parse();
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "[Error git push] " << ext.what() << "\n";
        return -1;
    }

    std::cerr << "Git push with repository " << args.repository << " and branches ";
    for (auto && branch : args.branches)
        std::cerr << branch << ' ';
    std::cerr << '\n';

    return 0;
}

// =====================================================================================================================
// main
// =====================================================================================================================

int main(int argc, char const ** argv)
{
    //![construction]
    sharg::parser top_level_parser{"mygit", argc, argv, sharg::update_notifications::on, {"push", "pull"}};
    //![construction]

    // Add information and flags, but no (positional) options to your top-level parser.
    // Because of ambiguity, we do not allow any (positional) options for the top-level parser.
    top_level_parser.info.description.push_back("You can push or pull from a remote repository.");
    bool flag{false};
    top_level_parser.add_flag(flag, 'f', "flag", "some flag");

    try
    {
        top_level_parser.parse(); // trigger command line parsing
    }
    catch (sharg::parser_error const & ext) // catch user errors
    {
        std::cerr << "[Error] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    //![get_sub_parser]
    sharg::parser & sub_parser = top_level_parser.get_sub_parser(); // hold a reference to the sub_parser
    //![get_sub_parser]

    std::cout << "Proceed to sub parser.\n";

    if (sub_parser.info.app_name == std::string_view{"mygit-pull"})
        return run_git_pull(sub_parser);
    else if (sub_parser.info.app_name == std::string_view{"mygit-push"})
        return run_git_push(sub_parser);
    else
        std::cout << "Unhandled subparser named " << sub_parser.info.app_name << '\n';
    // Note: Arriving in this else branch means you did not handle all sub_parsers in the if branches above.

    return 0;
}
