# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required (VERSION 3.12)

### Find doxygen and dependency to DOT tool
message (STATUS "Searching for doxygen.")
find_package (Doxygen REQUIRED)

if (NOT ${DOXYGEN_FOUND})
    message (FATAL_ERROR "Could not find doxygen. Not building documentation.")
endif ()

if (NOT ${DOXYGEN_DOT_FOUND})
    message (STATUS "Could not find dot tool. Disabling dot support.")
    set (SHARG_DOXYGEN_HAVE_DOT "NO")
else ()
    message (STATUS "Found dot tool. Enabling dot support.")
    set (SHARG_DOXYGEN_HAVE_DOT "YES")
endif ()

### Number of threads to use for dot. Doxygen's default is 0 (all threads).
set (SHARG_DOXYGEN_DOT_NUM_THREADS "0")

### Configure doc/developer targets.
set (SHARG_DOXYGEN_SOURCE_DIR "${SHARG_CLONE_DIR}")
set (SHARG_DOXYFILE_IN ${SHARG_DOXYGEN_INPUT_DIR}/sharg_doxygen_cfg.in)
set (SHARG_FOOTER_HTML_IN ${SHARG_DOXYGEN_INPUT_DIR}/sharg_footer.html.in)
set (SHARG_LAYOUT_IN ${SHARG_DOXYGEN_INPUT_DIR}/DoxygenLayout.xml.in)

option (SHARG_USER_DOC "Create build target and test for user documentation." ON)
option (SHARG_DEV_DOC "Create build target and test for developer documentation." ON)

### Download and extract cppreference-doxygen-web.tag.xml for std:: documentation links
set (SHARG_DOXYGEN_STD_TAGFILE "${PROJECT_BINARY_DIR}/cppreference-doxygen-web.tag.xml")
include (ExternalProject)
ExternalProject_Add (
    download-cppreference-doxygen-web-tag
    URL "https://github.com/PeterFeicht/cppreference-doc/releases/download/v20220730/html-book-20220730.tar.xz"
    URL_HASH SHA256=71f15003c168b8dc5a00cbaf19b6480a9b3e87ab7e462aa39edb63d7511c028b
    TLS_VERIFY ON
    DOWNLOAD_DIR "${PROJECT_BINARY_DIR}"
    DOWNLOAD_NAME "html-book.tar.xz"
    DOWNLOAD_NO_EXTRACT YES
    BINARY_DIR "${PROJECT_BINARY_DIR}"
    CONFIGURE_COMMAND /bin/sh -c "xzcat html-book.tar.xz | tar -xf - cppreference-doxygen-web.tag.xml"
    BUILD_COMMAND rm "html-book.tar.xz"
    INSTALL_COMMAND "")

### TEST HELPER
add_test (NAME cppreference-doxygen-web-tag COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target
                                                    download-cppreference-doxygen-web-tag)

# doxygen does not show any warnings (doxygen prints warnings / errors to cerr)
set (SHARG_TEST_DOXYGEN_FAIL_ON_WARNINGS
     "
    ${DOXYGEN_EXECUTABLE} > doxygen.cout 2> doxygen.cerr;
    cat \"doxygen.cerr\";
    test ! -s \"doxygen.cerr\"")

### install helper

# make sure that prefix path is /usr/local/share/doc/sharg/
if (NOT DEFINED CMAKE_SIZEOF_VOID_P)
    # we need this to suppress GNUInstallDirs AUTHOR_WARNING:
    #   CMake Warning (dev) at /usr/share/cmake-3.19/Modules/GNUInstallDirs.cmake:223 (message):
    #     Unable to determine default CMAKE_INSTALL_LIBDIR directory because no
    #     target architecture is known.  Please enable at least one language before
    #     including GNUInstallDirs.
    set (CMAKE_SIZEOF_VOID_P 8)
endif ()
include (GNUInstallDirs) # this is needed to prefix the install paths
