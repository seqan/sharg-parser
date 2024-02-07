# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required (VERSION 3.14)
project (sharg_app CXX)

# --- helper scripts
include (../find-package-diagnostics.cmake)
file (SHA256 "${SHARG_PACKAGE_ZIP_URL}" SHARG_PACKAGE_ZIP_HASH)
message (STATUS "SHARG_PACKAGE_ZIP_URL: ${SHARG_PACKAGE_ZIP_URL}")
message (STATUS "SHARG_PACKAGE_ZIP_HASH: SHA256=${SHARG_PACKAGE_ZIP_HASH}")
# ---

# fetch sharg sources (requires >= cmake 3.14)
include (FetchContent)
FetchContent_Declare (
    sharg
    URL "${SHARG_PACKAGE_ZIP_URL}" # change these values
    URL_HASH "SHA256=${SHARG_PACKAGE_ZIP_HASH}" # change these values
)
FetchContent_MakeAvailable (sharg)

# add sharg to search path
list (APPEND CMAKE_PREFIX_PATH "${sharg_SOURCE_DIR}")

# require sharg with a version between >=1.0.0 and <2.0.0
find_package (sharg 1.0 REQUIRED)

# build app with sharg
add_executable (hello_world ../src/hello_world.cpp)
target_link_libraries (hello_world sharg::sharg)
install (TARGETS hello_world)
