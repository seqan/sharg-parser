# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10)
project (sharg_test_snippet CXX)

include (../sharg-test.cmake)
include (../cmake/diagnostics/list_unused_snippets.cmake)

add_library (snippet_main snippet_main.cpp)
target_link_libraries (snippet_main PUBLIC sharg::test gtest)

macro (sharg_snippet test_name_prefix snippet snippet_base_path)
    sharg_test_component (snippet_target_name "${snippet}" TARGET_NAME)
    sharg_test_component (snippet_test_name "${snippet}" TEST_NAME)
    sharg_test_component (snippet_target_path "${snippet}" TARGET_PATH)
    set (target "${snippet_target_name}_snippet")

    add_executable (${target} "${snippet_base_path}/${snippet}")
    target_link_libraries (${target} PUBLIC snippet_main)
    set_target_properties(${target}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${snippet_target_path}"
    )
    collect_used_snippet (${target})

    set (snippet_test_target "${test_name_prefix}/${snippet_test_name}_snippet")
    add_test (NAME "${snippet_test_target}" COMMAND ${target})

    # disable version checker, as it interferes with comparing the snippet output
    set_tests_properties ("${snippet_test_target}" PROPERTIES ENVIRONMENT SHARG_NO_VERSION_CHECK=0)

    set (snippet_compare_test_target "${test_name_prefix}/${snippet_test_name}_snippet_cmp_output")

    add_test (NAME "${snippet_compare_test_target}"
              COMMAND ${CMAKE_COMMAND}
                      -DTARGET_FILE=$<TARGET_FILE:${target}>
                      -DSOURCE_FILE=${snippet_base_path}/${snippet}
                      -P "${CMAKE_SOURCE_DIR}/compare_snippet_output.cmake")

    # disable version checker, as it interferes with comparing the snippet output
    set_tests_properties ("${snippet_compare_test_target}" PROPERTIES ENVIRONMENT SHARG_NO_VERSION_CHECK=0)

    unset (snippet_target_name)
    unset (snippet_test_name)
    unset (snippet_target_path)
    unset (target)
endmacro ()

macro (sharg_snippets test_name_prefix snippet_base_path_)
    get_filename_component(snippet_base_path "${snippet_base_path_}" ABSOLUTE BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    message (STATUS "${snippet_base_path}")
    sharg_test_files (snippet_files "${snippet_base_path}" "*.cpp")
    foreach (snippet ${snippet_files})
        sharg_snippet("${test_name_prefix}" "${snippet}" "${snippet_base_path}")
    endforeach ()
endmacro ()

sharg_require_ccache ()
sharg_require_test ()

sharg_snippets ("snippet" "${CMAKE_CURRENT_SOURCE_DIR}")
sharg_snippets ("doc/snippet" "${CMAKE_CURRENT_SOURCE_DIR}/../../doc")

list_unused_snippets ("${CMAKE_CURRENT_SOURCE_DIR}")
list_unused_snippets ("${CMAKE_CURRENT_SOURCE_DIR}/../../doc")

