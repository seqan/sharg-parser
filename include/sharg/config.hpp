// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

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
 * | sharg::config::short_id             |           ✓          |      ✓      |              X            |
 * | sharg::config::long_id              |           ✓          |      ✓      |              X            |
 * | sharg::config::description          |           ✓          |      ✓      |              ✓            |
 * | sharg::config::default_message      |           ✓          |      X      |              X            |
 * | sharg::config::advanced             |           ✓          |      ✓      |              X            |
 * | sharg::config::hidden               |           ✓          |      ✓      |              X            |
 * | sharg::config::required             |           ✓          |      ✓      |             (✓)           |
 * | sharg::config::validator            |           ✓          |     (✓)     |              ✓            |
 *
 * \details
 * \stableapi{Since version 1.0.}
 */
template <typename validator_t = detail::default_validator>
struct config
{
    static_assert(sharg::validator<validator_t>, "The validator passed to sharg::config must model sharg::validator");

    /*!\brief The short identifier for the option (e.g. 'a', making the option callable via `-a`).
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     *
     * \stableapi{Since version 1.0.}
     */
    char short_id{'\0'};

    /*!\brief The long identifier for the option (e.g. "age", making the option callable via `--age`).
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     *
     * \stableapi{Since version 1.0.}
     */
    std::string long_id{};

    /*!\brief The description to be shown on any (exported) help page.
     * \details
     * \stableapi{Since version 1.0.}
     */
    std::string description{};

    /*!\brief The default message to be shown on any (exported) help page.
     *
     * "Default: " will be prepended. "." will be appended.
     *
     * ### Example
     *
     * `parser.add_option(j, sharg::config{.short_id = 'j'. default_message = "Same as -i"})`
     * will result in the help page looking like this:
     * ```
     * -j (signed 32 bit integer)
     *     Default: Same as -i.
     * ```
     *
     * \attention Not allowed for required options, flags, and positional options.
     *
     * \stableapi{Since version 1.0.}
     */
    std::string default_message{};

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
     *
     * \stableapi{Since version 1.0.}
     */
    bool advanced{false};

    /*!\brief Whether the option should be hidden.
     *
     * If set to true, the option will not be displayed on any help page;
     * Neither when calling `./executable --help`, nor when exported with `--export-help`.
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     *
     * \stableapi{Since version 1.0.}
     */
    bool hidden{false};

    /*!\brief Whether the option is required.
     *
     * If set to true and the user does not provide the respective option on the command line,
     * the argument_parser will automatically detect this and throw a sharg::invalid_argument exception.
     *
     * \attention This parameter cannot be set for positional options added with
     *            sharg::parser::add_positional_option and will trigger a sharg::design_error.
     *
     * \stableapi{Since version 1.0.}
     */
    bool required{false};

    /*!\brief A sharg::validator that verifies the value after parsing (callable).
     * \details
     * \stableapi{Since version 1.0.}
     */
    validator_t validator{};
};

} // namespace sharg
