#include <sharg/all.hpp>

int main(int argc, char const ** argv)
{
    sharg::parser myparser{"Test", argc, argv};
    std::string myvar{"Example"};
    myparser.add_option(myvar, sharg::config{.short_id = 's', .long_id = "special-op", .advanced = true});
}
