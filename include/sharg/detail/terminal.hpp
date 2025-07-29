// SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Checks if program is run interactively and retrieves dimensions of
 *        terminal (Transferred from seqan2).
 */

#pragma once

#ifndef _WIN32
#    include <unistd.h>

#    include <sys/ioctl.h>
#else
#    include <io.h>
#    include <stdio.h>
#endif

#include <sharg/platform.hpp>

namespace sharg::detail
{

/*!\brief Check whether the standard input is interactive.
 * \ingroup parser
 * \return True if standard input is a terminal, false otherwise.
 * \details
 * For example "./some_binary --help | less" will return false. "./some_binary --help" will return true.
 */
inline bool stdin_is_terminal()
{
#ifndef _WIN32
    return isatty(STDIN_FILENO);
#else
    return _isatty(_fileno(stdin));
#endif
}

/*!\brief Check whether the standard output is interactive.
 * \ingroup parser
 * \return True if standard output is a terminal, false otherwise.
 * \details
 * For example "./some_binary --help | less" will return false. "./some_binary --help" will return true.
 */
inline bool stdout_is_terminal()
{
#ifndef _WIN32
    return isatty(STDOUT_FILENO);
#else
    return _isatty(_fileno(stdout));
#endif
}

/*!\brief Check whether the standard error output is interactive.
 * \ingroup parser
 * \return True if standard error output is a terminal, false otherwise.
 * \details
 * For example "./some_binary --help 2> cerr.out" will return false. "./some_binary --help" will return true.
 */
inline bool stderr_is_terminal()
{
#ifndef _WIN32
    return isatty(STDERR_FILENO);
#else
    return _isatty(_fileno(stderr));
#endif
}

// ----------------------------------------------------------------------------
// Function get_terminal_size()
// ----------------------------------------------------------------------------

/*!\brief  Retrieve size of terminal.
 * \ingroup parser
 * \return The width of the current terminal in number of characters.
 *
 * \details
 *
 * Note: Only works on Linux/Unix.
 * TIOCGWINSZ is the command (number) to trigger filling the winsize struct.
 * STDOUT_FILENO is the default file descriptor (STDOUT_FILENO == fileno(stdout)).
 */
inline unsigned get_terminal_width()
{
#ifndef _WIN32

    struct winsize w;
    w.ws_row = 0;
    w.ws_col = 0;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    return w.ws_col;
#else
    return 80; // Not implemented for Windows yet. For inspiration, see https://stackoverflow.com/a/12642749.
#endif
}

} // namespace sharg::detail
