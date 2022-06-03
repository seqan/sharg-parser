#include <sharg/all.hpp>

int main(int argc, const char ** argv)
{
    sharg::parser myparser{"Test", argc, argv};
    std::string myvar{"Example"};
    myparser.add_option(myvar, 's', "special-op", "You know what you doin'?",
                        sharg::option_spec::advanced);
}
