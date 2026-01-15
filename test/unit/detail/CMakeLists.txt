# SPDX-FileCopyrightText: 2006-2026, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2026, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

find_path (SHARG_TEST_LICENSE_DIR
           NAMES LICENSE.md
           HINTS "${SHARG_CLONE_DIR}")

add_definitions (-DSHARG_TEST_LICENSE_DIR="${SHARG_TEST_LICENSE_DIR}")

sharg_test (format_help_test.cpp CYCLIC_DEPENDING_INCLUDES include-sharg-detail-format_html.hpp
                                                           include-sharg-detail-format_man.hpp)
sharg_test (format_html_test.cpp CYCLIC_DEPENDING_INCLUDES include-sharg-detail-format_help.hpp
                                                           include-sharg-detail-format_man.hpp)
sharg_test (format_man_test.cpp)
sharg_test (format_ctd_test.cpp)
sharg_test (format_cwl_test.cpp)
sharg_test (safe_filesystem_entry_test.cpp)
sharg_test (type_name_as_string_test.cpp)
sharg_test (version_check_debug_test.cpp)
sharg_test (version_check_release_test.cpp)

file (DOWNLOAD https://raw.githubusercontent.com/seqan/seqan3/main/include/seqan3/version.hpp
      ${CMAKE_BINARY_DIR}/include/seqan3/version.hpp)
sharg_test (seqan3_test.cpp)
target_include_directories (seqan3_test SYSTEM PUBLIC ${CMAKE_BINARY_DIR}/include)
