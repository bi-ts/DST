
#          Copyright Maksym V. Bilinets 2015 - 2016.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt )

cmake_minimum_required(VERSION 3.4)

project(dst)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(CMAKE_CONFIGURATION_TYPES Debug Release)

add_library(dst INTERFACE)

target_include_directories(dst INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

install(DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include DESTINATION .)

install(TARGETS dst EXPORT dst)
install(EXPORT dst DESTINATION cmake)

add_subdirectory(test)

