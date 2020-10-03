
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/detail/wary_ptr_factory.h>
#include <dst/allocator/wary_ptr.h>

#include "tester_wary_ptr.h"

#include <gtest/gtest.h>

using namespace dst;

namespace
{
class Test_wary_ptr_const_ptr : public ::testing::Test,
                                public dst::test::Tester_wary_ptr
{
public:
  Test_wary_ptr_const_ptr()
  : value(0)
  , ptr(detail::wary_ptr_factory::create_associated_ptr(&value, 1))
  {
  }

  ~Test_wary_ptr_const_ptr() noexcept(true)
  {
    neutralize(ptr);
  }

  int value;
  const dst::wary_ptr<int> ptr;
};
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_const_ptr.compare_with_nullptr <br>
///       Tests all possible comparison operations of const wary_ptr with
///       @c nullptr.
TEST_F(Test_wary_ptr_const_ptr, compare_with_nullptr)
{
  EXPECT_FALSE(ptr == nullptr);
  EXPECT_FALSE(nullptr == ptr);

  EXPECT_TRUE(ptr != nullptr);
  EXPECT_TRUE(nullptr != ptr);

  EXPECT_FALSE(ptr < nullptr);
  EXPECT_TRUE(nullptr < ptr);

  EXPECT_FALSE(ptr <= nullptr);
  EXPECT_TRUE(nullptr <= ptr);

  EXPECT_TRUE(ptr > nullptr);
  EXPECT_FALSE(nullptr > ptr);

  EXPECT_TRUE(ptr >= nullptr);
  EXPECT_FALSE(nullptr >= ptr);
}

/// @fn dst::detail::wary_ptr_det::wary_ptr_base<T, false>::operator*()
/// @test @b Test_wary_ptr_const_ptr.dereference <br>
///       Tests getting/setting values via pointer dereferencing.
TEST_F(Test_wary_ptr_const_ptr, dereference)
{
  value = 27;

  EXPECT_EQ(27, *ptr);

  *ptr = 0;

  EXPECT_EQ(0, value);
}

/// @fn wary_ptr<T>::wary_ptr(const wary_ptr<T>& other)
/// @test @b Test_wary_ptr_const_ptr.copy_ctor <br>
///       Tests copy constructor on @c const @c wary_ptr<int>.
TEST_F(Test_wary_ptr_const_ptr, copy_ctor)
{
  EXPECT_EQ(1, state(ptr).info()->use_count());

  dst::wary_ptr<int> ptr2(ptr);

  EXPECT_EQ(ptr, ptr2);
  EXPECT_EQ(2, state(ptr).info()->use_count());
}

/// @fn wary_ptr::release()
/// @test @b Test_wary_ptr_const_ptr.can_be_released <br>
///       Checks if a valid associated exclusive const wary_ptr can be
///       released.
TEST_F(Test_wary_ptr_const_ptr, can_be_released)
{
  EXPECT_FALSE(state(ptr).is_null());
  EXPECT_TRUE(state(ptr).is_associated());
  EXPECT_TRUE(state(ptr).is_exclusive());
  EXPECT_FALSE(state(ptr).is_released());
  EXPECT_FALSE(state(ptr).is_wild());
  EXPECT_TRUE(state(ptr).is_valid());

  detail::wary_ptr_factory::release(ptr);

  EXPECT_EQ(1, state(ptr).info()->use_count());

  EXPECT_FALSE(state(ptr).is_null());
  EXPECT_TRUE(state(ptr).is_associated());
  EXPECT_FALSE(state(ptr).is_exclusive());
  EXPECT_TRUE(state(ptr).is_released());
  EXPECT_TRUE(state(ptr).is_wild());
  EXPECT_FALSE(state(ptr).is_valid());
}
