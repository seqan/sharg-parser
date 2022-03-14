#include <sharg/all.hpp>

// my custom type
namespace foo
{

class bar
{
public:
    int a;

    // Make foo::bar model sharg::ostreamable
    friend std::ostream &operator<<( std::ostream & output, const bar & foob)
    {
        output << foob.a; // this needs to be adapted to suit your type
        return output;
    }

    // Make foo::bar model sharg::istreamable
    friend std::istream &operator>>( std::istream & input, bar & foob)
    {
        input >> foob.a;  // this needs to be adapted to suit your type
        return input;
    }
};

} // namespace foo

int main(int argc, char const ** argv)
{
    sharg::argument_parser parser{"my_foobar_parser", argc, argv, sharg::update_notifications::off};

    foo::bar foob{};
    parser.add_option(foob, 'f', "foo-bar", "Please supply an integer for member foo::bar::a");

    try
    {
        parser.parse(); // trigger command line parsing
    }
    catch (sharg::argument_parser_error const & ext) // catch user errors
    {
        std::cerr << "[Error] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    std::cout << "foob was initialised with a = " << foob.a << std::endl;

    return 0;
}
