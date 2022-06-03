// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

#include <gtest/gtest.h>

#include <type_traits>

#include <sharg/detail/type_name_as_string.hpp>

// Some test namespace to check if namespace information are preserved within the naming.
namespace foo
{
template <typename... type>
struct bar
{};
} // namespace foo

// Some types to test if type inspection works as expected.
// Note that the returned name might differ between compiler vendors and thus must be adapted accordingly
// in case this tests fails for those vendors.
using reflection_types = ::testing::Types<char,
                                          char16_t const,
                                          char32_t &,
                                          short *,
                                          double const * const,
                                          foo::bar<char> const &,
                                          foo::bar<foo::bar<char, double>>>;

template <typename param_type>
class type_inspection : public ::testing::Test
{

public:
    // Returns the name of the type according to the list of names defined above.
    std::string const expected_name() const
    {
        if constexpr (std::is_same_v<param_type, char>)
            return "char";
        else if constexpr (std::is_same_v<param_type, char16_t const>)
            return "char16_t const";
        else if constexpr (std::is_same_v<param_type, char32_t &>)
            return "char32_t &";
        else if constexpr (std::is_same_v<param_type, short *>)
            return "short*";
        else if constexpr (std::is_same_v<param_type, double const * const>)
            return "double const* const";
        else if constexpr (std::is_same_v<param_type, foo::bar<char> const &>)
            return "foo::bar<char> const &";
        else if constexpr (std::is_same_v<param_type, foo::bar<foo::bar<char, double>>>)
            return "foo::bar<foo::bar<char, double> >";
        else
            throw std::runtime_error{"Encountered unknown type in test."};
    }
};

// Register test.
TYPED_TEST_SUITE(type_inspection, reflection_types, );

TYPED_TEST(type_inspection, type_name_as_string)
{
    EXPECT_EQ(sharg::detail::type_name_as_string<TypeParam>, this->expected_name());
}
