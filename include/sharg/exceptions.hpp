// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides parser related exceptions.
 */

#pragma once

#include <stdexcept>

#include <sharg/platform.hpp>

namespace sharg
{
/*!\brief Parser exception that is thrown whenever there is an error
 * while parsing the command line arguments.
 * \ingroup exceptions
 *
 * \details
 *
 * Errors caught by the parser:
 *
 * - Unknown option/flag (not specified by developer but set by user)
 * - Too many positional options
 * - Too few positional options
 * - Option that was declared as required (option_spec::required) was not set
 * - Option is not a list but specified multiple times
 * - Type conversion failed
 * - Validation failed (as defined by the developer)
 *
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class parser_error : public std::runtime_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    parser_error(std::string const & s) : std::runtime_error(s)
    {}
};

/*!\brief Parser exception thrown when encountering unknown option.
 * \ingroup exceptions
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class unknown_option : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    unknown_option(std::string const & s) : parser_error(s)
    {}
};

/*!\brief Parser exception thrown when too many arguments are provided.
 * \ingroup exceptions
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class too_many_arguments : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    too_many_arguments(std::string const & s) : parser_error(s)
    {}
};

/*!\brief Parser exception thrown when too few arguments are provided.
 * \ingroup exceptions
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class too_few_arguments : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    too_few_arguments(std::string const & s) : parser_error(s)
    {}
};

/*!\brief Parser exception thrown when a required option is missing.
 * \ingroup exceptions
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class required_option_missing : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    required_option_missing(std::string const & s) : parser_error(s)
    {}
};

/*!\brief Parser exception thrown when a non-list option is declared multiple times.
 * \ingroup exceptions
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class option_declared_multiple_times : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    option_declared_multiple_times(std::string const & s) : parser_error(s)
    {}
};

/*!\brief Parser exception thrown when an incorrect argument is given as (positional) option value.
 * \ingroup exceptions
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class user_input_error : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    user_input_error(std::string const & s) : parser_error(s)
    {}
};

/*!\brief Parser exception thrown when an argument could not be casted to the according type.
 * \ingroup exceptions
 * \remark For a complete overview, take a look at \ref parser
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class validation_error : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    validation_error(std::string const & s) : parser_error(s)
    {}
};

/*!\brief Parser exception that is thrown whenever there is an design
 * error directed at the developer of the application (e.g. Reuse of option).
 *
 * \details
 *
 * Errors caught by the parser:
 *
 * - Reuse of a short or long identifier (must be unique)
 * - Both identifiers must not be empty (one is ok)
 * - Flag default value must be false
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
class design_error : public parser_error
{
public:
    /*!\brief The constructor.
     * \param[in] s The error message.
     *
     * \details
     * \stableapi{Since version 1.0.}
     */
    design_error(std::string const & s) : parser_error(s)
    {}
};

} // namespace sharg
