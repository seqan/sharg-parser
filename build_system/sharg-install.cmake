# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

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
install (FILES "${SHARG_CLONE_DIR}/build_system/sharg-config.cmake"
               "${SHARG_CLONE_DIR}/build_system/sharg-config-version.cmake"
         DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/sharg")

# install sharg header files in /include/sharg
install (DIRECTORY "${SHARG_INCLUDE_DIR}/sharg" TYPE INCLUDE)

