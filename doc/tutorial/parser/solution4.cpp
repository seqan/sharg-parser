// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp> // includes all necessary headers

// This is the program!
// Take a look at it if you are interested in an example of parsing a data file.
// -----------------------------------------------------------------------------
template <typename number_type, typename range_type>
number_type to_number(range_type && range)
{
    std::string str = [&range]()
    {
        std::string s;
        for (auto c : range)
            s.push_back(c);
        return s;
    }();
    number_type num;
    auto res = std::from_chars(str.data(), str.data() + str.size(), num);

    if (res.ec != std::errc{})
    {
        std::cerr << "Could not cast '" << str << "' to a valid number\n";
        throw std::invalid_argument{"CAST ERROR"};
    }
    return num;
}

void run_program(std::filesystem::path & path, std::vector<uint8_t> sn, std::string & aggr_by, bool hd_is_set)
{
    std::ifstream file{path.string()};

    if (file.is_open())
    {
        std::vector<double> v;
        std::string line;

        if (hd_is_set)
            std::getline(file, line); // ignore first line

        //![altered_while]
        while (std::getline(file, line))
        {
            auto splitted_line = line | std::views::split('\t');
            auto it = splitted_line.begin(); // move to 1rst column

            if (std::find(sn.begin(), sn.end(), to_number<uint8_t>(*it)) != sn.end())
                v.push_back(to_number<double>(*std::next(it, 4)));
        }
        //![altered_while]

        if (aggr_by == "median")
            std::cerr << ([&v] () { std::sort(v.begin(), v.end()); return v[v.size()/2]; })() << '\n';
        else if (aggr_by == "mean")
            std::cerr << ([&v] () { double sum{}; for (auto i : v) sum += i; return sum / v.size(); })()
                                 << '\n';
        else
            std::cerr << "I do not know the aggregation method " << aggr_by << '\n';
    }
    else
    {
        std::cerr << "Error: Cannot open file for reading.\n";
    }
}
// -----------------------------------------------------------------------------

//![solution]
struct cmd_arguments
{
    std::filesystem::path file_path{};
    std::vector<uint8_t> seasons{};
    std::string aggregate_by{"mean"};
    bool header_is_set{false};
};

void initialise_parser(sharg::parser & parser, cmd_arguments & args)
{
    parser.info.author = "Cercei";
    parser.info.short_description = "Aggregate average Game of Thrones viewers by season.";
    parser.info.version = "1.0.0";

    parser.add_positional_option(args.file_path,
                                 sharg::config{.description = "Please provide a tab separated data file."});

    parser.add_option(
        args.seasons,
        sharg::config{.short_id = 's', .long_id = "season", .description = "Choose the seasons to aggregate."});

    parser.add_option(args.aggregate_by,
                      sharg::config{.short_id = 'a',
                                    .long_id = "aggregate-by",
                                    .description = "Choose your method of aggregation: mean or median."});

    parser.add_flag(
        args.header_is_set,
        sharg::config{.short_id = 'H',
                      .long_id = "header-is-set",
                      .description =
                          "Let us know whether your data file contains a header to ensure correct parsing."});
}
//![solution]

int main(int argc, char ** argv)
{
    sharg::parser myparser{"Game-of-Parsing", argc, argv}; // initialise myparser
    cmd_arguments args{};

    initialise_parser(myparser, args);

    try
    {
        myparser.parse(); // trigger command line parsing
    }
    catch (sharg::parser_error const & ext) // catch user errors
    {
        std::cerr << "[Winter has come] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    // parsing was successful !
    // we can start running our program
    run_program(args.file_path, args.seasons, args.aggregate_by, args.header_is_set);

    return 0;
}
