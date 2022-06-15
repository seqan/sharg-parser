#include <sharg/all.hpp>

// my custom type
namespace foo
{

class bar
{
public:
    int a;

    // Make foo::bar satisfy sharg::ostreamable
    friend std::ostream & operator<<(std::ostream & output, bar const & my_bar)
    {
        output << my_bar.a; // Adapt this for your type
        return output;
    }

    // Make foo::bar satisfy sharg::istreamable
    friend std::istream & operator>>(std::istream & input, bar & my_bar)
    {
        input >> my_bar.a; // Adapt this for your type
        return input;
    }
};

} // namespace foo

int main(int argc, char const ** argv)
{
    sharg::parser parser{"my_foobar_parser", argc, argv, sharg::update_notifications::off};

    foo::bar my_bar{};
    parser.add_option(my_bar, 'f', "foo-bar", "Please supply an integer for member foo::bar::a");

    try
    {
        parser.parse(); // trigger command line parsing
    }
    catch (sharg::parser_error const & ext) // catch user errors
    {
        std::cerr << "[Error] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    std::cout << "my_bar was initialised with a = " << my_bar.a << std::endl;

    return 0;
}
