#include <sharg/all.hpp> // includes all necessary headers

void initialise_parser(sharg::parser & parser)
{
    parser.info.author = "Cersei";
    parser.info.short_description = "Aggregate average Game of Thrones viewers by season.";
    parser.info.version = "1.0.0";
}

int main(int argc, char ** argv)
{
    sharg::parser myparser{"Game-of-Parsing", argc, argv};
    initialise_parser(myparser);
    // code from assignment 1
}
