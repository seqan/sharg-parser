cmake_minimum_required (VERSION 3.8)

# Set directories for test output files, input data and binaries.
file (MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output)
add_definitions (-DOUTPUTDIR=\"${CMAKE_CURRENT_BINARY_DIR}/output/\")
add_definitions (-DDATADIR=\"${CMAKE_CURRENT_BINARY_DIR}/data/\")
add_definitions (-DBINDIR=\"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/\")

# Define cmake configuration flags to configure and build external projects with the same flags as specified for
# this project.
# We also pass the C_COMPILER such that googletest is built with the corresponding GCC.
# Otherwise, it might happen that the app is built with, e.g., g++-11, but gtest with gcc-7, which might cause trouble.
set (SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS "")
list (APPEND SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}")
list (APPEND SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
list (APPEND SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
list (APPEND SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}")
list (APPEND SEQAN3_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}")
set (SEQAN3_TEST_CLONE_DIR "${PROJECT_BINARY_DIR}/vendor/googletest")

include (seqan3_require_test)
seqan3_require_test ()

# Build tests just before their execution, because they have not been built with "all" target.
# The trick is here to provide a cmake file as a directory property that executes the build command.
file (WRITE "${CMAKE_CURRENT_BINARY_DIR}/build_test_targets.cmake"
            "execute_process (COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target api_test)\n"
            "execute_process (COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target cli_test)\n"
            "execute_process (COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target header_test)\n")
set_directory_properties (PROPERTIES TEST_INCLUDE_FILE "${CMAKE_CURRENT_BINARY_DIR}/build_test_targets.cmake")

# Test executables and libraries should not mix with the application files.
unset (CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
unset (CMAKE_LIBRARY_OUTPUT_DIRECTORY)
unset (CMAKE_RUNTIME_OUTPUT_DIRECTORY)

include (seqan3_test_component)

# A macro that adds an api or cli test.
macro (add_app_test test_filename)
    # Extract the test target name.
    file (RELATIVE_PATH source_file "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_LIST_DIR}/${test_filename}")
    seqan3_test_component (target "${source_file}" TARGET_NAME)
    seqan3_test_component (test_name "${source_file}" TEST_NAME)

    # Create the test target.
    add_executable (${target} ${test_filename})
    target_link_libraries (${target} "${PROJECT_NAME}_lib" seqan3::seqan3 gtest gtest_main)

    # Make seqan3::test available for both cli and api tests.
    target_include_directories (${target} PUBLIC "${SEQAN3_CLONE_DIR}/test/include")
    target_include_directories (${target} PUBLIC "${SEQAN3_TEST_CLONE_DIR}/googletest/include/")

    # Generate and set the test name.
    add_test (NAME "${test_name}" COMMAND ${target})

    unset (source_file)
    unset (target)
    unset (test_name)
endmacro ()

string (TOUPPER ${PROJECT_NAME} uppercase_project_name)
set (${uppercase_project_name}_HEADER_TEST_ONLY OFF CACHE BOOL "Only build header test.")

if (${uppercase_project_name}_HEADER_TEST_ONLY)
    add_subdirectory (header)
else ()
    # Fetch data and add the tests.
    include (data/datasources.cmake)
    add_subdirectory (api)
    add_subdirectory (cli)
    add_subdirectory (coverage)
endif ()

message (STATUS "${FontBold}You can run `make test` to build and run tests.${FontReset}")
