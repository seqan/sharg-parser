// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

void run_program(std::filesystem::path const & reference_path, std::filesystem::path const & index_path)
{
    std::cerr << "reference_file_path: " << reference_path << '\n';
    std::cerr << "index_path           " << index_path << '\n';
}

struct cmd_arguments
{
    std::filesystem::path reference_path{};
    std::filesystem::path index_path{"out.index"};
};

void initialise_parser(sharg::parser & parser, cmd_arguments & args)
{
    parser.info.author = "E. coli";
    parser.info.short_description = "Creates an index over a reference.";
    parser.info.version = "1.0.0";
    parser.add_option(args.reference_path,
                      sharg::config{.short_id = 'r',
                                    .long_id = "reference",
                                    .description = "The path to the reference.",
                                    .required = true,
                                    .validator = sharg::input_file_validator{{"fa", "fasta"}}});
    parser.add_option(
        args.index_path,
        sharg::config{.short_id = 'o',
                      .long_id = "output",
                      .description = "The output index file path.",
                      .validator =
                          sharg::output_file_validator{sharg::output_file_open_options::create_new, {"index"}}});
}

//![main]
int main(int argc, char const ** argv)
{
    sharg::parser parser("Indexer", argc, argv);
    cmd_arguments args{};

    initialise_parser(parser, args);

    try
    {
        parser.parse();
    }
    catch (sharg::parser_error const & ext)
    {
        std::cerr << "[PARSER ERROR] " << ext.what() << '\n';
        return -1;
    }

    run_program(args.reference_path, args.index_path);

    return 0;
}
//![main]
