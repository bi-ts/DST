
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/detail/wary_ptr_factory.h>
#include <dst/allocator/wary_ptr.h>

#include "tester_wary_ptr.h"

#include <gtest/gtest.h>

#include <cstdint> // std::int64_t
#include <numeric> // std::accumulate

using namespace dst;

namespace
{
class Test_wary_ptr_array : public ::testing::Test,
                            public dst::test::Tester_wary_ptr
{
public:
  Test_wary_ptr_array()
  : ptr(detail::wary_ptr_factory::create_associated_ptr(values, elements_num))
  {
    for (std::size_t i = 0; i < elements_num; ++i)
      values[i] = i;
  }

  ~Test_wary_ptr_array() noexcept(true)
  {
    neutralize(ptr);
  }

  static const std::size_t elements_num = 3;
  std::int64_t values[elements_num];
  dst::wary_ptr<std::int64_t> ptr;
};
}

/// @fn dst::detail::wary_ptr_det::wary_ptr::operator+=()
/// @test @b Test_wary_ptr_array.ptr_plus_assign <br>
TEST_F(Test_wary_ptr_array, ptr_plus_assign)
{
  ptr += 1;

  *ptr = 28;

  EXPECT_EQ(28, values[1]);
}

/// @fn dst::detail::wary_ptr_det::wary_ptr::operator++()
/// @test @b Test_wary_ptr_array.ptr_inc <br>
TEST_F(Test_wary_ptr_array, ptr_inc)
{
  EXPECT_EQ(0, *ptr);

  EXPECT_EQ(1, *(++ptr));
}

/// @fn dst::detail::wary_ptr_det::wary_ptr::operator++(int)
/// @test @b Test_wary_ptr_array.ptr_post_inc <br>
TEST_F(Test_wary_ptr_array, ptr_post_inc)
{
  EXPECT_EQ(0, *(ptr++));

  EXPECT_EQ(1, *ptr);
}

/// @fn dst::detail::wary_ptr_det::wary_ptr::operator-=()
/// @test @b Test_wary_ptr_array.ptr_minus_assign <br>
TEST_F(Test_wary_ptr_array, ptr_minus_assign)
{
  EXPECT_EQ(0, *ptr);

  ptr -= 1;

  EXPECT_FALSE(state(ptr).is_valid());
}

/// @fn dst::detail::wary_ptr_det::wary_ptr::operator--()
/// @test @b Test_wary_ptr_array.ptr_dec <br>
TEST_F(Test_wary_ptr_array, ptr_dec)
{
  EXPECT_TRUE(state(ptr).is_valid());

  EXPECT_FALSE(state(--ptr).is_valid());
}

/// @fn dst::detail::wary_ptr_det::wary_ptr::operator--(int)
/// @test @b Test_wary_ptr_array.ptr_post_dec <br>
TEST_F(Test_wary_ptr_array, ptr_post_dec)
{
  EXPECT_TRUE(state(ptr--).is_valid());

  EXPECT_FALSE(state(ptr).is_valid());
}

/// @fn dst::detail::wary_ptr_det::wary_ptr::operator+=()
/// @test @b Test_wary_ptr_array.out_of_range_shift <br>
///       Uses operator+=() to shift pointer beyond array's boundaries.
///       Checks that it became invalid.
TEST_F(Test_wary_ptr_array, out_of_range_shift)
{
  ptr += elements_num;

  EXPECT_TRUE(!state(ptr).is_valid());
}

/// @fn dst::detail::wary_ptr_det::wary_ptr_base<T,false>::operator[]()
/// @test @b Test_wary_ptr_array.indexing <br>
///       Uses subscript operator to access elements of an array via wary_ptr.
TEST_F(Test_wary_ptr_array, indexing)
{
  ptr[0] = 111;
  ptr[1] = 222;

  EXPECT_EQ(111, values[0]);
  EXPECT_EQ(222, values[1]);
}

/// @fn dst::detail::wary_ptr_det::wary_ptr_base<T,false>::operator[]()
/// @test @b Test_wary_ptr_array.index_out_of_range <br>
///       Checks if std::logic_error is thrown in case when
///       the index is out of range.
TEST_F(Test_wary_ptr_array, index_out_of_range)
{
  EXPECT_THROW(ptr[elements_num], std::logic_error);
}

/// @fn wary_ptr::release()
/// @test @b Test_wary_ptr_array.release_not_from_head <br>
///       Checks that release() throws @c std::logic_error exception if
///       the pointer given points not to the first byte of its memory block.
TEST_F(Test_wary_ptr_array, release_not_from_head)
{
  EXPECT_THROW(detail::wary_ptr_factory::release(++ptr), std::logic_error);
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_array.as_an_iterator<br>
///       Tests if wary_ptr can be used as an iterator in STL algorithms.
TEST_F(Test_wary_ptr_array, as_an_iterator)
{
  const auto n = elements_num;
  EXPECT_EQ(n, std::accumulate(ptr, ptr + n, std::int64_t(0)));
}
