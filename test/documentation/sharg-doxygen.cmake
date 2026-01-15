# SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
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
    URL "https://github.com/PeterFeicht/cppreference-doc/releases/download/v20250209/html-book-20250209.tar.xz"
    URL_HASH SHA256=ac50671a1f52d7f0ab0911d14450eb35e8c2f812edd0e426b2cd1e3d9db91d6f
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
# Second line filters warnings from tag file.
# Note: Because the commands are line-wise, CMake will insert a semicolon between them.
#       If this is changed to be a single line, the semicolon must be manually inserted.
set (SHARG_TEST_DOXYGEN_FAIL_ON_WARNINGS
     "${DOXYGEN_EXECUTABLE} -q > doxygen.cout 2> doxygen.cerr"
     "sed -i '/documented symbol '\\''T std::experimental::erase'\\'' was not declared or defined\\./d; /documented symbol '\\''T std::experimental::erase_if'\\'' was not declared or defined\\./d' \"doxygen.cerr\""
     "cat \"doxygen.cerr\""
     "test ! -s \"doxygen.cerr\""
     CACHE INTERNAL "The doxygen test command")

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
