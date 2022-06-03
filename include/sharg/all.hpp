// -----------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
// -----------------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Meta-header for the \link parser Parser module \endlink.
 */

/*!\file
 * \brief The Parser Module
 * \defgroup parser Parser
 *
 * # The Parser Class
 *
 * \copydetails sharg::parser
 *
 * # Parsing Command Line Arguments
 *
 * \copydetails sharg::parser::parse
 *
 * # Argument Validation
 *
 * The Sharg Parser offers a validation mechanism for (positional_)options
 * via callables. You can pass any functor that fulfils the sharg::validator
 * and takes the value passed to the add_(positional_)option function call as
 * a parameter. We provide some commonly used functor that might come in handy:
 *
 * - sharg::regex_validator
 * - sharg::value_list_validator
 * - sharg::arithmetic_range_validator
 * - sharg::input_file_validator
 * - sharg::output_file_validator
 * - sharg::input_directory_validator
 * - sharg::output_directory_validator
 */

#pragma once

#include <sharg/auxiliary.hpp>
#include <sharg/exceptions.hpp>
#include <sharg/parser.hpp>
#include <sharg/validators.hpp>
