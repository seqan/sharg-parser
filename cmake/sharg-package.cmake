# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

# This file describes how Sharg will be packaged.

cmake_minimum_required (VERSION 3.7)

set (CPACK_GENERATOR "TXZ")

set (CPACK_PACKAGE_VERSION "${SHARG_VERSION}")
set (CPACK_PACKAGE_VENDOR "seqan")
# A description of the project, used in places such as the introduction screen of CPack-generated Windows installers.
# set (CPACK_PACKAGE_DESCRIPTION_FILE "") # TODO
set (CPACK_PACKAGE_CHECKSUM "SHA256")
set (CPACK_PACKAGE_ICON "${SHARG_CLONE_DIR}/test/documentation/sharg_logo.png")
set (CPACK_RESOURCE_FILE_LICENSE "${SHARG_CLONE_DIR}/LICENSE.md")
set (CPACK_RESOURCE_FILE_README "${SHARG_CLONE_DIR}/README.md")

# Source Package
set (CPACK_SOURCE_GENERATOR "TXZ")
set (CPACK_SOURCE_IGNORE_FILES "\\\\.git($|/)")

include (CPack)
