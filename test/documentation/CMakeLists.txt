# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

# Minimum cmake version
cmake_minimum_required(VERSION 3.7)

### Find sharg
include (../../build_system/sharg-config-version.cmake)
set (SHARG_VERSION "${PACKAGE_VERSION}")

project (sharg LANGUAGES NONE VERSION "${SHARG_PROJECT_VERSION}")

if (NOT EXISTS "${SHARG_INCLUDE_DIR}/sharg/version.hpp")
    message (FATAL_ERROR "Could not find sharg. Not building documentation.")
endif ()
message (STATUS "Found: ${SHARG_INCLUDE_DIR} (${SHARG_VERSION})")

set (SHARG_DOXYGEN_INPUT_DIR "${CMAKE_SOURCE_DIR}")

include (sharg-doxygen.cmake)

enable_testing ()

if (SHARG_USER_DOC)
    add_subdirectory(doc_usr)
endif ()

if (SHARG_DEV_DOC)
    add_subdirectory(doc_dev)
endif ()

include (sharg-doxygen-package.cmake)
