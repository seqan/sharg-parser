# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

# list all search places
# NOTE: this can be enabled globally by -DSHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE=1 to help debug search paths
# set (CMAKE_FIND_DEBUG_MODE ${SHARG_EXTERNAL_PROJECT_FIND_DEBUG_MODE})

macro (sharg_print_diagnostics text)
    message (STATUS "  ${text}")
endmacro ()

message (STATUS "=== SHARG find_package Diagnostics start ===")
sharg_print_diagnostics ("CMAKE_COMMAND: ${CMAKE_COMMAND}")
sharg_print_diagnostics ("CMAKE_VERSION: ${CMAKE_VERSION}")
sharg_print_diagnostics ("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")

sharg_print_diagnostics (
    "Search paths (https://cmake.org/cmake/help/latest/command/find_package.html#config-mode-search-procedure)")
sharg_print_diagnostics ("1) CMAKE_FIND_USE_PACKAGE_ROOT_PATH: ${CMAKE_FIND_USE_CMAKE_PATH}")
sharg_print_diagnostics ("   SHARG_ROOT: ${SHARG_ROOT}")

sharg_print_diagnostics ("2) CMAKE_FIND_USE_CMAKE_PATH: ${CMAKE_FIND_USE_CMAKE_PATH}")
sharg_print_diagnostics ("   CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
sharg_print_diagnostics ("   CMAKE_FRAMEWORK_PATH: ${CMAKE_FRAMEWORK_PATH}")
sharg_print_diagnostics ("   CMAKE_APPBUNDLE_PATH: ${CMAKE_APPBUNDLE_PATH}")

sharg_print_diagnostics ("3) CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH: ${CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH}")
sharg_print_diagnostics ("   SHARG_DIR: ${SHARG_DIR}")

# 4) Using the HINTS option.
# See point 4 in https://cmake.org/cmake/help/latest/command/find_package.html#config-mode-search-procedure.
# There is no output.

sharg_print_diagnostics ("5) CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH: ${CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH}")

sharg_print_diagnostics ("6) CMAKE_FIND_USE_PACKAGE_REGISTRY: ${CMAKE_FIND_USE_PACKAGE_REGISTRY}")

sharg_print_diagnostics ("7) CMAKE_FIND_USE_CMAKE_SYSTEM_PATH: ${CMAKE_FIND_USE_CMAKE_SYSTEM_PATH}")
sharg_print_diagnostics ("   CMAKE_SYSTEM_PREFIX_PATH: ${CMAKE_SYSTEM_PREFIX_PATH}")
sharg_print_diagnostics ("   CMAKE_SYSTEM_FRAMEWORK_PATH: ${CMAKE_SYSTEM_FRAMEWORK_PATH}")
sharg_print_diagnostics ("   CMAKE_SYSTEM_APPBUNDLE_PATH: ${CMAKE_SYSTEM_APPBUNDLE_PATH}")

sharg_print_diagnostics ("8) CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY: ${CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY}")
message (STATUS "=== SHARG find_package Diagnostics end ===")
