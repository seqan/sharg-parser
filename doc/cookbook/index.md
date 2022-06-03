# The Sharg Cookbook {#cookbook}

[TOC]

This document provides example recipes on how to carry out particular tasks using the Sharg functionalities in C++.
Please note that these **recipes are not ordered**. You can use the links in the table of contents or the search
function of your browser to navigate them.

It will take some time, but we hope to expand this document into containing numerous great examples.
If you have suggestions for how to improve the Cookbook and/or examples you would like included,
please feel free to contact us.

# Constructing a basic parser

\include doc/cookbook/basic_arg_parse.cpp

# Constructing a subcommand parser

\include doc/howto/subcommand_parser/subcommand_parse.cpp

# Write a custom validator {#cookbook_custom_validator}
This recipe implements a validator that checks whether a numeric argument is an integral square (i.e. 0, 1, 4, 9...).
Invalid values throw a sharg::validation_error.

\snippet doc/tutorial/concepts/custom_validator_solution.cpp validator
