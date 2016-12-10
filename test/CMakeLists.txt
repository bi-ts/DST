
#          Copyright Maksym V. Bilinets 2015 - 2016.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          http://www.boost.org/LICENSE_1_0.txt )

cmake_minimum_required(VERSION 3.4)

find_package(Boost 1.61.0 EXACT QUIET)

if(NOT Boost_FOUND)
  include(get_boost)

  get_boost(1.61.0 015ae4afa6f3e597232bfe1dab949ace) 
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")                              
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")        
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")                                       
endif()

add_executable(dst_test 
  allocator/test_allocator_utility.cpp
  allocator/test_counter_allocator.cpp
  allocator/test_global_counter_allocator.cpp
  binary_tree/test_algorithm.cpp
  binary_tree/test_avl.cpp
  binary_tree/test_initializer_tree.cpp
  binary_tree/test_marking.cpp
  binary_tree/test_write_graphviz.cpp
  binary_tree/tools/trees_generator.cpp
  binary_tree/tools/trees_generator.h
  main.cpp
  test_utility.cpp
)

target_include_directories(dst_test PRIVATE "${Boost_INCLUDE_DIRS}")

target_link_libraries(dst_test dst)

add_test(NAME dst_test COMMAND dst_test)

