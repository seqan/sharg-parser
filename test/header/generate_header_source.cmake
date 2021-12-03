cmake_minimum_required (VERSION 3.10)

option (HEADER_FILE_ABSOLUTE "")
option (HEADER_FILE_INCLUDE "")
option (HEADER_TARGET_SOURCE "")
option (HEADER_TEST_NAME_SAFE "")
option (HEADER_COMPONENT "")
option (HEADER_SUB_TEST "")

file (WRITE "${HEADER_TARGET_SOURCE}" "") # write empty file

if (HEADER_SUB_TEST STREQUAL "no-self-include")
    # this test ensures that a header will not be included by itself later
    file (READ "${HEADER_FILE_ABSOLUTE}" header_content)

    string (REPLACE "#pragma once" "" header_content "${header_content}")

    file (APPEND "${HEADER_TARGET_SOURCE}" "${header_content}")
else ()
    # this test ensures that a header guard is in place
    file (APPEND "${HEADER_TARGET_SOURCE}" "
#include <${HEADER_FILE_INCLUDE}>
#include <${HEADER_FILE_INCLUDE}>")
endif()

# these includes are required by some headers (note that they follow)
file (APPEND "${HEADER_TARGET_SOURCE}" "
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
TEST(${HEADER_TEST_NAME_SAFE}) {}")

# test that sharg headers include platform.hpp
if ("${HEADER_COMPONENT}" MATCHES "sharg")

    # exclude sharg/std/*, sharg/contrib/*, and sharg/version.hpp from platform test
    if (NOT HEADER_FILE_INCLUDE MATCHES "sharg/(std/|contrib/|version.hpp)")
        file (APPEND "${HEADER_TARGET_SOURCE}" "
#ifndef SHARG_DOXYGEN_ONLY
#error \"Your header '${HEADER_FILE_INCLUDE}' file is missing #include <sharg/platform.hpp>\"
#endif")
    endif ()

    # sharg/std/* must not include platform.hpp (and therefore any other sharg header)
    # See https://github.com/seqan/product_backlog/issues/135
    if (HEADER_FILE_INCLUDE MATCHES "sharg/std/")
        file (APPEND "${HEADER_TARGET_SOURCE}" "
#ifdef SHARG_DOXYGEN_ONLY
#error \"The standard header '${HEADER_FILE_INCLUDE}' file MUST NOT include any other sharg header (except for sharg/contrib)\"
#endif")
    endif ()

    # test whether sharg has the visibility bug on lower gcc versions
    # https://github.com/seqan/seqan3/issues/1317
    if (NOT HEADER_FILE_INCLUDE MATCHES "sharg/version.hpp")
        file (APPEND "${HEADER_TARGET_SOURCE}" "
#include <sharg/platform.hpp>
class A{ int i{5}; };

template <typename t>
concept private_bug = requires(t a){a.i;};

static_assert(!private_bug<A>, \"See https://github.com/seqan/seqan3/issues/1317\");")
    endif ()

endif ()
