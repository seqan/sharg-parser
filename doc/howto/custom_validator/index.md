# How to make your own validator {#custom_validator}

[TOC]

This HowTo guides you through implementing your own `sharg::validator`.

\tutorial_head{Easy, 10 min, , }

# Motivation

To use a custom validator `sharg::validator`, the type must satisfy certain requirements (listed below).
This tutorial shows you what requirements must be met and supplies you with a copy and paste source
for your code.

# sharg::validator

Remember the tutorial on \ref tutorial_argument_parser ? Let's implement our own validator that checks
if a numeric argument is an integral square (i.e. the user shall only be allowed to enter 0, 1, 4, 9...).

## Understanding the requirements

In order to model the sharg::validator, your custom validator must provide the following:

  1. It needs to expose a `value_type` type member which identifies the type of variable the validator works on.
     Currently, the SeqAn validators either have value_type `double` or `std::string`.
     Since the validator works on every type that has a common reference type to `value_type`, it enables a validator
     with `value_type = double` to work on all arithmetic values.
     \attention In order to be chainable, the validators need to share the same value_type!
  2. It has to be a [functor](https://stackoverflow.com/questions/356950/what-are-c-functors-and-their-uses), which
     basically means it must provide `operator()`.
  3. It has to have a member function `std::string get_help_page_message() const` that returns a string that can be
     displayed on the help page.

## Formally satisfying the requirements

You can check if your type models sharg::validator in the following way:

```cpp
struct custom_validator
{
    // ...
};

static_assert(sharg::validator<custom_validator>);
```

To formally satisfy the requirements, your functions don't need the correct behaviour, yet.
Only the signatures need to be fully specified.

\assignment{Assignment 3: Custom validator I}
Implement enough of the above mentioned `struct custom_validator` for it to model sharg::validator and pass
the check. You can use an empty `main()`-function for now.
\endassignment
\solution
\include doc/howto/custom_validator/custom_validator_solution1.cpp
\endsolution

## Implementing the functionality

The above implementation is of course not yet useful.
It should be usable with this main function:

\snippet doc/tutorial/concepts/custom_validator_solution.cpp main

Try to think of the correct behaviour of this program.

It should print "Yeah!" for the arguments `-i 0`, `-i 4`, or `-i 144`; and/or `-j 0` or `-j 4`.

It should fail for the arguments `-i 3`; and/or `-j 144` or `-j 3`.

\assignment{Assignment 4: Custom validator II}
Implement your validator fully, i.e. make it throw seqan3::validation_error if the number provided is not a
square.
Also, give a nice description for the help page.

\endassignment
\solution
\snippet doc/tutorial/concepts/custom_validator_solution.cpp validator
\endsolution

You have now written your own type that is compatible with our constrained interfaces!
