#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    sharg::argument_parser myparser{"Game-of-Parsing", argc, argv, sharg::update_notifications::off};
    // disable update notifications permanently ----------------------------^
}
