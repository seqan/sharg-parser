// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/parser.hpp>
//![validator]
#include <cmath>

struct custom_validator
{
    using option_value_type = double; // used for all arithmetic types

    void operator()(option_value_type const & val) const
    {
        if ((std::round(val) != val) ||                       // not an integer
            (std::pow(std::round(std::sqrt(val)), 2) != val)) // not a square
        {
            throw sharg::validation_error{"The provided number is not an arithmetic square."};
        }
    }

    std::string get_help_page_message() const
    {
        return "Value must be the square of an integral number.";
    }
};
//![validator]

static_assert(sharg::validator<custom_validator>);

//![main]
int main(int argc, char ** argv)
{
    sharg::parser myparser("Test-Parser", argc, argv);

    int32_t variable{};
    int16_t variable2{};

    myparser.add_option(variable,
                        sharg::config{.short_id = 'i',
                                      .description = "An int that is a square",
                                      .validator = custom_validator{}}); // ← your validator is used!

    myparser.add_option(variable2,
                        sharg::config{.short_id = 'j',
                                      .description = "An int that is a square and within [0,20].",
                                      .validator = custom_validator{}
                                                 | sharg::arithmetic_range_validator{0, 20}}); // ← now it's chained

    try
    {
        myparser.parse(); // trigger command line parsing
    }
    catch (sharg::parser_error const & ext)
    {
        std::cout << ext.what() << '\n';
        return -1;
    }
    std::cout << "Yeah!\n";

    return 0;
}
//![main]
