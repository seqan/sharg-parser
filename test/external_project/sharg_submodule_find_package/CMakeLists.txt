# SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required (VERSION 3.12)
project (sharg_app CXX)

# --- helper scripts
include (../find-package-diagnostics.cmake)
# ---

# add sharg to search path
list (APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/submodules/sharg/cmake")

# require sharg with a version between >=1.0.0 and <2.0.0
find_package (sharg 1.0 REQUIRED)

# build app with sharg
add_executable (hello_world ../src/hello_world.cpp)
target_link_libraries (hello_world sharg::sharg)
install (TARGETS hello_world)
