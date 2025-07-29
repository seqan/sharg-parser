# SPDX-FileCopyrightText: 2006-2025, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2025, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

# This file describes where and which parts of Sharg should be installed to.

cmake_minimum_required (VERSION 3.14)

include (GNUInstallDirs)

# install documentation files in /share/doc
install (FILES "${SHARG_CLONE_DIR}/CHANGELOG.md" #
               "${SHARG_CLONE_DIR}/CODE_OF_CONDUCT.md" #
               "${SHARG_CLONE_DIR}/CONTRIBUTING.md" #
               "${SHARG_CLONE_DIR}/LICENSE.md" #
               "${SHARG_CLONE_DIR}/README.md"
         TYPE DOC)

# install cmake files in /share/cmake
install (FILES "${SHARG_CLONE_DIR}/cmake/sharg-config.cmake" "${SHARG_CLONE_DIR}/cmake/sharg-config-version.cmake"
         DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/sharg")

# install sharg header files in /include/sharg
install (DIRECTORY "${SHARG_INCLUDE_DIR}/sharg" TYPE INCLUDE)
