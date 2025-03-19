// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

// my custom type
namespace foo
{

class bar
{
public:
    int a;

    // Make foo::bar satisfy sharg::ostreamable
    friend std::ostream & operator<<(std::ostream & output, bar const & my_bar)
    {
        output << my_bar.a; // Adapt this for your type
        return output;
    }

    // Make foo::bar satisfy sharg::istreamable
    friend std::istream & operator>>(std::istream & input, bar & my_bar)
    {
        input >> my_bar.a; // Adapt this for your type
        return input;
    }
};

} // namespace foo

int main(int argc, char const ** argv)
{
    sharg::parser parser{"my_foobar_parser", argc, argv, sharg::update_notifications::off};

    foo::bar my_bar{};
    parser.add_option(my_bar,
                      sharg::config{.short_id = 'f', .long_id = "foo-bar", .description = "Supply an integer."});

    try
    {
        parser.parse(); // trigger command line parsing
    }
    catch (sharg::parser_error const & ext) // catch user errors
    {
        std::cerr << "[Error] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    std::cout << "my_bar was initialised with a = " << my_bar.a << '\n';

    return 0;
}
