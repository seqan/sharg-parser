# How to make your custom type model harg::argument_parser_compatible_option {#fulfil_argument_parser_compatible_option}

[TOC]

This HowTo shows how make your custom type fulfil sharg::argument_parser_compatible_option.

\tutorial_head{Easy, 10 min, , }

# Motivation

In order to use a custom type in an `sharg::argument_parser::add_option` or
`sharg::argument_parser::add_positional_option` call, the type must model `sharg::argument_parser_compatible_option`.
This tutorial will quickly show you what requirements must be met and will supply you with a copy and paste source
for your code.

# Concept sharg::argument_parser_compatible_option

As you can see in the API documentation of `sharg::argument_parser_compatible_option`, the type must model either
`sharg::istreamable` and `sharg::ostreamable` or `sharg::named_enumeration`.

**If your type is an enum, take a look at `sharg::enumeration_names` to make it compatabile with the
`sharg::argument_parser`.**

Otherwise, your type needs to model `sharg::istreamable` and `sharg::ostreamable`.
If you take a look at the concept in the documentation, you can see that the concept is simple. You merely need to
overload the stream operators `operator>>()` and `operator<<()` for your type and `std::istream`/`std::ostream`
respectively.

# Make your own type compatible

\note You must be able to modify the class itself for this solution to work.

The following example makes the class `bar` in namespace `foo` compatible with the `sharg::argument_parser`:

\include doc/howto/custom_types/custom_type.cpp

# Make an external type compatible

If you cannot modify the class you'd like to be compatible with the `sharg::parser`, you can do the following:

\include doc/howto/custom_types/external_custom_type.cpp
