# -----------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/seqan3/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10)
project (sarg_test_unit CXX)

find_package (SARG REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR}/../../build_system)

include (${CMAKE_CURRENT_LIST_DIR}/../../lib/seqan3/test/seqan3-test.cmake)
include (GoogleTest OPTIONAL)

include (diagnostics/list_missing_unit_tests)
include (diagnostics/list_unused_unit_tests)
include (include_dependencies/add_include_dependencies)

option (SARG_VERBOSE_TESTS "Run each test case individually" ON)
option (SARG_USE_INCLUDE_DEPENDENCIES "Build tests in an hierarchical order (by an include graph, i.e. tests with less dependencies are build first)" OFF)

macro (sarg_test unit_test_cpp)
    cmake_parse_arguments(SARG_TEST "" "" "CYCLIC_DEPENDING_INCLUDES" ${ARGN})

    file (RELATIVE_PATH unit_test "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_DIR}/${unit_test_cpp}")
    seqan3_test_component (target "${unit_test}" TARGET_NAME)
    seqan3_test_component (test_name "${unit_test}" TEST_NAME)

    add_executable (${target} ${unit_test_cpp})
    target_link_libraries (${target} seqan3::test::unit sarg::sarg)
    add_include_dependencies (${target} "${SARG_TEST_CYCLIC_DEPENDING_INCLUDES}")
    collect_used_test (${target})
    if (SARG_VERBOSE_TESTS)
        gtest_discover_tests(${target} TEST_PREFIX "${test_name}::" PROPERTIES TIMEOUT "30")
    else ()
        add_test (NAME "${test_name}" COMMAND ${target})
    endif ()

    unset (unit_test)
    unset (target)
    unset (test_name)
endmacro ()

seqan3_require_ccache ()
seqan3_require_test ()

list_missing_unit_tests()

# this does not work, yet:
#list_unused_unit_tests ()

# Top-level tests

add_subdirectories ()

sarg_test(format_parse_test.cpp)
sarg_test(format_parse_validators_test.cpp)
sarg_test(argument_parser_design_error_test.cpp)
