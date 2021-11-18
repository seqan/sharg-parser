cmake_minimum_required (VERSION 3.8)

include (seqan3_test_files)
include (seqan3_test_component)

add_library ("${PROJECT_NAME}_header_test_lib" INTERFACE)
target_link_libraries ("${PROJECT_NAME}_header_test_lib" INTERFACE "${PROJECT_NAME}_interface" gtest gtest_main)
target_include_directories ("${PROJECT_NAME}_header_test_lib" INTERFACE "${SEQAN3_TEST_CLONE_DIR}/googletest/include/")
# SeqAn3 script adds an include for <bechmark/benchmark.h>
target_include_directories ("${PROJECT_NAME}_header_test_lib" INTERFACE ./dummy_include)
set (target "header_test")

set (header_base_path "${CMAKE_CURRENT_LIST_DIR}/../../include/")

file (GLOB_RECURSE header_files RELATIVE "${header_base_path}" "${header_base_path}/*.hpp")

file (WRITE "${PROJECT_BINARY_DIR}/${target}.cpp" "")
add_executable (${target} ${PROJECT_BINARY_DIR}/${target}.cpp)
target_link_libraries (${target} ${PROJECT_NAME}_header_test_lib)
add_test (NAME "header/${target}" COMMAND ${target})

foreach (header ${header_files})
    seqan3_test_component (header_test_name "${header}" TEST_NAME)
    seqan3_test_component (header_target_name "${header}" TARGET_UNIQUE_NAME)

    foreach (header_sub_test "header-guard" "no-self-include")
        set (header_target_source "${PROJECT_BINARY_DIR}/${target}_files/${header_test_name}.hpp-${header_sub_test}.cpp")
        set (header_target "${target}--${header_target_name}-${header_sub_test}")

        string (REPLACE "-" "__" header_test_name_safe "${target}, ${header_target}")

        # we use add_custom_command to detect changes to a header file, which will update the generated source file
        add_custom_command (OUTPUT "${header_target_source}"
                            COMMAND "${CMAKE_COMMAND}"
                                    "-DHEADER_FILE_ABSOLUTE=${header_base_path}/${header}"
                                    "-DHEADER_FILE_INCLUDE=${header}"
                                    "-DHEADER_TARGET_SOURCE=${header_target_source}"
                                    "-DHEADER_TEST_NAME_SAFE=${header_test_name_safe}"
                                    "-DHEADER_COMPONENT=${PROJECT_NAME}"
                                    "-DHEADER_SUB_TEST=${header_sub_test}"
                                    "-P"
                                    "${SEQAN3_CLONE_DIR}/test/header/generate_header_source.cmake"
                            DEPENDS "${header_base_path}/${header}"
                                    "${SEQAN3_CLONE_DIR}/test/header/generate_header_source.cmake")

        add_library (${header_target} OBJECT "${header_target_source}")

        # NOTE: a simple target_link_libraries (${header_target} ${PROJECT_NAME}_header_test_lib)
        # is not possible for OBJECT libraries before cmake 3.12
        if (CMAKE_VERSION VERSION_LESS 3.12)
            target_compile_options (${header_target} PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME}_header_test_lib,INTERFACE_COMPILE_OPTIONS>)
            target_compile_definitions (${header_target} PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME}_header_test_lib,INTERFACE_COMPILE_DEFINITIONS>)
            target_include_directories (${header_target} PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME}_header_test_lib,INTERFACE_INCLUDE_DIRECTORIES>)
            add_dependencies (${header_target} "${PROJECT_NAME}_interface" gtest gtest_main)
        else ()
            target_link_libraries (${header_target} ${PROJECT_NAME}_header_test_lib)
        endif ()

        target_sources (${target} PRIVATE $<TARGET_OBJECTS:${header_target}>)
    endforeach ()
endforeach ()
