# --------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2022, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2022, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/main/LICENSE.md
# --------------------------------------------------------------------------------------------------------

cmake_minimum_required (VERSION 3.10)

set (CPACK_GENERATOR "TXZ")

set (CPACK_COMPONENTS_ALL doc)

set (CPACK_ARCHIVE_COMPONENT_INSTALL ON)

set (CPACK_PACKAGE_VENDOR "sharg")
# A description of the project, used in places such as the introduction screen of CPack-generated Windows installers.
# set (CPACK_PACKAGE_DESCRIPTION_FILE "") # TODO
set (CPACK_PACKAGE_CHECKSUM "SHA256")

include (CPack)
