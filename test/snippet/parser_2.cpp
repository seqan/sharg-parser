// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    sharg::parser myparser{"The-Age-App", argc, argv}; // initialize

    int age{30}; // define default values directly in the variable

    myparser.add_option(age, sharg::config{.short_id = 'a', .long_id = "user-age", .description = "Your age please."});

    try
    {
        myparser.parse();
    }
    catch (sharg::parser_error const & ext) // the user did something wrong
    {
        std::cerr << "The-Age-App - [PARSER ERROR] " << ext.what() << '\n'; // customize your error message
        return -1;
    }

    std::cerr << "integer given by user: " << age << '\n';
    return 0;
}
