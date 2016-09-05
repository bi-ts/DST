
cmake_minimum_required(VERSION 3.4)

if(NOT GTEST_FOUND)
  find_package(GTest REQUIRED)
endif()

include_directories(${GTEST_INCLUDE_DIRS})

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")                              
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")        
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")                                       
endif()

add_executable(dst_test 
  test_allocator_utility.cpp
  test_counter_allocator.cpp
  test_global_counter_allocator.cpp)

target_link_libraries(dst_test dst ${GTEST_BOTH_LIBRARIES})

