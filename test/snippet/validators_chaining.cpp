#include <sharg/all.hpp>

int main(int argc, char const ** argv)
{
    sharg::parser myparser{"Test", argc, argv}; // initialize

    //![validator_call]
    std::string file_name;

    sharg::regex_validator absolute_path_validator{"(/[^/]+)+/.*\\.[^/\\.]+$"};
    sharg::input_file_validator my_file_ext_validator{{"sa", "so"}};

    myparser.add_option(file_name,
                        sharg::config{.short_id = 'f',
                                      .long_id = "file",
                                      .description = "Give me a file name with an absolute path.",
                                      .validator = absolute_path_validator | my_file_ext_validator});
    //![validator_call]

    // an exception will be thrown if the user specifies a file name
    // that is not an absolute path or does not have one of the file extension [sa,so]
    try
    {
        myparser.parse();
    }
    catch (sharg::parser_error const & ext) // the user did something wrong
    {
        std::cerr << "[PARSER ERROR] " << ext.what() << "\n"; // customize your error message
        return -1;
    }

    std::cout << "filename given by user passed validation: " << file_name << "\n";
    return 0;
}
