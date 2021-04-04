
cmake_minimum_required(VERSION 3.18.4)

project(dst)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include("${CMAKE_CURRENT_LIST_DIR}/cmake/conan.cmake")

conan_cmake_run(CONANFILE conanfile.txt BASIC_SETUP KEEP_RPATHS CMAKE_TARGETS)

if (MSVC)
  add_compile_options(/WX)
else()
  add_compile_options(-Werror)
endif()

add_library(dst INTERFACE)

target_include_directories(dst INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include DESTINATION .)

install(TARGETS dst EXPORT dst)
install(EXPORT dst DESTINATION cmake)

add_subdirectory(test)

