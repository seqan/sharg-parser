// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>

#include <sharg/validators.hpp>

int main()
{
    // Default constructed validator has an empty extension list.
    sharg::output_file_validator validator1{};
    std::cerr << validator1.get_help_page_message() << '\n';

    // Specify your own extensions for the output file.
    sharg::output_file_validator validator2{sharg::output_file_open_options::create_new,
                                            std::vector{std::string{"exe"}, std::string{"fasta"}}};
    std::cerr << validator2.get_help_page_message() << '\n';

    // If you do not pass the output_file_open_options, the default of create_new is used.
    sharg::output_file_validator validator3{std::vector{std::string{"exe"}, std::string{"fasta"}}};
    std::cerr << validator3.get_help_page_message() << '\n';

    // Instead of passing a std::vector<std::string>, you can also pass each extension as separate argument.
    sharg::output_file_validator validator4{"exe", "fasta"};
    std::cerr << validator4.get_help_page_message() << '\n';

    return 0;
}
