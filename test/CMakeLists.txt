
cmake_minimum_required(VERSION 3.4)

find_package(Boost 1.61.0)

if(NOT Boost_FOUND)
  message(STATUS "Getting Boost...")

  include(get_boost)

  get_boost(1.61.0 015ae4afa6f3e597232bfe1dab949ace) 

  find_package(Boost 1.61.0 REQUIRED)
endif()

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")                              
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")        
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")                                       
endif()

add_executable(dst_test 
  binary_tree/test_algorithm.cpp
  binary_tree/test_write_graphviz.cpp
  binary_tree/tools/fibonacci_trees.cpp
  binary_tree/tools/fibonacci_trees.h
  main.cpp
  test_allocator_utility.cpp
  test_counter_allocator.cpp
  test_global_counter_allocator.cpp
)

target_include_directories(dst_test PRIVATE "${Boost_INCLUDE_DIRS}")

target_link_libraries(dst_test dst)

add_test(NAME dst_test COMMAND dst_test --log_level=test_suite)

