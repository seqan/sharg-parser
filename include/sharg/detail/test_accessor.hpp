// --------------------------------------------------------------------------------------------------------
// Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
// Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
// This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
// shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
// --------------------------------------------------------------------------------------------------------

/*!\file
 * \brief Forward declares sharg::detail::test_accessor.
 * \author Lydia Buntrock <lydia.buntrock AT fu-berlin.de>
 */

#pragma once

#include <sharg/platform.hpp>

namespace sharg::detail
{

/*!\brief Attorney-Client pattern for accessing private / protected class members in test cases.
 * \attention You can currently only have one definition of test_accessor in one translation unit.
 * \see https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Friendship_and_the_Attorney-Client
 */
struct test_accessor;

} // namespace sharg::detail
