# -----------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sarg/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------
#
# This CMake module will try to find SARG and its dependencies.  You can use
# it the same way you would use any other CMake module.
#
#   find_package (SARG [REQUIRED] ...)
#
# Since this makes a difference for CMAKE, pay attention to the case
# ("SARG", "SARG" and "sarg" are all valid, but other names not).
#
# SARG has the following platform requirements:
#
#   C++20
#   pthread
#
# SARG requires the following libraries:
#
#   SeqAn3    -- the succinct data structure library
#
# SARG has the following optional dependencies:
#
#   ZLIB      -- zlib compression library
#   BZip2     -- libbz2 compression library
#
# If you don't wish for these to be detected (and used), you may define SARG_NO_ZLIB,
# SARG_NO_BZIP2, SARG_NO_CEREAL and SARG_NO_LEMON respectively.
#
# If you wish to require the presence of ZLIB or BZip2, just check for the module before
# finding SARG, e.g. "find_package (ZLIB REQUIRED)".
# If you wish to require the presence of CEREAL, you may define SARG_CEREAL.
# If you wish to require the presence of LEMON, you may define SARG_LEMON.
#
# Once the search has been performed, the following variables will be set.
#
#   SARG_FOUND            -- Indicate whether SARG was found and requirements met.
#
#   SARG_VERSION          -- The version as string, e.g. "3.0.0"
#   SARG_VERSION_MAJOR    -- e.g. 3
#   SARG_VERSION_MINOR    -- e.g. 0
#   SARG_VERSION_PATCH    -- e.g. 0
#
#   SARG_INCLUDE_DIRS     -- to be passed to include_directories ()
#   SARG_LIBRARIES        -- to be passed to target_link_libraries ()
#   SARG_DEFINITIONS      -- to be passed to add_definitions ()
#   SARG_CXX_FLAGS        -- to be added to CMAKE_CXX_FLAGS
#
# Additionally, the following [IMPORTED][IMPORTED] targets are defined:
#
#   sarg::sarg          -- interface target where
#                                  target_link_libraries(target sarg::sarg)
#                              automatically sets
#                                  target_include_directories(target $SARG_INCLUDE_DIRS),
#                                  target_link_libraries(target $SARG_LIBRARIES),
#                                  target_compile_definitions(target $SARG_DEFINITIONS) and
#                                  target_compile_options(target $SARG_CXX_FLAGS)
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
set(CMAKE_REQUIRED_QUIET_SAVE ${CMAKE_REQUIRED_QUIET})
set(CMAKE_REQUIRED_QUIET ${${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY})

# ----------------------------------------------------------------------------
# Greeter
# ----------------------------------------------------------------------------

string (ASCII 27 Esc)
set (ColourBold "${Esc}[1m")
set (ColourReset "${Esc}[m")

if (NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
    message (STATUS "${ColourBold}Finding SARG and checking requirements:${ColourReset}")
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

macro (sarg_config_print text)
    if (NOT ${CMAKE_FIND_PACKAGE_NAME}_FIND_QUIETLY)
        message (STATUS "  ${text}")
    endif ()
endmacro ()

macro (sarg_config_error text)
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
# Find SARG include path
# ----------------------------------------------------------------------------

# Note that sarg-config.cmake can be standalone and thus SARG_CLONE_DIR might be empty.
# * `SARG_CLONE_DIR` was already found in sarg-config-version.cmake
# * `SARG_INCLUDE_DIR` was already found in sarg-config-version.cmake
find_path (SARG_SUBMODULES_DIR NAMES lib/seqan3 HINTS "${SARG_CLONE_DIR}" "${SARG_INCLUDE_DIR}/sarg")

if (SARG_INCLUDE_DIR)
    sarg_config_print ("SARG include dir found:   ${SARG_INCLUDE_DIR}")
else ()
    sarg_config_error ("SARG include directory could not be found (SARG_INCLUDE_DIR: '${SARG_INCLUDE_DIR}')")
endif ()

# ----------------------------------------------------------------------------
# Detect if we are a clone of repository and if yes auto-add submodules
# ----------------------------------------------------------------------------

if (SARG_CLONE_DIR)
    sarg_config_print ("Detected as running from a repository checkout…")
endif ()

if (SARG_SUBMODULES_DIR)
    file (GLOB submodules ${SARG_SUBMODULES_DIR}/lib/*/include)
    foreach (submodule ${submodules})
        if (IS_DIRECTORY ${submodule})
            sarg_config_print ("  …adding submodule include:  ${submodule}")
            set (SARG_DEPENDENCY_INCLUDE_DIRS ${submodule} ${SARG_DEPENDENCY_INCLUDE_DIRS})
        endif ()
    endforeach ()
endif ()

# ----------------------------------------------------------------------------
# Options for CheckCXXSourceCompiles
# ----------------------------------------------------------------------------

# deactivate messages in check_*
set (CMAKE_REQUIRED_QUIET       1)
# use global variables in Check* calls
set (CMAKE_REQUIRED_INCLUDES    ${CMAKE_INCLUDE_PATH} ${SARG_INCLUDE_DIR} ${SARG_DEPENDENCY_INCLUDE_DIRS})
set (CMAKE_REQUIRED_FLAGS       ${CMAKE_CXX_FLAGS})

# ----------------------------------------------------------------------------
# Force-deactivate optional dependencies
# ----------------------------------------------------------------------------

# These two are "opt-in", because detected by CMake
# If you want to force-require these, just do find_package (zlib REQUIRED) before find_package (sarg)
option (SARG_NO_ZLIB  "Don't use ZLIB, even if present." OFF)
option (SARG_NO_BZIP2 "Don't use BZip2, even if present." OFF)

# ----------------------------------------------------------------------------
# Require C++20
# ----------------------------------------------------------------------------

set (CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})

set (CXXSTD_TEST_SOURCE
    "#if !defined (__cplusplus) || (__cplusplus < 201709L)
    #error NOCXX20
    #endif
    int main() {}")

check_cxx_source_compiles ("${CXXSTD_TEST_SOURCE}" CXX20_BUILTIN)

if (CXX20_BUILTIN)
    sarg_config_print ("C++ Standard-20 support:    builtin")
else ()
    set (CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -std=c++20")

    check_cxx_source_compiles ("${CXXSTD_TEST_SOURCE}" CXX20_FLAG)

    if (CXX20_FLAG)
        sarg_config_print ("C++ Standard-20 support:    via -std=c++20")
    else ()
        sarg_config_error ("SARG requires C++20, but your compiler does not support it.")
    endif ()

    set (SARG_CXX_FLAGS "${SARG_CXX_FLAGS} -std=c++20")
endif ()

# ----------------------------------------------------------------------------
# Require C++ Concepts
# ----------------------------------------------------------------------------

#set (CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})

#set (CXXSTD_TEST_SOURCE
    #"static_assert (__cpp_concepts >= 201507);
    #int main() {}")

#set (CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} ${SARG_CXX_FLAGS}")
#check_cxx_source_compiles ("${CXXSTD_TEST_SOURCE}" SARG_CONCEPTS)

#if (SARG_CONCEPTS_FLAG)
    #sarg_config_print ("C++ Concepts support:       builtin")
#else ()
    #sarg_config_error ("SARG requires C++ Concepts, but your compiler does not support them.")
#endif ()

# ----------------------------------------------------------------------------
# thread support (pthread, windows threads)
# ----------------------------------------------------------------------------

set (THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package (Threads QUIET)

if (Threads_FOUND)
    set (SARG_LIBRARIES ${SARG_LIBRARIES} Threads::Threads)
    if ("${CMAKE_THREAD_LIBS_INIT}" STREQUAL "")
        sarg_config_print ("Thread support:             builtin.")
    else ()
        sarg_config_print ("Thread support:             via ${CMAKE_THREAD_LIBS_INIT}")
    endif ()
else ()
    sarg_config_print ("Thread support:             not found.")
endif ()

# ----------------------------------------------------------------------------
# Require SeqAn3
# ----------------------------------------------------------------------------

find_package (SeqAn3 REQUIRED QUIET
              HINTS ${CMAKE_CURRENT_LIST_DIR}/../lib/seqan3/build_system)

if (SEQAN3_FOUND)
    sarg_config_print ("Required dependency:        SeqAn3 found.")
else ()
    sarg_config_print ("The SeqAn3 library is required, but wasn't found. Get it from https://github.com/seqan/seqan3")
endif ()

# ----------------------------------------------------------------------------
# ZLIB dependency
# ----------------------------------------------------------------------------

if (NOT SARG_NO_ZLIB)
    find_package (ZLIB QUIET)
endif ()

if (ZLIB_FOUND)
    set (SARG_LIBRARIES         ${SARG_LIBRARIES}         ${ZLIB_LIBRARIES})
    set (SARG_DEPENDENCY_INCLUDE_DIRS      ${SARG_DEPENDENCY_INCLUDE_DIRS}      ${ZLIB_INCLUDE_DIRS})
    set (SARG_DEFINITIONS       ${SARG_DEFINITIONS}       "-DSARG_HAS_ZLIB=1")
    sarg_config_print ("Optional dependency:        ZLIB-${ZLIB_VERSION_STRING} found.")
else ()
    sarg_config_print ("Optional dependency:        ZLIB not found.")
endif ()

# ----------------------------------------------------------------------------
# BZip2 dependency
# ----------------------------------------------------------------------------

if (NOT SARG_NO_BZIP2)
    find_package (BZip2 QUIET)
endif ()

if (NOT ZLIB_FOUND AND BZIP2_FOUND)
    # NOTE (marehr): iostream_bzip2 uses the type `uInt`, which is defined by
    # `zlib`. Therefore, `bzip2` will cause a ton of errors without `zlib`.
    message (AUTHOR_WARNING "Disabling BZip2 [which was successfully found], "
                            "because ZLIB was not found. BZip2 depends on ZLIB.")
    unset (BZIP2_FOUND)
endif ()

if (BZIP2_FOUND)
    set (SARG_LIBRARIES         ${SARG_LIBRARIES}         ${BZIP2_LIBRARIES})
    set (SARG_DEPENDENCY_INCLUDE_DIRS      ${SARG_DEPENDENCY_INCLUDE_DIRS}      ${BZIP2_INCLUDE_DIRS})
    set (SARG_DEFINITIONS       ${SARG_DEFINITIONS}       "-DSARG_HAS_BZIP2=1")
    sarg_config_print ("Optional dependency:        BZip2-${BZIP2_VERSION_STRING} found.")
else ()
    sarg_config_print ("Optional dependency:        BZip2 not found.")
endif ()

# ----------------------------------------------------------------------------
# System dependencies
# ----------------------------------------------------------------------------

# librt
if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux") OR
    (${CMAKE_SYSTEM_NAME} STREQUAL "kFreeBSD") OR
    (${CMAKE_SYSTEM_NAME} STREQUAL "GNU"))
    set (SARG_LIBRARIES ${SARG_LIBRARIES} rt)
endif ()

# libexecinfo -- implicit
check_include_file_cxx (execinfo.h _SARG_HAVE_EXECINFO)
mark_as_advanced (_SARG_HAVE_EXECINFO)
if (_SARG_HAVE_EXECINFO)
    sarg_config_print ("Optional dependency:        libexecinfo found.")
    if ((${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD") OR (${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD"))
        set (SARG_LIBRARIES ${SARG_LIBRARIES} execinfo elf)
    endif ()
else ()
    sarg_config_print ("Optional dependency:        libexecinfo not found.")
endif ()

# ----------------------------------------------------------------------------
# Perform compilability test of platform.hpp (tests some requirements)
# ----------------------------------------------------------------------------

set (CXXSTD_TEST_SOURCE
     "#include <sarg/platform.hpp>
     int main() {}")

# using try_compile instead of check_cxx_source_compiles to capture output in case of failure
file (WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx" "${CXXSTD_TEST_SOURCE}\n")

try_compile (SARG_PLATFORM_TEST
             ${CMAKE_BINARY_DIR}
             ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx
             CMAKE_FLAGS         "-DCOMPILE_DEFINITIONS:STRING=${CMAKE_CXX_FLAGS} ${SARG_CXX_FLAGS}"
                                 "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_INCLUDE_PATH};${SARG_INCLUDE_DIR};${SARG_DEPENDENCY_INCLUDE_DIRS}"
             COMPILE_DEFINITIONS ${SARG_DEFINITIONS}
             LINK_LIBRARIES      ${SARG_LIBRARIES}
             OUTPUT_VARIABLE     SARG_PLATFORM_TEST_OUTPUT)

if (SARG_PLATFORM_TEST)
    sarg_config_print ("SARG platform.hpp build:  passed.")
else ()
    sarg_config_error ("SARG platform.hpp build:  failed!\n\
                        ${SARG_PLATFORM_TEST_OUTPUT}")
endif ()

# ----------------------------------------------------------------------------
# Finish find_package call
# ----------------------------------------------------------------------------

find_package_handle_standard_args (${CMAKE_FIND_PACKAGE_NAME} REQUIRED_VARS SARG_INCLUDE_DIR)

# Set SARG_* variables with the content of ${CMAKE_FIND_PACKAGE_NAME}_(FOUND|...|VERSION)
# This needs to be done, because `find_package(SARG)` might be called in any case-sensitive way and we want to
# guarantee that SARG_* are always set.
foreach (package_var FOUND DIR ROOT CONFIG VERSION VERSION_MAJOR VERSION_MINOR VERSION_PATCH VERSION_TWEAK VERSION_COUNT)
    set (SARG_${package_var} "${${CMAKE_FIND_PACKAGE_NAME}_${package_var}}")
endforeach ()

# propagate SARG_INCLUDE_DIR into SARG_INCLUDE_DIRS
set (SARG_INCLUDE_DIRS ${SARG_INCLUDE_DIR} ${SARG_DEPENDENCY_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# Export targets
# ----------------------------------------------------------------------------

if (SARG_FOUND AND NOT TARGET sarg::sarg)
    separate_arguments (SARG_CXX_FLAGS_LIST UNIX_COMMAND "${SARG_CXX_FLAGS}")

    add_library (sarg_sarg INTERFACE)
    target_compile_definitions (sarg_sarg INTERFACE ${SARG_DEFINITIONS})
    target_compile_options (sarg_sarg INTERFACE ${SARG_CXX_FLAGS_LIST})
    target_link_libraries (sarg_sarg INTERFACE "${SARG_LIBRARIES}")
    # include sarg/include/ as -I, because sarg should never produce warnings.
    target_include_directories (sarg_sarg INTERFACE "${SARG_INCLUDE_DIR}")
    # include everything except sarg/include/ as -isystem, i.e.
    # a system header which suppresses warnings of external libraries.
    target_include_directories (sarg_sarg SYSTEM INTERFACE "${SARG_DEPENDENCY_INCLUDE_DIRS}")
    add_library (sarg::sarg ALIAS sarg_sarg)
endif ()

set (CMAKE_REQUIRED_QUIET ${CMAKE_REQUIRED_QUIET_SAVE})

if (SARG_FIND_DEBUG)
  message ("Result for ${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt")
  message ("")
  message ("  CMAKE_BUILD_TYPE            ${CMAKE_BUILD_TYPE}")
  message ("  CMAKE_SOURCE_DIR            ${CMAKE_SOURCE_DIR}")
  message ("  CMAKE_INCLUDE_PATH          ${CMAKE_INCLUDE_PATH}")
  message ("  SARG_INCLUDE_DIR          ${SARG_INCLUDE_DIR}")
  message ("")
  message ("  ${CMAKE_FIND_PACKAGE_NAME}_FOUND                ${${CMAKE_FIND_PACKAGE_NAME}_FOUND}")
  message ("  SARG_HAS_ZLIB             ${ZLIB_FOUND}")
  message ("  SARG_HAS_BZIP2            ${BZIP2_FOUND}")
  message ("")
  message ("  SARG_INCLUDE_DIRS         ${SARG_INCLUDE_DIRS}")
  message ("  SARG_LIBRARIES            ${SARG_LIBRARIES}")
  message ("  SARG_DEFINITIONS          ${SARG_DEFINITIONS}")
  message ("  SARG_CXX_FLAGS            ${SARG_CXX_FLAGS}")
  message ("")
  message ("  SARG_VERSION              ${SARG_VERSION}")
  message ("  SARG_VERSION_MAJOR        ${SARG_VERSION_MAJOR}")
  message ("  SARG_VERSION_MINOR        ${SARG_VERSION_MINOR}")
  message ("  SARG_VERSION_PATCH        ${SARG_VERSION_PATCH}")
endif ()
