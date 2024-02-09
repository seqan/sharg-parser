# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required (VERSION 3.16)
project (sharg_test_external_project CXX)

include (../sharg-test.cmake) # for SHARG_EXTERNAL_PROJECT_CMAKE_ARGS, SHARG_VERSION
include (ExternalProject)

set (SHARG_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../")

include (install-sharg.cmake)

option (SHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE
        "Enable this option if you want to get a detailed list which paths were considered for find_package(...)" FALSE)

# 1) This tests test/external_project/sharg_submodule_add_subdirectory/CMakeLists.txt
#    That means we use add_subdirectory directly on sharg's top level CMakeLists.txt.
#    This will automatically call find_package and expose our sharg::sharg target.
#    This is expected to work with CMake >= 3.4.
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    sharg_submodule_add_subdirectory
    PREFIX sharg_submodule_add_subdirectory
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sharg_submodule_add_subdirectory"
    CMAKE_ARGS ${SHARG_EXTERNAL_PROJECT_CMAKE_ARGS} #
               "-DCMAKE_FIND_DEBUG_MODE=${SHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE}" #
               "-DSHARG_ROOT=${SHARG_ROOT}" #
               "-DSHARG_NO_TDL=${SHARG_NO_TDL}")

# 2) This tests test/external_project/sharg_submodule_find_package/CMakeLists.txt
#    We have a sharg checkout somewhere and we point CMAKE_PREFIX_PATH to <checkout>/sharg/build_system
#    and then use `find_package` to find `sharg-config.cmake` which exposes our `sharg::sharg` target.
#    This is expected to work with CMake >= 3.4.
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    sharg_submodule_find_package
    PREFIX sharg_submodule_find_package
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sharg_submodule_find_package"
    CMAKE_ARGS ${SHARG_EXTERNAL_PROJECT_CMAKE_ARGS} #
               "-DCMAKE_FIND_DEBUG_MODE=${SHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE}" #
               "-DCMAKE_PREFIX_PATH=${SHARG_ROOT}/build_system" #
               "-DSHARG_NO_TDL=${SHARG_NO_TDL}")

# 3) This tests test/external_project/sharg_installed/CMakeLists.txt
#    This test assumes that sharg was installed by make install (e.g. system-wide).
#    This is the way most upstream packages, like debian, provide our library.
#    This test assumes that `sharg-config.cmake` can be found by cmake in some global paths like /usr/share/cmake/.
#
#    We simulate this by using our `make package` release, e.g. the one we release under
#    https://github.com/seqan/sharg/releases, and unzipping it to some folder and making
#    that path globally accessible by CMAKE_SYSTEM_PREFIX_PATH.
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    sharg_installed
    PREFIX sharg_installed
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sharg_installed"
    CMAKE_ARGS ${SHARG_EXTERNAL_PROJECT_CMAKE_ARGS} #
               "-DCMAKE_FIND_DEBUG_MODE=${SHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE}" #
               "-DCMAKE_SYSTEM_PREFIX_PATH=${SHARG_SYSTEM_PREFIX}" #
               "-DSHARG_NO_TDL=${SHARG_NO_TDL}")
add_dependencies (sharg_installed sharg_test_prerequisite)

# 4) This tests test/external_project/sharg_fetch_content_zip/CMakeLists.txt
#    It uses fetch_content (a CMake 3.14 feature) to download our zip-release (e.g. zip, tar.xz) from
#    https://github.com/seqan/sharg/releases. fetch_content will automatically download, verify, extract it.
#    The user only needs to define CMAKE_PREFIX_PATH to be able to find our `sharg-config.cmake`.
#    Note that FetchContent is a CMake >= 3.14 feature.
#    This is expected to work with CMake >= 3.14.
# (ExternalProject_Add simulates a fresh and separate invocation of cmake ../)
ExternalProject_Add (
    sharg_fetch_content_zip
    PREFIX sharg_fetch_content_zip
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sharg_fetch_content_zip"
    CMAKE_ARGS ${SHARG_EXTERNAL_PROJECT_CMAKE_ARGS} #
               "-DCMAKE_FIND_DEBUG_MODE=${SHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE}" #
               "-DSHARG_PACKAGE_ZIP_URL=${SHARG_PACKAGE_ZIP_URL}" #
               "-DSHARG_NO_TDL=${SHARG_NO_TDL}")
add_dependencies (sharg_fetch_content_zip sharg_test_prerequisite)

# 5) This test is the same as 2) but emulates the settings within the setup tutorial.
#    This test is used as snippet in the setup tutorial.
ExternalProject_Add (
    sharg_setup_tutorial
    PREFIX sharg_setup_tutorial
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sharg_setup_tutorial"
    CMAKE_ARGS ${SHARG_EXTERNAL_PROJECT_CMAKE_ARGS} #
               "-DCMAKE_FIND_DEBUG_MODE=${SHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE}" #
               "-DSHARG_NO_TDL=${SHARG_NO_TDL}")
