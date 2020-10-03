
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <gtest/gtest.h>

#include <cstdint> // std::int64_t, std::int16_t

#include <dst/allocator/detail/wary_alloc_counter.h>

using namespace dst;

namespace
{
class Test_wary_alloc_counter
: public ::testing::Test,
  public detail::wary_alloc_det::wary_alloc_counter
{
public:
  Test_wary_alloc_counter()
  {
  }

  ~Test_wary_alloc_counter()
  {
  }
};
}

/// @class dst::detail::wary_alloc_det::wary_alloc_counter
/// @test @b Test_wary_alloc_counter.notification <br>
TEST_F(Test_wary_alloc_counter, notification)
{
  EXPECT_EQ(0, allocations_count());
  EXPECT_EQ(0, allocated_bytes());
  EXPECT_EQ(0, allocated_bytes_max());
  EXPECT_EQ(0, allocated_bytes_total());

  notify_objects_allocated<std::int64_t>(1);

  EXPECT_EQ(1, allocations_count());
  EXPECT_EQ(sizeof(std::int64_t), allocated_bytes());
  EXPECT_EQ(sizeof(std::int64_t), allocated_bytes_max());
  EXPECT_EQ(sizeof(std::int64_t), allocated_bytes_total());

  notify_objects_allocated<std::int64_t>(5);

  EXPECT_EQ(2, allocations_count());
  EXPECT_EQ(6 * sizeof(std::int64_t), allocated_bytes());
  EXPECT_EQ(6 * sizeof(std::int64_t), allocated_bytes_max());
  EXPECT_EQ(6 * sizeof(std::int64_t), allocated_bytes_total());

  notify_objects_deallocated(typeid(std::int64_t), 5, sizeof(std::int64_t));

  EXPECT_EQ(2, allocations_count());
  EXPECT_EQ(1 * sizeof(std::int64_t), allocated_bytes());
  EXPECT_EQ(6 * sizeof(std::int64_t), allocated_bytes_max());
  EXPECT_EQ(6 * sizeof(std::int64_t), allocated_bytes_total());

  notify_objects_allocated<std::int64_t>(3);

  EXPECT_EQ(3, allocations_count());
  EXPECT_EQ(4 * sizeof(std::int64_t), allocated_bytes());
  EXPECT_EQ(6 * sizeof(std::int64_t), allocated_bytes_max());
  EXPECT_EQ(9 * sizeof(std::int64_t), allocated_bytes_total());

  notify_objects_allocated<std::int64_t>(3);

  EXPECT_EQ(4, allocations_count());
  EXPECT_EQ(7 * sizeof(std::int64_t), allocated_bytes());
  EXPECT_EQ(7 * sizeof(std::int64_t), allocated_bytes_max());
  EXPECT_EQ(12 * sizeof(std::int64_t), allocated_bytes_total());

  notify_objects_deallocated(typeid(std::int64_t), 3, sizeof(std::int64_t));
  notify_objects_deallocated(typeid(std::int64_t), 3, sizeof(std::int64_t));
  notify_objects_deallocated(typeid(std::int64_t), 1, sizeof(std::int64_t));

  EXPECT_EQ(4, allocations_count());
  EXPECT_EQ(0, allocated_bytes());
  EXPECT_EQ(7 * sizeof(std::int64_t), allocated_bytes_max());
  EXPECT_EQ(12 * sizeof(std::int64_t), allocated_bytes_total());
}

/// @class dst::detail::wary_alloc_det::wary_alloc_counter
/// @test @b Test_wary_alloc_counter.copy_ctor <br>
TEST_F(Test_wary_alloc_counter, copy_ctor)
{
  detail::wary_alloc_det::wary_alloc_counter another(*this);

  EXPECT_EQ(0, allocations_count());
  EXPECT_EQ(0, another.allocations_count());

  notify_objects_allocated<std::int64_t>(1);

  EXPECT_EQ(1, allocations_count());
  EXPECT_EQ(1, another.allocations_count());

  notify_objects_deallocated(typeid(std::int64_t), 1, sizeof(std::int64_t));
}

/// @class dst::detail::wary_alloc_det::wary_alloc_counter
/// @test @b Test_wary_alloc_counter.equality_test <br>
TEST_F(Test_wary_alloc_counter, equality_test)
{
  detail::wary_alloc_det::wary_alloc_counter copy(*this);
  detail::wary_alloc_det::wary_alloc_counter another;

  EXPECT_TRUE(*this == copy);
  EXPECT_FALSE(*this == another);
}

/// @class dst::detail::wary_alloc_det::wary_alloc_counter
/// @test @b Test_wary_alloc_counter.inequality_test <br>
TEST_F(Test_wary_alloc_counter, inequality_test)
{
  detail::wary_alloc_det::wary_alloc_counter copy(*this);
  detail::wary_alloc_det::wary_alloc_counter another;

  EXPECT_TRUE(*this != another);
  EXPECT_FALSE(*this != copy);
}

/// @class dst::detail::wary_alloc_det::wary_alloc_counter
/// @test @b Test_wary_alloc_counter.allocated_objects <br>
///       Notifies about objects allocation and deallocation of std::int64_t
///       and std::int16_t types and checks if they are counted properly.
TEST_F(Test_wary_alloc_counter, allocated_objects)
{
  notify_objects_allocated<std::int64_t>(1);

  EXPECT_EQ(1, allocated_objects<std::int64_t>());
  EXPECT_EQ(0, allocated_objects<std::int16_t>());

  notify_objects_allocated<std::int16_t>(3);

  EXPECT_EQ(1, allocated_objects<std::int64_t>());
  EXPECT_EQ(3, allocated_objects<std::int16_t>());

  notify_objects_allocated<std::int64_t>(3);

  EXPECT_EQ(4, allocated_objects<std::int64_t>());
  EXPECT_EQ(3, allocated_objects<std::int16_t>());

  notify_objects_allocated<std::int16_t>(1);

  EXPECT_EQ(4, allocated_objects<std::int64_t>());
  EXPECT_EQ(4, allocated_objects<std::int16_t>());

  notify_objects_deallocated(typeid(std::int64_t), 3, sizeof(std::int64_t));
  notify_objects_deallocated(typeid(std::int16_t), 3, sizeof(std::int16_t));

  EXPECT_EQ(1, allocated_objects<std::int64_t>());
  EXPECT_EQ(1, allocated_objects<std::int16_t>());

  notify_objects_deallocated(typeid(std::int64_t), 1, sizeof(std::int64_t));
  notify_objects_deallocated(typeid(std::int16_t), 1, sizeof(std::int16_t));

  EXPECT_EQ(0, allocated_objects<std::int64_t>());
  EXPECT_EQ(0, allocated_objects<std::int16_t>());

  EXPECT_EQ(0, allocated_bytes());
}
