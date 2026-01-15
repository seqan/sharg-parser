// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    sharg::parser myparser{"Penguin_Parade", argc, argv}; // initialize

    myparser.info.version = "2.0.0";
    myparser.info.date = "12.01.2017";
    myparser.info.short_description = "Organize your penguin parade";
    myparser.info.description.push_back("First Paragraph.");
    myparser.info.description.push_back("Second Paragraph.");
    myparser.info.examples.push_back("./penguin_parade Skipper Kowalski Rico Private -d 10 -m 02 -y 2017");

    int d{01};   // day
    int m{01};   // month
    int y{2050}; // year

    myparser.add_option(d, sharg::config{.short_id = 'd', .long_id = "day", .description = "Your preferred day."});
    myparser.add_option(m, sharg::config{.short_id = 'm', .long_id = "month", .description = "Your preferred month."});
    myparser.add_option(y, sharg::config{.short_id = 'y', .long_id = "year", .description = "Your preferred year."});

    std::vector<std::string> penguin_names;

    myparser.add_positional_option(penguin_names, sharg::config{.description = "Specify the names of the penguins."});

    try
    {
        myparser.parse();
    }
    catch (sharg::parser_error const & ext) // the user did something wrong
    {
        std::cerr << ext.what() << "\n";
        return -1;
    }

    // organize ...

    return 0;
}
