# SPDX-FileCopyrightText: 2006-2024, Knut Reinert & Freie Universität Berlin
# SPDX-FileCopyrightText: 2016-2024, Knut Reinert & MPI für molekulare Genetik
# SPDX-License-Identifier: BSD-3-Clause

message (STATUS "Configuring devel doc.")

set (SHARG_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
set (SHARG_DOXYGEN_EXCLUDE_SYMBOLS "")
set (SHARG_DOXYGEN_PREDEFINED_NDEBUG "")
set (SHARG_DOXYGEN_ENABLED_SECTIONS "DEV")
set (SHARG_SHOW_DEV_GROUP "YES")
set (SHARG_DOXYGEN_EXTRACT_PRIVATE "YES")

configure_file (${SHARG_DOXYFILE_IN} ${SHARG_DOXYGEN_OUTPUT_DIR}/Doxyfile)
configure_file (${SHARG_FOOTER_HTML_IN} ${SHARG_DOXYGEN_OUTPUT_DIR}/sharg_footer.html)
configure_file (${SHARG_LAYOUT_IN} ${SHARG_DOXYGEN_OUTPUT_DIR}/DoxygenLayout.xml)

add_custom_target (doc_dev ALL
                   COMMAND ${DOXYGEN_EXECUTABLE}
                   WORKING_DIRECTORY ${SHARG_DOXYGEN_OUTPUT_DIR}
                   BYPRODUCTS html/
                   DEPENDS download-cppreference-doxygen-web-tag
                   COMMENT "Generating developer API documentation with Doxygen"
                   VERBATIM)

# Install doc_dev documentation in ./install_doc_dev folder
# cmake --install . --prefix install_doc_dev --component doc_dev
install (DIRECTORY "${SHARG_DOXYGEN_OUTPUT_DIR}/html"
         DESTINATION "${CMAKE_INSTALL_DOCDIR}"
         COMPONENT doc-dev
         EXCLUDE_FROM_ALL
         PATTERN "*.md5" EXCLUDE
         PATTERN "*.map" EXCLUDE
         PATTERN "formula.repository" EXCLUDE)

### Enable testing

enable_testing ()

message (STATUS "Add dev doc test.")
add_test (NAME doc_dev_no_warnings_test
          COMMAND bash -c "${SHARG_TEST_DOXYGEN_FAIL_ON_WARNINGS}"
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_test (NAME doc_dev_no_uncond_requires_test
          COMMAND bash -c "${SHARG_TEST_DOXYGEN_FAIL_ON_UNCOND_REQUIRES}"
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
