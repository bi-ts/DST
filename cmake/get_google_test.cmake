
cmake_minimum_required(VERSION 3.4)

set(_input_file "${CMAKE_CURRENT_LIST_DIR}/get_google_test/CMakeLists.txt.in")

macro(get_google_test google_test_version google_test_md5)
  set(import_file_path
      "${CMAKE_BINARY_DIR}/ext-projects/google_test/src/google_test-install/lib/cmake/GTest/GTestConfig.cmake")

  if(NOT EXISTS "${import_file_path}")
    message(STATUS "Getting Google Test...")

    set(EXT_PROJECTS_DIR "${CMAKE_BINARY_DIR}/ext-projects")
    set(GOOGLE_TEST_VERSION ${google_test_version})
    set(GOOGLE_TEST_PACKAGE_MD5 ${google_test_md5})

    configure_file(
      "${_input_file}"
      "${CMAKE_BINARY_DIR}/get-google_test/CMakeLists.txt"
      @ONLY
    )

    unset(GOOGLE_TEST_VERSION)
    unset(GOOGLE_TEST_PACKAGE_MD5)

    execute_process(
      COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/get-google_test"
    )

    execute_process(
      COMMAND ${CMAKE_COMMAND} --build .
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/get-google_test"
    )

  endif()

  include("${import_file_path}")

  if(NOT TARGET GTest::gtest)
    message(FATAL_ERROR "Failed to import target 'test::test'")
  endif()

  if(NOT TARGET GTest::gtest_main)
    message(FATAL_ERROR "Failed to import target 'test::test_main'")
  endif()
endmacro()

