# SPDX-FileCopyrightText: 2006-2026, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2026, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: CC0-1.0

# CPM Package Lock
# This file should be committed to version control

# URL/GIT_TAG may be annotated with a branch name
# This is needed for https://github.com/seqan/actions/tree/main/update_cpm_package_lock

# The first argument of CPMDeclarePackage can be freely chosen and is used as argument in CPMGetPackage.
# The NAME argument should be package name that would also be used in a find_package call.
# Ideally, both are the same, which might not always be possible: https://github.com/cpm-cmake/CPM.cmake/issues/603
# This is needed to support CPM_USE_LOCAL_PACKAGES

# TDL
set (SHARG_TDL_VERSION 1.0.1)
CPMDeclarePackage (tdl
                   NAME tdl
                   VERSION ${SHARG_TDL_VERSION}
                   GITHUB_REPOSITORY deNBI-cibi/tool_description_lib
                   SYSTEM TRUE
                   OPTIONS "INSTALL_TDL OFF" "CMAKE_MESSAGE_LOG_LEVEL WARNING")
# googletest
set (SHARG_GOOGLETEST_VERSION 1.17.0)
CPMDeclarePackage (googletest
                   NAME GTest
                   VERSION ${SHARG_GOOGLETEST_VERSION}
                   GITHUB_REPOSITORY google/googletest
                   SYSTEM TRUE
                   OPTIONS "BUILD_GMOCK OFF" "INSTALL_GTEST OFF" "CMAKE_MESSAGE_LOG_LEVEL WARNING")
# doxygen-awesome
set (SHARG_DOXYGEN_AWESOME_VERSION 2.4.1)
CPMDeclarePackage (doxygen_awesome
                   NAME doxygen_awesome
                   VERSION ${SHARG_DOXYGEN_AWESOME_VERSION}
                   GITHUB_REPOSITORY jothepro/doxygen-awesome-css
                   DOWNLOAD_ONLY TRUE
                   QUIET YES)
# use_ccache
set (SHARG_USE_CCACHE_VERSION d2a54ef555b6fc2d496a4c9506dbeb7cf899ce37)
CPMDeclarePackage (use_ccache
                   NAME use_ccache
                   GIT_TAG ${SHARG_USE_CCACHE_VERSION} # main
                   GITHUB_REPOSITORY seqan/cmake-scripts
                   SOURCE_SUBDIR ccache
                   SYSTEM TRUE
                   EXCLUDE_FROM_ALL TRUE)
