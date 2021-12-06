#include <seqan3/io/sequence_file/input.hpp>

#include <sharg/validators.hpp>

int main()
{
    // Default constructed validator has an empty extension list.
    sharg::input_file_validator validator1{};
    std::cerr << validator1.get_help_page_message() << '\n';

    // Specify your own extensions for the input file.
    sharg::input_file_validator validator2{std::vector{std::string{"exe"}, std::string{"fasta"}}};
    std::cerr << validator2.get_help_page_message() << '\n';

    // Give the seqan3 file type as a template argument to get all valid extensions for this file.
    sharg::input_file_validator<seqan3::sequence_file_input<>> validator3{};
    std::cerr << validator3.get_help_page_message() << '\n';

    return 0;
}
