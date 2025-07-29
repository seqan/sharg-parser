// SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: CC0-1.0

#include <sharg/all.hpp>

//![validator_include]
#include <sharg/validators.hpp>

//![validator_include]

struct cmd_arguments
{
    std::filesystem::path file_path{};
    std::vector<uint8_t> seasons{};
    std::string aggregate_by{"mean"};
    bool header_is_set{};
};

cmd_arguments args{};

// clang-format off
int main(int argc, char ** argv)
{

{
sharg::parser parser{"Example-Parser", argc, argv};
//![add_positional_option]
size_t variable{};
parser.add_positional_option(variable, sharg::config{.description = "This is a description."});
//![add_positional_option]
}

{
sharg::parser parser{"Example-Parser", argc, argv};
//![add_option]
size_t variable{};
parser.add_option(variable, sharg::config{.short_id = 'n', .long_id = "my-num", .description = "A description."});
//![add_option]
}

{
sharg::parser parser{"Example-Parser", argc, argv};
//![add_flag]
bool variable{false};
parser.add_flag(variable, sharg::config{.short_id = 'f', .long_id = "my-flag", .description = "A description."});
//![add_flag]
}

{
sharg::parser parser{"Example-Parser", argc, argv};
//![option_list]
std::vector<std::string> list_variable{};
parser.add_option(list_variable, sharg::config{.short_id = 'n', .long_id = "names", .description = "Some names."});
//![option_list]
}

{
sharg::parser parser{"Example-Parser", argc, argv};
//![positional_option_list]
std::string variable{};
std::vector<std::string> list_variable{};
parser.add_positional_option(variable, sharg::config{.description = "Give me a single variable."});
parser.add_positional_option(list_variable, sharg::config{.description = "Give me one or more variables!."});
//![positional_option_list]
}

{
sharg::parser parser{"Example-Parser", argc, argv};
//![required_option]
std::string required_variable{};
parser.add_option(required_variable, sharg::config{.short_id = 'n', .long_id = "name",
                                                   .description = "I really need a name.", .required = true});
//![required_option]
}

{
sharg::parser parser{"Example-Parser", argc, argv};
//![input_file_validator]
parser.add_positional_option(args.file_path, sharg::config{.description = "Please provide a tab separated data file.",
                             .validator = sharg::input_file_validator{{"tsv"}}});
//![input_file_validator]
}
}
// clang-format on
