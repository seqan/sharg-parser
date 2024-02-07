// SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <system_error>

#include <sharg/all.hpp>

namespace sharg::custom
{
// Specialise the sharg::custom::parsing data structure to enable parsing of std::errc.
template <>
struct parsing<std::errc>
{
    // Specialise a mapping from an identifying string to the respective value of your type Foo.
    static inline std::unordered_map<std::string_view, std::errc> const enumeration_names{
        {"no_error", std::errc{}},
        {"timed_out", std::errc::timed_out},
        {"invalid_argument", std::errc::invalid_argument},
        {"io_error", std::errc::io_error}};
};

} // namespace sharg::custom

int main(int argc, char const * argv[])
{
    std::errc value{};

    sharg::parser parser{"my_program", argc, argv};

    // Because of the parsing struct and
    // the static member function enumeration_names
    // you can now add an option that takes a value of type std::errc:
    auto validator = sharg::value_list_validator{(sharg::enumeration_names<std::errc> | std::views::values)};
    parser.add_option(value,
                      sharg::config{.short_id = 'e',
                                    .long_id = "errc",
                                    .description = "Give me a std::errc value.",
                                    .validator = validator});

    try
    {
        parser.parse();
    }
    catch (sharg::parser_error const & ext) // the user did something wrong
    {
        std::cerr << "[PARSER ERROR] " << ext.what() << "\n"; // customize your error message
        return -1;
    }

    return 0;
}
