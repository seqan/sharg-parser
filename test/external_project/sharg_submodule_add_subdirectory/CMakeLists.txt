# SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required (VERSION 3.12)
project (sharg_app CXX)

# --- helper scripts
include (../find-package-diagnostics.cmake)
# ---

# use git checkout of sharg
option (INSTALL_SHARG "" OFF) # we don't need to install sharg
add_subdirectory ("${SHARG_ROOT}" "sharg_build")

# build app with sharg
add_executable (hello_world ../src/hello_world.cpp)
target_link_libraries (hello_world sharg::sharg)
install (TARGETS hello_world)
