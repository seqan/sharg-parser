# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.14)

# install and package sharg library
ExternalProject_Add (
    sharg_test_prerequisite
    PREFIX sharg_test_prerequisite
    SOURCE_DIR "${SHARG_ROOT}"
    CMAKE_ARGS ${SHARG_EXTERNAL_PROJECT_CMAKE_ARGS} #
               "-DCMAKE_INSTALL_PREFIX=<BINARY_DIR>/usr"
    STEP_TARGETS configure install
    BUILD_BYPRODUCTS "<BINARY_DIR>/include")

set (SHARG_PACKAGE_ZIP_URL "${PROJECT_BINARY_DIR}/sharg-${SHARG_VERSION}-${CMAKE_SYSTEM_NAME}.zip")
ExternalProject_Add_Step (
    sharg_test_prerequisite package
    COMMAND ${CMAKE_CPACK_COMMAND} -G ZIP -B "${PROJECT_BINARY_DIR}"
    DEPENDEES configure install
    WORKING_DIRECTORY "<BINARY_DIR>"
    BYPRODUCTS ${SHARG_PACKAGE_ZIP_URL} #
               "${SHARG_PACKAGE_ZIP_URL}.sha256")

ExternalProject_Get_Property (sharg_test_prerequisite BINARY_DIR)
set (SHARG_SYSTEM_PREFIX "${BINARY_DIR}/usr")
