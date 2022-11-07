#define main test

#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    int val{};

    sharg::parser parser{"Eat-Me-App", argc, argv};
    parser.add_subsection("Eating Numbers");
    parser.add_option(val, sharg::config{.short_id = 'i', .long_id = "int", .description = "Desc."});
    parser.parse();

    return 0;
}

#undef main

int main()
{
    std::string argv1{"./Eat-Me-App"}, argv2{"-h"};
    std::array argv{argv1.data(), argv2.data()};

    return test(argv.size(), argv.data());
}
