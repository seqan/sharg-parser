#include <sharg/all.hpp>

int main(int argc, const char ** argv)
{
    sharg::parser myparser{"Test", argc, argv}; // initialize

    //! [validator_call]
    std::filesystem::path myfile{};

    myparser.add_option(myfile,'f',"file","The input file containing the sequences.",
                        sharg::option_spec::standard, sharg::input_file_validator{{"fa","fasta"}});
    //! [validator_call]

    // an exception will be thrown if the user specifies a filename
    // that does not have one of the extensions ["fa","fasta"] or if the file does not exist/is not readable.
    try
    {
        myparser.parse();
    }
    catch (sharg::parser_error const & ext) // the user did something wrong
    {
        std::cerr << "[PARSER ERROR] " << ext.what() << "\n"; // customize your error message
        return -1;
    }

    std::cerr << "filename given by user passed validation: " << myfile << "\n";
    return 0;
}
