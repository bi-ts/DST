
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/global_counter_allocator.h>

#include <gtest/gtest.h>

#include <list>
#include <memory> // std::allocator_traits

TEST(test_global_counter_allocator, allocate_int)
{
  using allocator_type = dst::global_counter_allocator<int>;

  allocator_type allocator;

  EXPECT_EQ(0, allocator_type::allocated());

  int* p_int = allocator.allocate(1);

  EXPECT_EQ(sizeof(int), allocator_type::allocated());

  int* p_ints = allocator.allocate(10);

  EXPECT_EQ(11 * sizeof(int), allocator_type::allocated());

  allocator.deallocate(p_int, 1);

  EXPECT_EQ(10 * sizeof(int), allocator_type::allocated());

  allocator.deallocate(p_ints, 10);

  EXPECT_EQ(0, allocator_type::allocated());
}

TEST(test_global_counter_allocator, void_allocator)
{
  using int_allocator_type = dst::global_counter_allocator<int>;
  using void_allocator_type = dst::global_counter_allocator<void>;
  
  int_allocator_type allocator;

  EXPECT_EQ(0, void_allocator_type::allocated());

  std::allocator_traits<void_allocator_type>::pointer p_void =
    allocator.allocate(3);

  EXPECT_EQ(3 * sizeof(int), void_allocator_type::allocated());

  allocator.deallocate(
    static_cast<std::allocator_traits<int_allocator_type>::pointer>(p_void),
    3);

  EXPECT_EQ(0, void_allocator_type::allocated());
}

