// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/parser.hpp>

struct custom_validator
{
    using option_value_type = double; // used for all arithmetic types

    void operator()(option_value_type const &) const
    {
        // add implementation later
    }

    std::string get_help_page_message() const
    {
        // add real implementation later
        return "";
    }
};

static_assert(sharg::validator<custom_validator>); // does not cause compile error

int main()
{}
