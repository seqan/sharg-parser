# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

# This file provides functionality common to the different test modules used by
# SeqAn3. To build tests, run cmake on one of the sub-folders in this directory
# which contain a CMakeLists.txt.

cmake_minimum_required (VERSION 3.12)

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 23)
endif ()

if (NOT DEFINED CMAKE_CXX_STANDARD_REQUIRED)
    set (CMAKE_CXX_STANDARD_REQUIRED ON)
endif ()

if (NOT DEFINED CMAKE_CXX_EXTENSIONS)
    set (CMAKE_CXX_EXTENSIONS OFF)
endif ()

set (SHARG_TEST_CPM_DIR
     "${CMAKE_CURRENT_LIST_DIR}/../cmake/"
     CACHE STRING "Path to directory containing CPM.cmake.")

set (CPM_INDENT "CMake Package Manager CPM: ")
include ("${SHARG_TEST_CPM_DIR}/CPM.cmake")
CPMUsePackageLock ("${SHARG_TEST_CPM_DIR}/package-lock.cmake")

include (${CMAKE_CURRENT_LIST_DIR}/cmake/sharg_require_ccache.cmake)

# require Sharg package
find_package (Sharg REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR}/../cmake)

enable_testing ()

include (CheckCXXSourceCompiles)
include (FindPackageHandleStandardArgs)
include (FindPackageMessage)

option (SHARG_TEST_BUILD_OFFLINE "Skip the update step of external projects." OFF)

option (SHARG_WITH_WERROR "Report compiler warnings as errors." ON)

# ----------------------------------------------------------------------------
# Paths to folders.
# ----------------------------------------------------------------------------

find_path (SHARG_TEST_INCLUDE_DIR
           NAMES sharg/test/tmp_filename.hpp
           HINTS "${CMAKE_CURRENT_LIST_DIR}/include/")
find_path (SHARG_TEST_CMAKE_MODULE_DIR
           NAMES sharg_test_component.cmake
           HINTS "${CMAKE_CURRENT_LIST_DIR}/cmake/")
list (APPEND CMAKE_MODULE_PATH "${SHARG_TEST_CMAKE_MODULE_DIR}")

# ----------------------------------------------------------------------------
# Interface targets for the different test modules in seqan3.
# ----------------------------------------------------------------------------

# sharg::test exposes a base set of required flags, includes, definitions and
# libraries which are in common for **all** seqan3 tests
if (NOT TARGET sharg::test)
    add_library (sharg_test INTERFACE)
    target_compile_options (sharg_test INTERFACE "-pedantic" "-Wall" "-Wextra")

    if (SHARG_WITH_WERROR)
        target_compile_options (sharg_test INTERFACE "-Werror")
        message (STATUS "Building tests with -Werror.")
    endif ()

    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # GCC12 and above: Disable warning about std::hardware_destructive_interference_size not being ABI-stable.
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 12)
            target_compile_options (sharg_test INTERFACE "-Wno-interference-size")
        endif ()

        # Warn about failed return value optimization.
        if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 14)
            target_compile_options (sharg_test INTERFACE "-Wnrvo")
        endif ()
    endif ()

    target_link_libraries (sharg_test INTERFACE "sharg::sharg" "pthread")
    target_include_directories (sharg_test INTERFACE "${SHARG_TEST_INCLUDE_DIR}")
    add_library (sharg::test ALIAS sharg_test)
endif ()

# sharg::test::unit specifies required flags, includes and libraries
# needed for unit test cases in sharg/test/unit
if (NOT TARGET sharg::test::unit)
    add_library (sharg_test_unit INTERFACE)
    target_link_libraries (sharg_test_unit INTERFACE "sharg::test" "GTest::gtest_main")
    add_library (sharg::test::unit ALIAS sharg_test_unit)
endif ()

# sharg::test::header specifies required flags, includes and libraries
# needed for header test cases in sharg/test/header
if (NOT TARGET sharg::test::header)
    add_library (sharg_test_header INTERFACE)
    target_link_libraries (sharg_test_header INTERFACE "sharg::test::unit")
    target_compile_options (sharg_test_header INTERFACE "-Wno-unused-const-variable")
    target_compile_definitions (sharg_test_header INTERFACE -DSHARG_DISABLE_DEPRECATED_WARNINGS)
    target_compile_definitions (sharg_test_header INTERFACE -DSHARG_HEADER_TEST)
    add_library (sharg::test::header ALIAS sharg_test_header)
endif ()

# ----------------------------------------------------------------------------
# Commonly shared options for external projects.
# ----------------------------------------------------------------------------

set (SHARG_EXTERNAL_PROJECT_CMAKE_ARGS "")
list (APPEND SHARG_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}")
list (APPEND SHARG_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
list (APPEND SHARG_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}")
list (APPEND SHARG_EXTERNAL_PROJECT_CMAKE_ARGS "-DCMAKE_VERBOSE_MAKEFILE=${CMAKE_VERBOSE_MAKEFILE}")

# ----------------------------------------------------------------------------
# Commonly used macros for the different test modules in seqan3.
# ----------------------------------------------------------------------------

include (sharg_test_component)
include (sharg_test_files)
include (add_subdirectories)
