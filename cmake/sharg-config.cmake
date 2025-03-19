# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause
#
# This CMake module will try to find SHARG and its dependencies.  You can use
# it the same way you would use any other CMake module.
#
#   find_package (SHARG [REQUIRED] ...)
#
# Since this makes a difference for CMAKE, pay attention to the case
# ("SHARG", "SHARG" and "sharg" are all valid, but other names not).
#
# SHARG has the following platform requirements:
#
#   C++20
#   pthread
#
# SHARG has the following optional dependencies:
#
#   ZLIB      -- zlib compression library
#   BZip2     -- libbz2 compression library
#
# If you don't wish for these to be detected (and used), you may define SHARG_NO_ZLIB,
# SHARG_NO_BZIP2, SHARG_NO_CEREAL and SHARG_NO_LEMON respectively.
#
# If you wish to require the presence of ZLIB or BZip2, just check for the module before
# finding SHARG, e.g. "find_package (ZLIB REQUIRED)".
# If you wish to require the presence of CEREAL, you may define SHARG_CEREAL.
# If you wish to require the presence of LEMON, you may define SHARG_LEMON.
#
# Once the search has been performed, the following variables will be set.
#
#   SHARG_FOUND            -- Indicate whether SHARG was found and requirements met.
#
#   SHARG_VERSION          -- The version as string, e.g. "3.0.0"
#   SHARG_VERSION_MAJOR    -- e.g. 3
#   SHARG_VERSION_MINOR    -- e.g. 0
#   SHARG_VERSION_PATCH    -- e.g. 0
#
#   SHARG_INCLUDE_DIRS     -- to be passed to include_directories ()
#   SHARG_LIBRARIES        -- to be passed to target_link_libraries ()
#   SHARG_DEFINITIONS      -- to be passed to add_definitions ()
#
# Additionally, the following [IMPORTED][IMPORTED] targets are defined:
#
#   sharg::sharg          -- interface target where
#                                  target_link_libraries(target sharg::sharg)
#                              automatically sets
#                                  target_include_directories(target $SHARG_INCLUDE_DIRS),
#                                  target_link_libraries(target $SHARG_LIBRARIES),
#                                  target_compile_definitions(target $SHARG_DEFINITIONS) and
#                              for a target.
#
#   [IMPORTED]: https://cmake.org/cmake/help/v3.10/prop_tgt/IMPORTED.html#prop_tgt:IMPORTED
#
# ============================================================================

cmake_minimum_required (VERSION 3.4...3.12)

# ----------------------------------------------------------------------------
# Set initial variables
# ----------------------------------------------------------------------------

# make output globally quiet if required by find_package, this effects cmake functions like `check_*`
set (CMAKE_REQUIRED_QUIET_SAVE ${CMAKE_REQUIRED_QUIET})
set (CMAKE_REQUIRED_QUIET ${${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY})

# ----------------------------------------------------------------------------
# Greeter
# ----------------------------------------------------------------------------

string (ASCII 27 Esc)
set (ColourBold "${Esc}[1m")
set (ColourReset "${Esc}[m")

if (NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
    message (STATUS "${ColourBold}Finding SHARG and checking requirements:${ColourReset}")
endif ()

# ----------------------------------------------------------------------------
# Includes
# ----------------------------------------------------------------------------

include (CheckIncludeFileCXX)
include (CheckCXXSourceCompiles)
include (FindPackageHandleStandardArgs)

# ----------------------------------------------------------------------------
# Pretty printing and error handling
# ----------------------------------------------------------------------------

macro (sharg_config_print text)
    if (NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
        message (STATUS "  ${text}")
    endif ()
endmacro ()

macro (sharg_config_error text)
    if (${CMAKE_FIND_PACKAGE_NAME}_FIND_REQUIRED)
        message (FATAL_ERROR ${text})
    else ()
        if (NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
            message (WARNING ${text})
        endif ()
        return ()
    endif ()
endmacro ()

# ----------------------------------------------------------------------------
# CPM
# ----------------------------------------------------------------------------

# This will be true for git clones and source packages, but not for installed packages.
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
    set (SHARG_HAS_CPM TRUE)
else ()
    set (SHARG_HAS_CPM FALSE)
endif ()

if (SHARG_HAS_CPM)
    set (CPM_INDENT "  CMake Package Manager CPM: ")
    include ("${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
    CPMUsePackageLock ("${CMAKE_CURRENT_LIST_DIR}/package-lock.cmake")
endif ()

# ----------------------------------------------------------------------------
# Find SHARG include path
# ----------------------------------------------------------------------------

# Note that sharg-config.cmake can be standalone and thus SHARG_CLONE_DIR might be empty.
# * `SHARG_INCLUDE_DIR` was already found in sharg-config-version.cmake
if (SHARG_INCLUDE_DIR)
    sharg_config_print ("SHARG include dir found:   ${SHARG_INCLUDE_DIR}")
else ()
    sharg_config_error ("SHARG include directory could not be found (SHARG_INCLUDE_DIR: '${SHARG_INCLUDE_DIR}')")
endif ()

# ----------------------------------------------------------------------------
# Options for CheckCXXSourceCompiles
# ----------------------------------------------------------------------------

# deactivate messages in check_*
set (CMAKE_REQUIRED_QUIET 1)
# use global variables in Check* calls
set (CMAKE_REQUIRED_INCLUDES ${CMAKE_INCLUDE_PATH} ${SHARG_INCLUDE_DIR})
set (CMAKE_REQUIRED_FLAGS ${CMAKE_CXX_FLAGS})

# ----------------------------------------------------------------------------
# Force-deactivate optional dependencies
# ----------------------------------------------------------------------------

option (SHARG_NO_TDL "Do not use TDL, even if present." OFF)

# ----------------------------------------------------------------------------
# Check supported compilers
# ----------------------------------------------------------------------------

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 12)
    message (FATAL_ERROR "GCC < 12 is not supported. The detected compiler version is ${CMAKE_CXX_COMPILER_VERSION}.")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 17)
    message (FATAL_ERROR "Clang < 17 is not supported. The detected compiler version is ${CMAKE_CXX_COMPILER_VERSION}.")
endif ()

# ----------------------------------------------------------------------------
# thread support (pthread, windows threads)
# ----------------------------------------------------------------------------

set (THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package (Threads QUIET)

if (TARGET Threads::Threads)
    list (APPEND SHARG_LIBRARIES Threads::Threads)
    if ("${CMAKE_THREAD_LIBS_INIT}" STREQUAL "")
        sharg_config_print ("Thread support:             builtin.")
    else ()
        sharg_config_print ("Thread support:             via ${CMAKE_THREAD_LIBS_INIT}")
    endif ()
else ()
    sharg_config_print ("Thread support:             not found.")
endif ()

# ----------------------------------------------------------------------------
# tool description lib (tdl) dependency
# ----------------------------------------------------------------------------

if (NOT SHARG_NO_TDL)
    if (NOT SHARG_HAS_CPM)
        find_package (tdl QUIET)
    else ()
        CPMGetPackage (tdl)
    endif ()

    if (TARGET tdl::tdl)
        sharg_config_print ("Optional dependency:        TDL found.")
        list (APPEND SHARG_DEFINITIONS "-DSHARG_HAS_TDL=1")
        list (APPEND SHARG_LIBRARIES tdl::tdl)
    else ()
        sharg_config_print ("Optional dependency:        TDL not found.")
        list (APPEND SHARG_DEFINITIONS "-DSHARG_HAS_TDL=0")
    endif ()
else ()
    sharg_config_print ("Optional dependency:        TDL deactivated.")
    list (APPEND SHARG_DEFINITIONS "-DSHARG_HAS_TDL=0")
endif ()

# ----------------------------------------------------------------------------
# System dependencies
# ----------------------------------------------------------------------------

# librt
find_library (SHARG_RT_LIB rt)
if (SHARG_RT_LIB)
    list (APPEND SHARG_LIBRARIES ${SHARG_RT_LIB})
endif ()

# libexecinfo -- implicit
find_package (Backtrace QUIET)
if (TARGET Backtrace::Backtrace)
    list (APPEND SHARG_LIBRARIES Backtrace::Backtrace)
    sharg_config_print ("Optional dependency:        libexecinfo found.")
else ()
    sharg_config_print ("Optional dependency:        libexecinfo not found.")
endif ()

# ----------------------------------------------------------------------------
# Perform compilability test of platform.hpp (tests some requirements)
# ----------------------------------------------------------------------------

# cmake-format: off
# Note: With CMake >= 3.25, the file WRITE can be removed, the second and third line in try_compile can be replaced by
# SOURCE_FROM_CONTENT "platform_test.cpp" "#include <sharg/platform.hpp>\nint main() {}"
file (WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/platform_test.cpp"
            "#include <sharg/platform.hpp>\nint main() {}")

try_compile (SHARG_PLATFORM_TEST
             ${CMAKE_BINARY_DIR}
             ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/platform_test.cpp
             CMAKE_FLAGS "-DINCLUDE_DIRECTORIES:STRING=${SHARG_INCLUDE_DIR}"
             COMPILE_DEFINITIONS ${SHARG_DEFINITIONS}
             CXX_STANDARD 23
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF
             OUTPUT_VARIABLE SHARG_PLATFORM_TEST_OUTPUT)
# cmake-format: on

if (SHARG_PLATFORM_TEST)
    sharg_config_print ("SHARG platform.hpp build:   passed.")
else ()
    sharg_config_error ("SHARG platform.hpp build:   failed!\n\
                        ${SHARG_PLATFORM_TEST_OUTPUT}")
endif ()

# ----------------------------------------------------------------------------
# Finish find_package call
# ----------------------------------------------------------------------------

if (CMAKE_FIND_PACKAGE_NAME)
    find_package_handle_standard_args (${CMAKE_FIND_PACKAGE_NAME} REQUIRED_VARS SHARG_INCLUDE_DIR)

    # Set SHARG_* variables with the content of ${CMAKE_FIND_PACKAGE_NAME}_(FOUND|...|VERSION)
    # This needs to be done, because `find_package(SHARG)` might be called in any case-sensitive way and we want to
    # guarantee that SHARG_* are always set.
    foreach (package_var
             FOUND
             DIR
             ROOT
             CONFIG
             VERSION
             VERSION_MAJOR
             VERSION_MINOR
             VERSION_PATCH
             VERSION_TWEAK
             VERSION_COUNT)
        set (SHARG_${package_var} "${${CMAKE_FIND_PACKAGE_NAME}_${package_var}}")
    endforeach ()
else ()
    set (SHARG_VERSION "${PACKAGE_VERSION}")
endif ()

# ----------------------------------------------------------------------------
# Export targets
# ----------------------------------------------------------------------------

if (NOT TARGET sharg::sharg)
    add_library (sharg_sharg INTERFACE)
    target_compile_definitions (sharg_sharg INTERFACE ${SHARG_DEFINITIONS})
    target_compile_features (sharg_sharg INTERFACE cxx_std_23)
    target_link_libraries (sharg_sharg INTERFACE ${SHARG_LIBRARIES})
    target_include_directories (sharg_sharg INTERFACE "${SHARG_INCLUDE_DIR}")
    add_library (sharg::sharg ALIAS sharg_sharg)
endif ()

set (CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET_SAVE})

if (SHARG_FIND_DEBUG)
    message ("Result for ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
    message ("")
    message ("  CMAKE_BUILD_TYPE            ${CMAKE_BUILD_TYPE}")
    message ("  CMAKE_SOURCE_DIR            ${CMAKE_SOURCE_DIR}")
    message ("  CMAKE_INCLUDE_PATH          ${CMAKE_INCLUDE_PATH}")
    message ("  SHARG_INCLUDE_DIR          ${SHARG_INCLUDE_DIR}")
    message ("")
    message ("  ${CMAKE_FIND_PACKAGE_NAME}_FOUND                ${${CMAKE_FIND_PACKAGE_NAME}_FOUND}")
    message ("")
    message ("  SHARG_INCLUDE_DIR          ${SHARG_INCLUDE_DIR}")
    message ("  SHARG_LIBRARIES            ${SHARG_LIBRARIES}")
    message ("  SHARG_DEFINITIONS          ${SHARG_DEFINITIONS}")
    message ("")
    message ("  SHARG_VERSION              ${SHARG_VERSION}")
    message ("  SHARG_VERSION_MAJOR        ${SHARG_VERSION_MAJOR}")
    message ("  SHARG_VERSION_MINOR        ${SHARG_VERSION_MINOR}")
    message ("  SHARG_VERSION_PATCH        ${SHARG_VERSION_PATCH}")
endif ()
