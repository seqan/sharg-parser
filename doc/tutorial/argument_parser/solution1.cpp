#include <sharg/all.hpp> // includes all necessary headers
#include <seqan3/core/debug_stream.hpp>   // our custom output stream

void initialise_argument_parser(sharg::argument_parser & parser)
{
    parser.info.author = "Cersei";
    parser.info.short_description = "Aggregate average Game of Thrones viewers by season.";
    parser.info.version = "1.0.0";
}

int main(int argc, char ** argv)
{
    sharg::argument_parser myparser{"Game-of-Parsing", argc, argv};
    initialise_argument_parser(myparser);
    // code from assignment 1
}
