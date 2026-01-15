# How to make your custom type model sharg::parsable {#fulfil_parsable}

<!--
SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC-BY-4.0
-->

[TOC]

This HowTo guides you through satisfying the requirements of sharg::parsable.

\tutorial_head{Easy, 10 min, , }

# Motivation

To use a custom type with `sharg::parser::add_option` or
`sharg::parser::add_positional_option`, the type must satisfy `sharg::parsable`.
This tutorial shows you what requirements must be met and supplies you with a copy and paste source
for your code.

# Concept sharg::parsable

As you can see in the API documentation of `sharg::parsable`, the type must model either both
`sharg::istreamable` and `sharg::ostreamable` or `sharg::named_enumeration`.

**If your type is an enum, refer to `sharg::enumeration_names` on how to make it compatible with the
`sharg::parser`.**

In all other cases, your type needs to model `sharg::istreamable` and `sharg::ostreamable`.
As you can see in the respective documentation, the concept is simple. You merely need to
supply the stream operators `operator>>()` and `operator<<()` for your type.

# Make your own type compatible

\note You must be able to modify the class itself for this solution to work.

The following example makes the class `bar` in namespace `foo` compatible with the `sharg::parser`:

\include doc/howto/custom_types/custom_type.cpp

# Make an external type compatible

If you cannot modify the class, you can do the following:

\include doc/howto/custom_types/external_custom_type.cpp
