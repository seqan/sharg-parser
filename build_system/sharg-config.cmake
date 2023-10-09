# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2023, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2023, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------
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
#   SHARG_CXX_FLAGS        -- to be added to CMAKE_CXX_FLAGS
#
# Additionally, the following [IMPORTED][IMPORTED] targets are defined:
#
#   sharg::sharg          -- interface target where
#                                  target_link_libraries(target sharg::sharg)
#                              automatically sets
#                                  target_include_directories(target $SHARG_INCLUDE_DIRS),
#                                  target_link_libraries(target $SHARG_LIBRARIES),
#                                  target_compile_definitions(target $SHARG_DEFINITIONS) and
#                                  target_compile_options(target $SHARG_CXX_FLAGS)
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
# Find SHARG include path
# ----------------------------------------------------------------------------

# Note that sharg-config.cmake can be standalone and thus SHARG_CLONE_DIR might be empty.
# * `SHARG_CLONE_DIR` was already found in sharg-config-version.cmake
# * `SHARG_INCLUDE_DIR` was already found in sharg-config-version.cmake
find_path (SHARG_SUBMODULES_DIR
           NAMES submodules/tool_description_lib
           HINTS "${SHARG_CLONE_DIR}" "${SHARG_INCLUDE_DIR}/sharg")

if (SHARG_INCLUDE_DIR)
    sharg_config_print ("SHARG include dir found:   ${SHARG_INCLUDE_DIR}")
else ()
    sharg_config_error ("SHARG include directory could not be found (SHARG_INCLUDE_DIR: '${SHARG_INCLUDE_DIR}')")
endif ()

# ----------------------------------------------------------------------------
# Detect if we are a clone of repository and if yes auto-add submodules
# ----------------------------------------------------------------------------

if (SHARG_CLONE_DIR)
    sharg_config_print ("Detected as running from a repository checkout…")
endif ()

# Currently unused.
# if (SHARG_SUBMODULES_DIR)
#     file (GLOB submodules ${SHARG_SUBMODULES_DIR}/lib/*/include)
#     foreach (submodule ${submodules})
#         if (IS_DIRECTORY ${submodule})
#             sharg_config_print ("  …adding submodule include:  ${submodule}")
#             set (SHARG_DEPENDENCY_INCLUDE_DIRS ${submodule} ${SHARG_DEPENDENCY_INCLUDE_DIRS})
#         endif ()
#     endforeach ()
# endif ()

# ----------------------------------------------------------------------------
# Options for CheckCXXSourceCompiles
# ----------------------------------------------------------------------------

# deactivate messages in check_*
set (CMAKE_REQUIRED_QUIET 1)
# use global variables in Check* calls
set (CMAKE_REQUIRED_INCLUDES ${CMAKE_INCLUDE_PATH} ${SHARG_INCLUDE_DIR} ${SHARG_DEPENDENCY_INCLUDE_DIRS})
set (CMAKE_REQUIRED_FLAGS ${CMAKE_CXX_FLAGS})

# ----------------------------------------------------------------------------
# Require C++20
# ----------------------------------------------------------------------------

set (CMAKE_REQUIRED_FLAGS_SAVE ${CMAKE_REQUIRED_FLAGS})

set (CXXSTD_TEST_SOURCE
     "#if !defined (__cplusplus) || (__cplusplus < 202002)
      #error NOCXX20
      #endif
      int main() {}")

check_cxx_source_compiles ("${CXXSTD_TEST_SOURCE}" CXX20_BUILTIN)

if (CXX20_BUILTIN)
    sharg_config_print ("C++ Standard-20 support:    builtin")
else ()
    set (CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS_SAVE} -std=c++20")

    check_cxx_source_compiles ("${CXXSTD_TEST_SOURCE}" CXX20_FLAG)

    if (CXX20_FLAG)
        sharg_config_print ("C++ Standard-20 support:    via -std=c++20")
    else ()
        sharg_config_error ("SHARG requires C++20, but your compiler does not support it.")
    endif ()

    set (SHARG_CXX_FLAGS "${SHARG_CXX_FLAGS} -std=c++20")
endif ()

# ----------------------------------------------------------------------------
# thread support (pthread, windows threads)
# ----------------------------------------------------------------------------

set (THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package (Threads QUIET)

if (Threads_FOUND)
    set (SHARG_LIBRARIES ${SHARG_LIBRARIES} Threads::Threads)
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
find_package (TDL QUIET HINTS ${SHARG_SUBMODULES_DIR}/submodules/tool_description_lib ${SHARG_HINT_TDL})

if (TDL_FOUND)
    sharg_config_print ("Dependency:                 TDL found.")
else ()
    sharg_config_error ("Dependency:                 TDL not found.")
endif ()

# ----------------------------------------------------------------------------
# System dependencies
# ----------------------------------------------------------------------------

# librt
if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    OR (${CMAKE_SYSTEM_NAME} STREQUAL "kFreeBSD")
    OR (${CMAKE_SYSTEM_NAME} STREQUAL "GNU"))
    set (SHARG_LIBRARIES ${SHARG_LIBRARIES} rt)
endif ()

# libexecinfo -- implicit
check_include_file_cxx (execinfo.h _SHARG_HAVE_EXECINFO)
mark_as_advanced (_SHARG_HAVE_EXECINFO)
if (_SHARG_HAVE_EXECINFO)
    sharg_config_print ("Optional dependency:        libexecinfo found.")
    if ((${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD") OR (${CMAKE_SYSTEM_NAME} STREQUAL "OpenBSD"))
        set (SHARG_LIBRARIES ${SHARG_LIBRARIES} execinfo elf)
    endif ()
else ()
    sharg_config_print ("Optional dependency:        libexecinfo not found.")
endif ()

# ----------------------------------------------------------------------------
# Perform compilability test of platform.hpp (tests some requirements)
# ----------------------------------------------------------------------------

set (CXXSTD_TEST_SOURCE "#include <sharg/platform.hpp>
     int main() {}")

# using try_compile instead of check_cxx_source_compiles to capture output in case of failure
file (WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx" "${CXXSTD_TEST_SOURCE}\n")

try_compile (SHARG_PLATFORM_TEST ${CMAKE_BINARY_DIR}
             ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx
             CMAKE_FLAGS "-DCOMPILE_DEFINITIONS:STRING=${CMAKE_CXX_FLAGS} ${SHARG_CXX_FLAGS}"
                         "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_INCLUDE_PATH};${SHARG_INCLUDE_DIR};${SHARG_DEPENDENCY_INCLUDE_DIRS}"
             COMPILE_DEFINITIONS ${SHARG_DEFINITIONS}
             LINK_LIBRARIES ${SHARG_LIBRARIES}
             OUTPUT_VARIABLE SHARG_PLATFORM_TEST_OUTPUT)

if (SHARG_PLATFORM_TEST)
    sharg_config_print ("SHARG platform.hpp build:   passed.")
else ()
    sharg_config_error ("SHARG platform.hpp build:   failed!\n\
                        ${SHARG_PLATFORM_TEST_OUTPUT}")
endif ()

# ----------------------------------------------------------------------------
# Finish find_package call
# ----------------------------------------------------------------------------

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

# propagate SHARG_INCLUDE_DIR into SHARG_INCLUDE_DIRS
set (SHARG_INCLUDE_DIRS ${SHARG_INCLUDE_DIR} ${SHARG_DEPENDENCY_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# Export targets
# ----------------------------------------------------------------------------

if (SHARG_FOUND AND NOT TARGET sharg::sharg)
    separate_arguments (SHARG_CXX_FLAGS_LIST UNIX_COMMAND "${SHARG_CXX_FLAGS}")

    add_library (sharg_sharg INTERFACE)
    target_compile_definitions (sharg_sharg INTERFACE ${SHARG_DEFINITIONS})
    target_compile_options (sharg_sharg INTERFACE ${SHARG_CXX_FLAGS_LIST})

    # Include TDL as system header to suppress warnings.
    get_target_property (tdl_include_dir tdl::tdl INTERFACE_INCLUDE_DIRECTORIES)
    target_include_directories (sharg_sharg SYSTEM INTERFACE ${tdl_include_dir})

    target_link_libraries (sharg_sharg INTERFACE "${SHARG_LIBRARIES}" tdl::tdl)
    # include sharg/include/ as -I, because sharg should never produce warnings.
    target_include_directories (sharg_sharg INTERFACE "${SHARG_INCLUDE_DIR}")
    # include everything except sharg/include/ as -isystem, i.e.
    # a system header which suppresses warnings of external libraries.
    target_include_directories (sharg_sharg SYSTEM INTERFACE "${SHARG_DEPENDENCY_INCLUDE_DIRS}")
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
    message ("  SHARG_INCLUDE_DIRS         ${SHARG_INCLUDE_DIRS}")
    message ("  SHARG_LIBRARIES            ${SHARG_LIBRARIES}")
    message ("  SHARG_DEFINITIONS          ${SHARG_DEFINITIONS}")
    message ("  SHARG_CXX_FLAGS            ${SHARG_CXX_FLAGS}")
    message ("")
    message ("  SHARG_VERSION              ${SHARG_VERSION}")
    message ("  SHARG_VERSION_MAJOR        ${SHARG_VERSION_MAJOR}")
    message ("  SHARG_VERSION_MINOR        ${SHARG_VERSION_MINOR}")
    message ("  SHARG_VERSION_PATCH        ${SHARG_VERSION_PATCH}")
endif ()
