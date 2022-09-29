# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10)

# A compatible function for cmake < 3.20 that basically returns `cmake_path (GET <filename> STEM LAST_ONLY <out_var>)`
function (sharg_path_longest_stem out_var filename)
    if (CMAKE_VERSION VERSION_LESS 3.20) # cmake < 3.20
        get_filename_component (result "${filename}" NAME)
        if (result MATCHES "\\.")
            string (REGEX REPLACE "(.+)[.].*" "\\1" result "${result}")
        endif ()
    else () # cmake >= 3.20
        cmake_path (GET filename STEM LAST_ONLY result)
    endif ()

    set ("${out_var}"
         "${result}"
         PARENT_SCOPE) # out-var
endfunction ()

# ======
# TESTS
# ======

sharg_path_longest_stem (sharg_cmake_test_path "/a/b/c/")
if (NOT sharg_cmake_test_path STREQUAL "")
    message (FATAL_ERROR "internal error: '${sharg_cmake_test_path}' vs '', "
                         "sharg_path_longest_stem produces wrong result")
endif ()

sharg_path_longest_stem (sharg_cmake_test_path "/a/b/c/hello")
if (NOT sharg_cmake_test_path STREQUAL "hello")
    message (FATAL_ERROR "internal error: '${sharg_cmake_test_path}' vs 'hello', "
                         "sharg_path_longest_stem produces wrong result")
endif ()

sharg_path_longest_stem (sharg_cmake_test_path "/a/b/c/hello.cpp")
if (NOT sharg_cmake_test_path STREQUAL "hello")
    message (FATAL_ERROR "internal error: '${sharg_cmake_test_path}' vs 'hello', "
                         "sharg_path_longest_stem produces wrong result")
endif ()

sharg_path_longest_stem (sharg_cmake_test_path "/a/b/c/hello.world.cpp")
if (NOT sharg_cmake_test_path STREQUAL "hello.world")
    message (FATAL_ERROR "internal error: '${sharg_cmake_test_path}' vs 'hello.world', "
                         "sharg_path_longest_stem produces wrong result")
endif ()
