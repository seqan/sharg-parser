// SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <iostream>

#include <sharg/validators.hpp>

int main()
{
    // Default constructed validator has an empty extension list.
    sharg::input_file_validator validator1{};
    std::cerr << validator1.get_help_page_message() << '\n';

    // Specify your own extensions for the input file.
    sharg::input_file_validator validator2{std::vector{std::string{"exe"}, std::string{"fasta"}}};
    std::cerr << validator2.get_help_page_message() << '\n';

    return 0;
}
