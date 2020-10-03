
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/detail/wary_ptr_factory.h>
#include <dst/allocator/wary_ptr.h>

#include "tester_wary_ptr.h"

#include <gtest/gtest.h>

#include <cstdint>

using namespace dst;

namespace
{
class Test_wary_ptr_null_ptr : public ::testing::Test,
                               public dst::test::Tester_wary_ptr
{
public:
  struct Obj
  {
    std::int64_t field;
  };

public:
  Test_wary_ptr_null_ptr()
  : ptr(nullptr)
  {
  }

  ~Test_wary_ptr_null_ptr() noexcept(true)
  {
  }

  dst::wary_ptr<Obj> ptr;
};
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_null_ptr.compare_with_nullptr <br>
///       Tests all possible comparison operations of null wary_ptr with
///       @c nullptr.
/// @todo separate comparison operation testing into separate tests.
TEST_F(Test_wary_ptr_null_ptr, compare_with_nullptr)
{
  EXPECT_TRUE(ptr == nullptr);
  EXPECT_TRUE(nullptr == ptr);

  EXPECT_FALSE(ptr != nullptr);
  EXPECT_FALSE(nullptr != ptr);

  EXPECT_FALSE(ptr < nullptr);
  EXPECT_FALSE(nullptr < ptr);

  EXPECT_TRUE(ptr <= nullptr);
  EXPECT_TRUE(nullptr <= ptr);

  EXPECT_FALSE(ptr > nullptr);
  EXPECT_FALSE(nullptr > ptr);

  EXPECT_TRUE(ptr >= nullptr);
  EXPECT_TRUE(nullptr >= ptr);
}

/// @fn wary_ptr::release()
/// @test @b Test_wary_ptr_null_ptr.release_fails <br>
///       Checks that release() on null pointer throws @c std::logic_error
///       exception.
TEST_F(Test_wary_ptr_null_ptr, release_fails)
{
  EXPECT_THROW(detail::wary_ptr_factory::release(ptr), std::logic_error);
}

/// @fn dst::detail::wary_ptr_det::wary_ptr_base<T, false>::operator*()
/// @test @b Test_wary_ptr_null_ptr.dereference_fails <br>
///       Checks if std::logic_error is thrown when null
///       pointer is dereferenced.
TEST_F(Test_wary_ptr_null_ptr, dereference_fails)
{
  EXPECT_THROW(*ptr, std::logic_error);
}

/// @fn dst::wary_ptr<T>::operator->()
/// @test @b Test_wary_ptr_null_ptr.arrow_fails <br>
///       Checks if arrow operator throws std::logic_error on
///       null pointer.
TEST_F(Test_wary_ptr_null_ptr, arrow_fails)
{
  EXPECT_THROW(ptr->field = 0, std::logic_error);
}
