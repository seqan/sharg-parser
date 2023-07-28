# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.4)
project (sharg_app CXX)

# --- helper scripts
include (../find-package-diagnostics.cmake)
# ---

# add sharg to search path
list (APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/sharg/build_system") # change this value

# require sharg with a version between >=1.0.0 and <2.0.0
find_package (sharg 1.0 REQUIRED)

# build app with sharg
add_executable (hello_world ../src/hello_world.cpp)
target_link_libraries (hello_world sharg::sharg)
install (TARGETS hello_world)