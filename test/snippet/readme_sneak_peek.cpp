#define main test

#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    int val{};

    sharg::parser parser{"Eat-Me-App", argc, argv};
    parser.add_subsection("Eating Numbers");
    parser.add_option(val, sharg::config{.short_id = 'i', .long_id = "int", .description = "Desc."});
    parser.parse();
}

#undef main

int main(int argc, char ** argv)
{
    char * test_argv[] = {"./Eat-Me-App", "-h"};
    int const test_argc = sizeof(test_argv) / sizeof(*test_argv);
    return test(test_argc, test_argv);
}
