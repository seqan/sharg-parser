// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    sharg::parser myparser{"Game-of-Parsing", argc, argv, sharg::update_notifications::off};
    // disable update notifications permanently ----------------------------^
}
