# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

# Minimum cmake version
cmake_minimum_required (VERSION 3.7)

### Find sharg
include (../../cmake/sharg-config-version.cmake)
set (SHARG_VERSION "${PACKAGE_VERSION}")

# ----------------------------------------------------------------------------
# CPM
# ----------------------------------------------------------------------------

# This will be true for git clones and source packages.
if (EXISTS "${SHARG_INCLUDE_DIR}/../cmake/CPM.cmake")
    set (SHARG_HAS_CPM TRUE)
else ()
    set (SHARG_HAS_CPM FALSE)
endif ()

if (SHARG_HAS_CPM)
    set (CPM_INDENT "CMake Package Manager CPM: ")
    include ("${SHARG_INCLUDE_DIR}/../cmake/CPM.cmake")
    CPMUsePackageLock ("${SHARG_INCLUDE_DIR}/../cmake/package-lock.cmake")
endif ()

project (sharg
         LANGUAGES NONE
         VERSION "${SHARG_PROJECT_VERSION}")

if (NOT EXISTS "${SHARG_INCLUDE_DIR}/sharg/version.hpp")
    message (FATAL_ERROR "Could not find sharg. Not building documentation.")
endif ()
message (STATUS "Found: ${SHARG_INCLUDE_DIR} (${SHARG_VERSION})")

if (SHARG_HAS_CPM)
    CPMGetPackage (doxygen_awesome)
else ()
    message (FATAL_ERROR "Could not find doxygen-awesome. Not building documentation.")
endif ()
message (STATUS "Found: ${doxygen_awesome_SOURCE_DIR}")

set (SHARG_DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}")

include (sharg-doxygen.cmake)

enable_testing ()

if (SHARG_USER_DOC)
    add_subdirectory (doc_usr)
endif ()

if (SHARG_DEV_DOC)
    add_subdirectory (doc_dev)
endif ()

include (sharg-doxygen-package.cmake)
