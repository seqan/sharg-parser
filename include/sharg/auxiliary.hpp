// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Provides auxiliary information.
 */

#pragma once

#include <string>
#include <vector>

#include <sharg/platform.hpp>

namespace sharg
{

/*!\brief Indicates whether application allows automatic update notifications by the sharg::parser.
 * \ingroup misc
 * \details
 * \stableapi{Since version 1.0.}
 */
enum class update_notifications : uint8_t
{
    on, //!< Automatic update notifications should be enabled.
    off //!< Automatic update notifications should be disabled.
};

/*!\brief Stores all parser related meta information of the sharg::parser.
 * \ingroup parser
 * \details
 * \attention You should supply as much information as possible to help the users
 * of your application.
 *
 * The meta information is assembled in a struct to provide a central access
 * point that can be easily extended.
 *
 * \remark For a complete overview, take a look at \ref parser
 *
 * \stableapi{Since version 1.0.}
 */
struct parser_meta_data // holds all meta information
{
    /*!\brief The application name that will be displayed on the help page.
     *
     * The application name must only contain alpha-numeric characters, '_' or '-',
     * i.e. the following regex must evaluate to true: `\"^[a-zA-Z0-9_-]+$\"`.
     */
    std::string app_name;

    //!\brief The version information `MAJOR.MINOR.PATH` (e.g. 3.1.3)
    std::string version;

    //!\brief A short description of the application (e.g. "A tool for mapping reads to the genome").
    std::string short_description;

    //!\brief Your name ;-)
    std::string author;

    //!\brief The author's e-mail address for correspondence.
    std::string email;

    /*!\brief The date that the application was last updated. Keep this updated,
     *!          since it will tell your users that the application is maintained.
     */
    std::string date;

    //!\brief A link to  your github/gitlab project with the newest release.
    std::string url;

    //!\brief Brief copyright (and/or license) information.
    std::string short_copyright;

    /*!\brief Detailed copyright information that will be displayed
     *        when the user specifies "--copyright" on the command line.
     */
    std::string long_copyright;

    //!\brief How  users shall cite your application.
    std::string citation;

    /*!\brief The title of your man page when exported by specifying
     *        "--export-help man" on the common line.
     */
    std::string man_page_title;

    //!\brief The man page section info (type `man man` on the command line for more information).
    unsigned man_page_section{1};

    /*!\brief A more detailed description that is displayed on the help
     *        page in the section "DESCRIPTION". Each `std::string` appended
     *        to the description vector will be treated as a paragraph and
     *        is separated by a new line.
     */
    std::vector<std::string> description;

    /*!\brief Add lines of usage to the synopsis section of the help page (e.g.
     *        "./my_read_mapper [OPTIONS] FILE1 FILE1").
     */
    std::vector<std::string> synopsis;

    /*!\brief Provide some examples on how to use your tool and what standard
     *        parameters might be appropriate in different cases (e.g.
     *        "./my_read_mapper -s 3 --my_flag path/infile1").
     */
    std::vector<std::string> examples;
};

} // namespace sharg
