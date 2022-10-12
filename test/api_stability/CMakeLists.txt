cmake_minimum_required (VERSION 3.8)

project (api_stability)

find_package (Git)

include (../sharg-test.cmake)

include (ExternalProject)
set (SHARG_LAST_STABLE_VERSION "1.0.0")
set (SHARG_LAST_STABLE_SOURCE_URL
     "https://github.com/seqan/sharg-parser/releases/download/${SHARG_LAST_STABLE_VERSION}/sharg-${SHARG_LAST_STABLE_VERSION}-Source.tar.xz"
)
set (SHARG_LAST_STABLE_SOURCE_SHA256 "babd34fc0851ac6710346b67eb46ca067576fc319e0be5c478437733a69b66a0")

set (SHARG_API_STABILITY_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSHARG_DISABLE_DEPRECATED_WARNINGS=1")

macro (sharg_api_stability_test test_suite git_apply)
    ExternalProject_Add (
        api_stability_${test_suite}
        URL ${SHARG_LAST_STABLE_SOURCE_URL}
        URL_HASH SHA256=${SHARG_LAST_STABLE_SOURCE_SHA256}
        SOURCE_SUBDIR "test/${test_suite}"
        CMAKE_ARGS ${SHARG_EXTERNAL_PROJECT_CMAKE_ARGS}
                   -DCMAKE_CXX_FLAGS=${SHARG_API_STABILITY_CXX_FLAGS}
                   -DCMAKE_CXX_COMPILER_LAUNCHER=${CMAKE_CXX_COMPILER_LAUNCHER}
                   -DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_LIST_DIR}/../../build_system # use current build_system
                   -DSHARG_VERBOSE_TESTS=OFF
        PATCH_COMMAND ${CMAKE_COMMAND} -E echo "<SOURCE_DIR>"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "<SOURCE_DIR>/include"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "<SOURCE_DIR>/build_system"
        COMMAND sh -c "${git_apply}"
        TEST_BEFORE_INSTALL TRUE
        TEST_COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -j2
        INSTALL_COMMAND "")
endmacro ()

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${SHARG_LAST_STABLE_VERSION}")
    set (SHARG_GIT_APPLY
         "${GIT_EXECUTABLE} apply --verbose ${CMAKE_CURRENT_LIST_DIR}/${SHARG_LAST_STABLE_VERSION}/*.patch")
else ()
    set (SHARG_GIT_APPLY "true")
    message (STATUS "No patches found in ${CMAKE_CURRENT_LIST_DIR}/${SHARG_LAST_STABLE_VERSION}.")
endif ()

sharg_api_stability_test ("snippet" "${SHARG_GIT_APPLY}")
sharg_api_stability_test ("unit" "${SHARG_GIT_APPLY}")
