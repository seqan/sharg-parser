# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------------

find_path (SEQAN3_TEST_LICENSE_DIR NAMES LICENSE.md HINTS "${SEQAN3_CLONE_DIR}")

add_definitions(-DSEQAN3_TEST_LICENSE_DIR="${SEQAN3_TEST_LICENSE_DIR}")

sharg_test(format_help_test.cpp CYCLIC_DEPENDING_INCLUDES
            include-seqan3-argument_parser-detail-format_html.hpp
            include-seqan3-argument_parser-detail-format_man.hpp)
sharg_test(format_html_test.cpp CYCLIC_DEPENDING_INCLUDES
            include-seqan3-argument_parser-detail-format_help.hpp
            include-seqan3-argument_parser-detail-format_man.hpp)
sharg_test(format_man_test.cpp)
sharg_test(safe_filesystem_entry_test.cpp)
sharg_test(type_name_as_string_test.cpp)
sharg_test(version_check_debug_test.cpp)
sharg_test(version_check_release_test.cpp)
