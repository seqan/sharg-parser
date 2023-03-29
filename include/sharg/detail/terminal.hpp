// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

/*!\file
 * \author Svenja Mehringer <svenja.mehringer AT fu-berlin.de>
 * \brief Checks if program is run interactively and retrieves dimensions of
 *        terminal (Transferred from seqan2).
 */

#pragma once

#ifndef _WIN32
#    include <sys/ioctl.h>
#else
#    include <windows.h>
#endif

#include <cstdio>
#include <unistd.h>

#include <sharg/platform.hpp>

namespace sharg::detail
{

// ----------------------------------------------------------------------------
// Function input_is_terminal()
// ----------------------------------------------------------------------------

/*!\brief Check whether the input is a terminal.
 * \ingroup parser
 * \return True if code is run in a terminal, false otherwise.
 * \details
 * For example "./some_binary --help | less" will return false. "./some_binary --help" will return true.
 */
inline bool input_is_terminal()
{
#ifndef _WIN32
    return isatty(STDIN_FILENO);
#else
    return false;
#endif
}

/*!\brief Check whether the output is interactive.
 * \ingroup parser
 * \return True if code is run in a terminal, false otherwise.
 * \details
 * For example "./some_binary --help | less" will return false. "./some_binary --help" will return true.
 */
inline bool output_is_terminal()
{
#ifndef _WIN32
    return isatty(STDOUT_FILENO);
#else
    return false;
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
    return 80; // not implemented in windows
#endif
}

} // namespace sharg::detail
