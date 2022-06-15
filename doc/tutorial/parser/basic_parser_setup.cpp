#include <sharg/all.hpp> // includes all necessary headers

int main(int argc, char ** argv)
{
    sharg::parser myparser{"Game-of-Parsing", argc, argv}; // initialise myparser

    // ... add information, options, flags and positional options

    try
    {
        myparser.parse(); // trigger command line parsing
    }
    catch (sharg::parser_error const & ext) // catch user errors
    {
        std::cerr << "[Winter has come] " << ext.what() << "\n"; // customise your error message
        return -1;
    }
}
