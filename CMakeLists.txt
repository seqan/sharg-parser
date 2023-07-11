# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

# This file provides functionality common to the different test modules used by
# Sharg. To build tests, run cmake on one of the sub-folders in this directory
# which contain a CMakeLists.txt.

cmake_minimum_required (VERSION 3.12)

find_path (SHARG_MODULE_PATH "sharg-config.cmake" HINTS "${CMAKE_CURRENT_LIST_DIR}/build_system/")
list (APPEND CMAKE_MODULE_PATH "${SHARG_MODULE_PATH}")

include (sharg-config-version)

project (sharg
         LANGUAGES CXX
         VERSION "${SHARG_PROJECT_VERSION}"
         DESCRIPTION "Sharg -- hungrily eating away your arguments"
         HOMEPAGE_URL "https://github.com/seqan/sharg-parser")

find_package (Sharg 1.0 REQUIRED HINTS ${SHARG_MODULE_PATH})

option (INSTALL_SHARG "Enable installation of Sharg. (Projects embedding Sharg may want to turn this OFF.)" ON)

if (INSTALL_SHARG)
    include (sharg-install)
    include (sharg-package)
endif ()
