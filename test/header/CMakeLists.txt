# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10)
project (sharg_header_test CXX)

include (../sharg-test.cmake)

macro (sharg_header_test component header_base_path exclude_regex)
    set (target "${component}_header_test")

    sharg_test_files (header_files "${header_base_path}" "*.hpp;*.h")

    if (NOT ";${exclude_regex};" STREQUAL ";;")
        list (FILTER header_files EXCLUDE REGEX "${exclude_regex}")
    endif()

    file (WRITE "${PROJECT_BINARY_DIR}/${target}.cpp" "")
    add_executable (${target} ${PROJECT_BINARY_DIR}/${target}.cpp)
    target_link_libraries (${target} sharg::test::header)
    add_test (NAME "header/${target}" COMMAND ${target})

    foreach (header ${header_files})
        sharg_test_component (header_test_name "${header}" TEST_NAME)
        sharg_test_component (header_target_name "${header}" TARGET_UNIQUE_NAME)

        foreach (header_sub_test "header-guard" "no-self-include")
            set (header_target_source "${PROJECT_BINARY_DIR}/${target}_files/${header_test_name}.hpp-${header_sub_test}.cpp")
            set (header_target "${target}--${header_target_name}-${header_sub_test}")

            string (REPLACE "-" "__" header_test_name_safe "${target}, ${header_target}")

            add_custom_command (OUTPUT "${header_target_source}"
                                COMMAND "${CMAKE_COMMAND}"
                                        "-DHEADER_FILE_ABSOLUTE=${header_base_path}/${header}"
                                        "-DHEADER_FILE_INCLUDE=${header}"
                                        "-DHEADER_TARGET_SOURCE=${header_target_source}"
                                        "-DHEADER_TEST_NAME_SAFE=${header_test_name_safe}"
                                        "-DHEADER_COMPONENT=${component}"
                                        "-DHEADER_SUB_TEST=${header_sub_test}"
                                        "-P"
                                        "${CMAKE_CURRENT_SOURCE_DIR}/generate_header_source.cmake"
                                DEPENDS "${header_base_path}/${header}"
                                        "${CMAKE_CURRENT_SOURCE_DIR}/generate_header_source.cmake")
            add_library (${header_target} OBJECT "${header_target_source}")

            if (CMAKE_VERSION VERSION_LESS 3.12)
                target_compile_options (${header_target} PRIVATE $<TARGET_PROPERTY:sharg::test::header,INTERFACE_COMPILE_OPTIONS>)
                target_compile_definitions (${header_target} PRIVATE $<TARGET_PROPERTY:sharg::test::header,INTERFACE_COMPILE_DEFINITIONS>)
                target_include_directories (${header_target} PRIVATE $<TARGET_PROPERTY:sharg::test::header,INTERFACE_INCLUDE_DIRECTORIES>)
                target_include_directories (${header_target} SYSTEM PRIVATE $<TARGET_PROPERTY:sharg::test::header,INTERFACE_SYSTEM_INCLUDE_DIRECTORIES>)
                add_dependencies (${header_target} gtest gbenchmark)
            else ()
                target_link_libraries (${header_target} sharg::test::header)
            endif ()

            target_sources (${target} PRIVATE $<TARGET_OBJECTS:${header_target}>)
        endforeach ()
    endforeach ()

    unset (target)
    unset (header_files)
    unset (header_test_name)
    unset (header_test_name_safe)
    unset (header_target_name)
    unset (header_target_source)
    unset (header_target)
endmacro ()

sharg_require_ccache ()
sharg_require_benchmark ()
sharg_require_test ()

sharg_header_test (sharg "${SHARG_INCLUDE_DIR}" "")
