
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/counter_allocator.h>

#include <gtest/gtest.h>

#include <list>
#include <memory> // std::allocator_traits

TEST(test_counter_allocator, allocate_int)
{
  dst::counter_allocator<int> allocator;

  EXPECT_EQ(0, allocator.allocated());

  int* p_int = allocator.allocate(1);

  EXPECT_EQ(sizeof(int), allocator.allocated());

  int* p_ints = allocator.allocate(10);

  EXPECT_EQ(11 * sizeof(int), allocator.allocated());

  allocator.deallocate(p_int, 1);

  EXPECT_EQ(10 * sizeof(int), allocator.allocated());

  allocator.deallocate(p_ints, 10);

  EXPECT_EQ(0, allocator.allocated());
}

TEST(test_counter_allocator, rebind)
{
  dst::counter_allocator<int> int_allocator;

  std::allocator_traits<dst::counter_allocator<int>>::rebind_alloc<double>
    double_allocator(int_allocator);

  EXPECT_EQ(0, int_allocator.allocated());
  EXPECT_EQ(0, double_allocator.allocated());

  int* p_int = int_allocator.allocate(1);

  EXPECT_EQ(sizeof(int), int_allocator.allocated());
  EXPECT_EQ(sizeof(int), double_allocator.allocated());

  double* p_double = double_allocator.allocate(2);

  EXPECT_EQ(2 * sizeof(double) + sizeof(int), int_allocator.allocated());
  EXPECT_EQ(2 * sizeof(double) + sizeof(int), double_allocator.allocated());

  int_allocator.deallocate(p_int, 1);

  EXPECT_EQ(2 * sizeof(double), int_allocator.allocated());
  EXPECT_EQ(2 * sizeof(double), double_allocator.allocated());

  double_allocator.deallocate(p_double, 2);

  EXPECT_EQ(0, int_allocator.allocated());
  EXPECT_EQ(0, double_allocator.allocated());
}

TEST(test_counter_allocator, with_std_list)
{
  dst::counter_allocator<int> allocator;

  EXPECT_EQ(0, allocator.allocated());

  {
    std::list<int, dst::counter_allocator<int>> l(allocator);

    l.assign({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});

    EXPECT_NE(0, allocator.allocated());
  }

  EXPECT_EQ(0, allocator.allocated());
}

TEST(test_counter_allocator, base_allocator)
{
  dst::counter_allocator<int> base_allocator;
  dst::counter_allocator<int, dst::counter_allocator<int>> allocator(
    base_allocator);

  EXPECT_TRUE(allocator.base() == base_allocator);
  EXPECT_FALSE(allocator.base() == dst::counter_allocator<int>());

  EXPECT_EQ(0, base_allocator.allocated());
  EXPECT_EQ(0, allocator.allocated());

  int* p_ints = allocator.allocate(10);

  EXPECT_EQ(10 * sizeof(int), base_allocator.allocated());
  EXPECT_EQ(10 * sizeof(int), allocator.allocated());

  allocator.deallocate(p_ints, 10);

  EXPECT_EQ(0, base_allocator.allocated());
  EXPECT_EQ(0, allocator.allocated());
}

TEST(test_counter_allocator, void_allocator)
{
  dst::counter_allocator<int> allocator;

  EXPECT_EQ(0, allocator.allocated());

  std::allocator_traits<dst::counter_allocator<void>>::pointer p_void =
    allocator.allocate(2);

  EXPECT_EQ(2 * sizeof(int), allocator.allocated());

  allocator.deallocate(
    static_cast<std::allocator_traits<dst::counter_allocator<int>>::pointer>(
      p_void),
    2);

  EXPECT_EQ(0, allocator.allocated());
}

