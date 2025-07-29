// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

int main(int argc, char const ** argv)
{
    sharg::parser myparser{"Test", argc, argv}; // initialize

    //! [validator_call]
    std::filesystem::path mydir{};

    myparser.add_option(mydir,
                        sharg::config{.short_id = 'd',
                                      .long_id = "dir",
                                      .description = "The directory containing the input files.",
                                      .validator = sharg::input_directory_validator{}});
    //! [validator_call]

    // an exception will be thrown if the user specifies a directory that does not exists or has insufficient
    // read permissions.
    try
    {
        myparser.parse();
    }
    catch (sharg::parser_error const & ext) // the user did something wrong
    {
        std::cerr << "[PARSER ERROR] " << ext.what() << "\n"; // customize your error message
        return -1;
    }

    std::cerr << "directory given by user passed validation: " << mydir << "\n";
    return 0;
}
