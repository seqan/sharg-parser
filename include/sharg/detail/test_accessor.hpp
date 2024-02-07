// SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
// SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
// SPDX-License-Identifier: BSD-3-Clause

/*!\file
 * \brief Forward declares sharg::detail::test_accessor.
 * \author Lydia Buntrock <lydia.buntrock AT fu-berlin.de>
 */

#pragma once

#include <sharg/platform.hpp>

namespace sharg::detail
{

/*!\brief Attorney-Client pattern for accessing private / protected class members in test cases.
 * \ingroup misc
 * \attention You can currently only have one definition of test_accessor in one translation unit.
 * \see https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Friendship_and_the_Attorney-Client
 */
struct test_accessor;

} // namespace sharg::detail
