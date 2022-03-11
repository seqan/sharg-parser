#include <iostream>

#include <sharg/validators.hpp>

int main()
{
    // Default constructed validator has an empty extension list.
    sharg::output_file_validator validator1{sharg::output_file_open_options::create_new};
    std::cerr << validator1.get_help_page_message() << '\n';

    // Specify your own extensions for the output file.
    sharg::output_file_validator validator2{sharg::output_file_open_options::create_new,
                                            std::vector{std::string{"exe"}, std::string{"fasta"}}};
    std::cerr << validator2.get_help_page_message() << '\n';

    return 0;
}
