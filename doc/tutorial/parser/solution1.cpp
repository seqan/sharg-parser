// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp> // includes all necessary headers

void initialise_parser(sharg::parser & parser)
{
    parser.info.author = "Cersei";
    parser.info.short_description = "Aggregate average Game of Thrones viewers by season.";
    parser.info.version = "1.0.0";
}

int main(int argc, char ** argv)
{
    sharg::parser myparser{"Game-of-Parsing", argc, argv};
    initialise_parser(myparser);
    // code from assignment 1
}
