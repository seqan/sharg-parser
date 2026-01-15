// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

int main(int argc, char const ** argv)
{
    sharg::parser myparser{"Test", argc, argv}; // initialize

    //![validator_call]
    std::string my_string;
    sharg::regex_validator my_validator{"[a-zA-Z]+@[a-zA-Z]+\\.com"};

    myparser.add_option(my_string,
                        sharg::config{.short_id = 's',
                                      .long_id = "str",
                                      .description = "Give me a string.",
                                      .validator = my_validator});
    //![validator_call]

    // an exception will be thrown if the user specifies a string
    // that is no email address ending on .com
    try
    {
        myparser.parse();
    }
    catch (sharg::parser_error const & ext) // the user did something wrong
    {
        std::cerr << "[PARSER ERROR] " << ext.what() << "\n"; // customize your error message
        return -1;
    }

    std::cerr << "email address given by user passed validation: " << my_string << "\n";
    return 0;
}
