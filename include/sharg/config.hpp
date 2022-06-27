// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides sharg::config class.
 */

#pragma once

#include <sharg/validators.hpp>

namespace sharg
{

/*!\brief Option struct that is passed to the `sharg::parser::add_option()` function.
 * \ingroup parser
 *
 * The following table gives an overview about which parameters are valid for which type of option added to the parser.
 * Please see the individual entities for detailed documentation.
 *
 * * `✓` - Parameter can be set for this type of option
 * * `(✓)` - Parameter can be set but is not indented for this type of option
 * * `X` - Setting this parameter for this type of option raises a sharg::design_error
 *
 * | Parameter                           | option ( `-a/--all`) | flag (`-f`) | positional_option (`foo`) |
 * |-------------------------------------|----------------------|-------------|---------------------------|
 * | sharg::config::description          |           ✓          |      ✓      |              ✓            |
 * | sharg::config::short_id             |           ✓          |      ✓      |              X            |
 * | sharg::config::long_id              |           ✓          |      ✓      |              X            |
 * | sharg::config::validator            |           ✓          |     (✓)     |              ✓            |
 * | sharg::config::advanced             |           ✓          |      ✓      |              X            |
 * | sharg::config::hidden               |           ✓          |      ✓      |              X            |
 * | sharg::config::required             |           ✓          |      ✓      |             (✓)           |
 *
 */
template <typename validator_t = detail::default_validator>
struct config
{
    static_assert(sharg::validator<validator_t>, "The validator passed to sharg:;config must model sharg::validator");

    /*!\brief The short identifier for the option (e.g. 'a', making the option callable via `-a`).
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     */
    char short_id{'\0'};

    /*!\brief The long identifier for the option (e.g. "age", making the option callable via `--age`).
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     */
    std::string long_id{};

    //!\brief The description to be shown on any (exported) help page.
    std::string description{};

    /*!\brief Whether the option should only be displayed on the advanced help page.
     *
     * This is only relevant for the help page printed on the command line.
     * If set to true, the option will only be displayed when the user requests the advanced help via
     * `./executable --advanced-help` or `./executable -hh`.
     *
     * This can help to not overwhelm the user with parameters but still provide an extensive interface of your
     * application.
     *
     * When information are exported with `--export-help`, advanced options are always shown.
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     */
    bool advanced{false};

    /*!\brief Whether the option should be hidden.
     *
     * If set to true, the option will not be displayed on any help page;
     * Neither when calling `./executable --help`, nor when exported with `--export-help`.
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     */
    bool hidden{false};

    /*!\brief Whether the option is required.
     *
     * If set to true and the user does not provide the respective option on the command line,
     * the argument_parser will automatically detect this and throw a sharg::invalid_argument exception.
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     */
    bool required{false};

    //!\brief A sharg::validator that verifies the value after parsing (callable).
    validator_t validator{};
};

} // namespace sharg
