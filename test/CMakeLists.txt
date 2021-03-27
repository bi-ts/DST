
cmake_minimum_required(VERSION 3.18.4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(dst_test
  allocator/test_allocator_utility.cpp
  allocator/test_counter_allocator.cpp
  allocator/test_global_counter_allocator.cpp
  binary_tree/test_algorithm.cpp
  binary_tree/test_avl.cpp
  binary_tree/test_indexing.cpp
  binary_tree/test_initializer_tree.cpp
  binary_tree/test_marking.cpp
  binary_tree/test_ordering.cpp
  binary_tree/test_write_graphviz.cpp
  binary_tree/tools/trees_generator.cpp
  binary_tree/tools/trees_generator.h
  main.cpp
  test_utility.cpp
)

target_link_libraries(dst_test dst CONAN_PKG::boost)

add_test(NAME dst_test COMMAND dst_test)

add_executable(dst_test_wary_ptr
  allocator/wary_ptr/test_mem_block_info.cpp
  allocator/wary_ptr/test_wary_alloc_counter.cpp
  allocator/wary_ptr/test_wary_allocator.cpp
  allocator/wary_ptr/test_wary_ptr_array.cpp
  allocator/wary_ptr/test_wary_ptr_const_ptr.cpp
  allocator/wary_ptr/test_wary_ptr_init.cpp
  allocator/wary_ptr/test_wary_ptr_nullptr.cpp
  allocator/wary_ptr/test_wary_ptr_obj_ptr.cpp
  allocator/wary_ptr/test_wary_ptr_one_ptr.cpp
  allocator/wary_ptr/test_wary_ptr_two_ptrs.cpp
  allocator/wary_ptr/tester_wary_ptr.h
)

target_link_libraries(dst_test_wary_ptr dst CONAN_PKG::gtest)

add_test(NAME dst_test_wary_ptr COMMAND dst_test_wary_ptr)

