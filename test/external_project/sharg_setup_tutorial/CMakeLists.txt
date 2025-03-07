# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

cmake_minimum_required (VERSION 3.12)

# --- helper scripts
include (../find-package-diagnostics.cmake)
# ---

# Copy the cpp so we can use add_executable like in the tutorial
file (COPY "${CMAKE_SOURCE_DIR}/../src/hello_world.cpp" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file (RENAME "${CMAKE_CURRENT_BINARY_DIR}/hello_world.cpp" "${CMAKE_CURRENT_BINARY_DIR}/another_program.cpp")
file (COPY "${CMAKE_SOURCE_DIR}/../src/hello_world.cpp" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
# In the tutorial, the CMAKE_CURRENT_SOURCE_DIR is a sibling directory of the sharg checkout
set (CMAKE_CURRENT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../source")

## [adding_files]
cmake_minimum_required (VERSION 3.12)
project (sharg_tutorial CXX)

# add sharg to search path
list (APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../sharg-parser/cmake")

# require sharg with a version between >=1.0.0 and <2.0.0
find_package (sharg 1.0 REQUIRED)

# build app with sharg
add_executable (hello_world hello_world.cpp)
target_link_libraries (hello_world sharg::sharg)

add_executable (another_program another_program.cpp)
target_link_libraries (another_program sharg::sharg)
## [adding_files]

install (TARGETS hello_world another_program)
