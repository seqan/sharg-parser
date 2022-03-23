#include <sharg/all.hpp>

// external type, i.e., you cannot change the implementation
namespace external
{

class bar
{
public:
    int a;
};

} // namespace external

namespace std
{

// Make external::bar satisfy sharg::ostreamable
ostream & operator<<(ostream & output, const external::bar & ext_bar)
{
    output << ext_bar.a; // Adapt this for your type
    return output;
}

// Make external::bar satisfy sharg::istreamable
istream & operator>>(istream & input, external::bar & ext_bar)
{
    input >> ext_bar.a;  // Adapt this for your type
    return input;
}

} // namespace std

int main(int argc, char const ** argv)
{
    sharg::argument_parser parser{"my_ext_bar_parser", argc, argv, sharg::update_notifications::off};

    external::bar ext_bar{};
    parser.add_option(ext_bar, 'f', "external-bar", "Please supply an integer for member external::bar::a");

    try
    {
        parser.parse(); // trigger command line parsing
    }
    catch (sharg::argument_parser_error const & ext) // catch user errors
    {
        std::cerr << "[Error] " << ext.what() << "\n"; // customise your error message
        return -1;
    }

    std::cout << "ext_bar was initialised with a = " << ext_bar.a << std::endl;

    return 0;
}
