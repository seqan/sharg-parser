# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10)
project (sharg_test_unit CXX)

include (../sharg-test.cmake)
include (GoogleTest OPTIONAL)

include (diagnostics/list_missing_unit_tests)
include (diagnostics/list_unused_unit_tests)
include (include_dependencies/add_include_dependencies)

option (SHARG_VERBOSE_TESTS "Run each test case individually" ON)
option (SHARG_USE_INCLUDE_DEPENDENCIES "Build tests in an hierarchical order (by an include graph, i.e. tests with less dependencies are build first)" OFF)

macro (sharg_test unit_test_cpp)
    cmake_parse_arguments(SHARG_TEST "" "" "CYCLIC_DEPENDING_INCLUDES" ${ARGN})

    file (RELATIVE_PATH unit_test "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_DIR}/${unit_test_cpp}")
    sharg_test_component (target "${unit_test}" TARGET_NAME)
    sharg_test_component (test_name "${unit_test}" TEST_NAME)

    add_executable (${target} ${unit_test_cpp})
    target_link_libraries (${target} sharg::test::unit)
    add_include_dependencies (${target} "${SHARG_TEST_CYCLIC_DEPENDING_INCLUDES}")
    collect_used_test (${target})
    if (SHARG_VERBOSE_TESTS)
        gtest_discover_tests(${target} TEST_PREFIX "${test_name}::" PROPERTIES TIMEOUT "30")
    else ()
        add_test (NAME "${test_name}" COMMAND ${target})
    endif ()

    unset (unit_test)
    unset (target)
    unset (test_name)
endmacro ()

sharg_require_ccache ()
sharg_require_test ()

add_subdirectories ()

list_missing_unit_tests()
list_unused_unit_tests ()
