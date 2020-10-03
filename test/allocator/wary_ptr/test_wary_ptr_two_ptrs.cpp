
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/wary_ptr.h>

#include <dst/allocator/detail/wary_ptr_factory.h>

#include "tester_wary_ptr.h"

#include <gtest/gtest.h>

#include <cstdint> // std::int64_t

using namespace dst;

namespace
{
class Test_wary_ptr_two_ptrs : public ::testing::Test,
                               public dst::test::Tester_wary_ptr
{
public:
  Test_wary_ptr_two_ptrs()
  : ptr_1(detail::wary_ptr_factory::create_associated_ptr(values, elements_num))
  , ptr_2(ptr_1)
  {
    ++++ptr_2; // shift by two
  }

  ~Test_wary_ptr_two_ptrs() noexcept(true)
  {
    neutralize(ptr_2);
    neutralize(ptr_1);
  }

  static const std::size_t elements_num = 3;
  std::int64_t values[elements_num];
  dst::wary_ptr<std::int64_t> ptr_1;
  dst::wary_ptr<std::int64_t> ptr_2;
};
}

/// @fn wary_ptr<T>::operator==(const wary_ptr<U>& other) const
/// @test @b Test_wary_ptr_two_ptrs.equality_test <br>
///       Tests if:
///       * Two different pointers do not compare equal.
///       * Pointers compare equal to themselves.
///       * An associated and a loose pointer compare equal if they point to
///         the same memory location.
TEST_F(Test_wary_ptr_two_ptrs, equality_test)
{
  wary_ptr<std::int64_t> loose_ptr =
    wary_ptr<std::int64_t>::pointer_to(values[0]);

  EXPECT_FALSE(ptr_1 == ptr_2);
  EXPECT_FALSE(ptr_2 == ptr_1);

  EXPECT_TRUE(ptr_1 == ptr_1);
  EXPECT_TRUE(ptr_2 == ptr_2);

  EXPECT_TRUE(ptr_1 == loose_ptr);
}

/// @fn wary_ptr<T>::operator!=(const wary_ptr<U>& other) const
/// @test @b Test_wary_ptr_two_ptrs.inequality_test <br>
///       Uses operator!=() to tests if:
///       * Two different pointers compare inequal.
///       * Pointers do not compare inequal to themselves.
TEST_F(Test_wary_ptr_two_ptrs, inequality_test)
{
  EXPECT_TRUE(ptr_1 != ptr_2);
  EXPECT_TRUE(ptr_2 != ptr_1);

  EXPECT_FALSE(ptr_1 != ptr_1);
  EXPECT_FALSE(ptr_2 != ptr_2);
}

/// @fn wary_ptr<T>::operator<(const wary_ptr<U>& other) const
/// @test @b Test_wary_ptr_two_ptrs.operator_less <br>
///       Tests operator<() comparing:
///       * Two different pointers.
///       * The same pointer with itself.
TEST_F(Test_wary_ptr_two_ptrs, operator_less)
{
  EXPECT_TRUE(ptr_1 < ptr_2);
  EXPECT_FALSE(ptr_2 < ptr_1);

  EXPECT_FALSE(ptr_1 < ptr_1);
  EXPECT_FALSE(ptr_2 < ptr_2);
}

/// @fn wary_ptr<T>::operator>(const wary_ptr<U>& other) const
/// @test @b Test_wary_ptr_two_ptrs.operator_greater <br>
///       Tests operator>() comparing:
///       * Two different pointers.
///       * The same pointer with itself.
TEST_F(Test_wary_ptr_two_ptrs, operator_greater)
{
  EXPECT_FALSE(ptr_1 > ptr_2);
  EXPECT_TRUE(ptr_2 > ptr_1);

  EXPECT_FALSE(ptr_1 > ptr_1);
  EXPECT_FALSE(ptr_2 > ptr_2);
}

/// @fn wary_ptr<T>::operator<=(const wary_ptr<U>& other) const
/// @test @b Test_wary_ptr_two_ptrs.operator_less_eq <br>
///       Tests operator<=() comparing:
///       * Two different pointers.
///       * The same pointer with itself.
TEST_F(Test_wary_ptr_two_ptrs, operator_less_eq)
{
  EXPECT_TRUE(ptr_1 <= ptr_2);
  EXPECT_FALSE(ptr_2 <= ptr_1);

  EXPECT_TRUE(ptr_1 <= ptr_1);
  EXPECT_TRUE(ptr_2 <= ptr_2);
}

/// @fn wary_ptr<T>::operator>=(const wary_ptr<U>& other) const
/// @test @b Test_wary_ptr_two_ptrs.operator_greater_eq <br>
///       Tests operator>=() comparing:
///       * Two different pointers.
///       * The same pointer with itself.
TEST_F(Test_wary_ptr_two_ptrs, operator_greater_eq)
{
  EXPECT_FALSE(ptr_1 >= ptr_2);
  EXPECT_TRUE(ptr_2 >= ptr_1);

  EXPECT_TRUE(ptr_1 >= ptr_1);
  EXPECT_TRUE(ptr_2 >= ptr_2);
}

/// @fn dst::wary_ptr::operator+()
/// @test @b Test_wary_ptr_two_ptrs.operator_plus <br>
///       Uses operator+() to shift pointer.
TEST_F(Test_wary_ptr_two_ptrs, operator_plus)
{
  auto ptr = ptr_1 + 2;

  EXPECT_EQ(ptr_2, ptr);
}

/// @fn dst::wary_ptr::operator-(std::ptrdiff_t) const
/// @test @b Test_wary_ptr_two_ptrs.operator_minus <br>
///       Uses operator-() to shift pointer.
TEST_F(Test_wary_ptr_two_ptrs, operator_minus)
{
  auto ptr = ptr_2 - 2;

  EXPECT_EQ(ptr_1, ptr);
}

/// @fn dst::wary_ptr::operator-(const wary_ptr<T>&) const
/// @test @b Test_wary_ptr_two_ptrs.difference <br>
///       Uses operator-() to calculate difference between pointers.
TEST_F(Test_wary_ptr_two_ptrs, difference)
{
  EXPECT_EQ(2, ptr_2 - ptr_1);
  EXPECT_EQ(-2, ptr_1 - ptr_2);
}
