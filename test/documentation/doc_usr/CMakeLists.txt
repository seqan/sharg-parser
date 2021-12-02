# -----------------------------------------------------------------------------------------------------------
# Copyright (c) 2006-2021, Knut Reinert & Freie Universität Berlin
# Copyright (c) 2016-2021, Knut Reinert & MPI für molekulare Genetik
# This file may be used, modified and/or redistributed under the terms of the 3-clause BSD-License
# shipped with this file and also available at: https://github.com/seqan/sharg-parser/blob/master/LICENSE.md
# -----------------------------------------------------------------------------------------------------------

message (STATUS "Configuring user doc.")

set (SHARG_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set (SHARG_DOXYGEN_EXCLUDE_SYMBOLS "detail sharg::simd") #/""
set (SHARG_DOXYGEN_PREDEFINED_NDEBUG "-NDEBUG") #/""
set (SHARG_DOXYGEN_ENABLED_SECTIONS "") #/"DEV"
set (SHARG_DOXYGEN_EXTRACT_PRIVATE "NO") #/"YES":

configure_file (${SHARG_DOXYFILE_IN} ${SHARG_DOXYGEN_OUTPUT_DIR}/Doxyfile)
configure_file (${SHARG_FOOTER_HTML_IN} ${SHARG_DOXYGEN_OUTPUT_DIR}/sharg_footer.html)

add_custom_target(doc_usr ALL
                  COMMAND ${DOXYGEN_EXECUTABLE}
                  WORKING_DIRECTORY ${SHARG_DOXYGEN_OUTPUT_DIR}
                  BYPRODUCTS html/
                  DEPENDS download-cppreference-doxygen-web-tag
                  COMMENT "Generating user API documentation with Doxygen"
                  VERBATIM)

# Install doc_usr documentation in ./export folder
# make DESTDIR=export install
install (
    DIRECTORY "${SHARG_DOXYGEN_OUTPUT_DIR}/html"
    DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    COMPONENT doc
    PATTERN "*.md5" EXCLUDE
    PATTERN "*.map" EXCLUDE
    PATTERN "formula.repository" EXCLUDE
)

### Enable testing

enable_testing()

message (STATUS "Add user doc test.")
add_test(NAME doc_usr_no_warnings_test
         COMMAND bash -c "${SHARG_TEST_DOXYGEN_FAIL_ON_WARNINGS}")

add_test(NAME doc_usr_no_uncond_requires_test
         COMMAND bash -c "${SHARG_TEST_DOXYGEN_FAIL_ON_UNCOND_REQUIRES}")
