// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

// external type, i.e., you cannot change the implementation
namespace external
{

class bar
{
public:
    int a;
};

} // namespace external

namespace std
{

// Make external::bar satisfy sharg::ostreamable
ostream & operator<<(ostream & output, external::bar const & ext_bar)
{
    output << ext_bar.a; // Adapt this for your type
    return output;
}

// Make external::bar satisfy sharg::istreamable
istream & operator>>(istream & input, external::bar & ext_bar)
{
    input >> ext_bar.a; // Adapt this for your type
    return input;
}

} // namespace std

int main(int argc, char const ** argv)
{
    sharg::parser parser{"my_ext_bar_parser", argc, argv, sharg::update_notifications::off};

    external::bar ext_bar{};
    parser.add_option(ext_bar, sharg::config{.short_id = 'f', .long_id = "ext-bar", .description = "Supply an int."});

    try
    {
        parser.parse(); // trigger command line parsing
    }
    catch (sharg::parser_error const & ext) // catch user errors
    {
        std::cerr << "[Error] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    std::cout << "ext_bar was initialised with a = " << ext_bar.a << std::endl;

    return 0;
}
