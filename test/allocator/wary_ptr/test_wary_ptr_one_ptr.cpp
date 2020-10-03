
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/wary_ptr.h>

#include <dst/allocator/detail/wary_ptr_factory.h>

#include "tester_wary_ptr.h"

#include <gtest/gtest.h>

using namespace dst;

namespace
{
class Test_wary_ptr_one_ptr : public ::testing::Test,
                              public dst::test::Tester_wary_ptr
{
public:
  Test_wary_ptr_one_ptr()
  : value(0)
  , ptr(detail::wary_ptr_factory::create_associated_ptr(&value, 1))
  {
  }

  ~Test_wary_ptr_one_ptr() noexcept(true)
  {
    neutralize(ptr);
  }

  int value;
  dst::wary_ptr<int> ptr;
};
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_one_ptr.compare_with_nullptr <br>
///       Tests all possible comparison operations of non-const wary_ptr with
///       @c nullptr.
TEST_F(Test_wary_ptr_one_ptr, compare_with_nullptr)
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

/// @fn wary_ptr<T>::operator=(wary_ptr<T>)
/// @test @b Test_wary_ptr_one_ptr.assign_with_mem_leak <br>
///       Tests if @c std::logic_error exception is thrown when the last
///       pointer to a memory block is assigned @c nullptr.
TEST_F(Test_wary_ptr_one_ptr, assign_with_mem_leak)
{
  EXPECT_THROW(ptr = nullptr, std::logic_error);
}

/// @fn wary_ptr<T>::operator=(wary_ptr<T>)
/// @test @b Test_wary_ptr_one_ptr.self_assignment <br>
///       Tests if exclusive pointer self-assignment doesn't throw.
TEST_F(Test_wary_ptr_one_ptr, self_assignment)
{
  ptr = ptr;
}

/// @fn wary_ptr<T>::operator=(wary_ptr<T>)
/// @test @b Test_wary_ptr_one_ptr.assign_null_ptr <br>
///       Tests if wary_ptr is disassociated from its memory block after
///       @c nullptr assignment.
TEST_F(Test_wary_ptr_one_ptr, assign_null_ptr)
{
  wary_ptr<int> ptr_copy = ptr;

  EXPECT_EQ(&value, state(ptr).ptr());
  EXPECT_EQ(2, state(ptr).info()->use_count());

  ptr = nullptr;

  EXPECT_EQ(nullptr, state(ptr).ptr());
  EXPECT_EQ(1, state(ptr_copy).info()->use_count());

  neutralize(ptr_copy);
}

/// @fn wary_ptr<T>::operator=(wary_ptr<T>)
/// @test @b Test_wary_ptr_one_ptr.assign_zero <br>
///       Tests if wary_ptr is disassociated from its memory block after
///       0 value assignment.
TEST_F(Test_wary_ptr_one_ptr, assign_zero)
{
  wary_ptr<int> ptr_copy = ptr;

  EXPECT_EQ(&value, state(ptr).ptr());
  EXPECT_EQ(2, state(ptr).info()->use_count());

  ptr = 0;

  EXPECT_EQ(nullptr, state(ptr).ptr());
  EXPECT_EQ(1, state(ptr_copy).info()->use_count());

  neutralize(ptr_copy);
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_one_ptr.implicit_cast_to_void <br>
///       Tests implicit wary_ptr<int> conversion to wary_ptr<void>.
TEST_F(Test_wary_ptr_one_ptr, implicit_cast_to_void)
{
  using dst::wary_ptr;

  wary_ptr<void> ptr_void = ptr;

  EXPECT_EQ(ptr, ptr_void);

  EXPECT_EQ(2, state(ptr).info()->use_count());
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_one_ptr.implicit_cast_to_const_void <br>
///       Tests implicit wary_ptr<int> conversion to wary_ptr<const void>.
TEST_F(Test_wary_ptr_one_ptr, implicit_cast_to_const_void)
{
  using dst::wary_ptr;

  wary_ptr<const void> ptr_void = ptr;

  EXPECT_EQ(ptr, ptr_void);

  EXPECT_EQ(2, state(ptr).info()->use_count());
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_one_ptr.implicit_cast_to_const <br>
///       Tests implicit wary_ptr<int> conversion to wary_ptr<const int>.
TEST_F(Test_wary_ptr_one_ptr, implicit_cast_to_const)
{
  using dst::wary_ptr;

  wary_ptr<const int> ptr_const = ptr;

  EXPECT_EQ(ptr, ptr_const);

  EXPECT_EQ(2, state(ptr).info()->use_count());
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_one_ptr.static_cast_from_void <br>
///       Tests explicit wary_ptr<void> conversion to wary_ptr<int>.
TEST_F(Test_wary_ptr_one_ptr, static_cast_from_void)
{
  using dst::wary_ptr;

  wary_ptr<void> ptr_void = ptr;

  wary_ptr<int> ptr_2 = static_cast<wary_ptr<int>>(ptr_void);

  EXPECT_EQ(ptr, ptr_2);

  EXPECT_EQ(3, state(ptr).info()->use_count());
}

/// @fn wary_ptr::release()
/// @test @b Test_wary_ptr_one_ptr.release_non_exclusive <br>
///       Checks that if a memory block is released through one of its
///       associated pointers, another pointer becomes wild.
TEST_F(Test_wary_ptr_one_ptr, release_non_exclusive)
{
  wary_ptr<int> ptr_copy = ptr;

  EXPECT_EQ(2, state(ptr).info()->use_count());
  EXPECT_FALSE(state(ptr_copy).is_wild());

  auto res = detail::wary_ptr_factory::release(ptr);

  EXPECT_TRUE(state(ptr_copy).is_wild());
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_one_ptr.associated_null_ptr <br>
///       Checks that associated wary_ptr remains exclusively associated even
///       if it becomes null after some arithmetical operations.
TEST_F(Test_wary_ptr_one_ptr, associated_null_ptr)
{
  wary_ptr<void> ptr_void = static_cast<wary_ptr<void>>(ptr);

  wary_ptr<char> ptr_byte = static_cast<wary_ptr<char>>(ptr_void);

  ptr_byte -= (reinterpret_cast<std::ptrdiff_t>(&value));

  EXPECT_TRUE(state(ptr_byte).is_null());
  EXPECT_TRUE(state(ptr_byte).is_associated());
  EXPECT_FALSE(state(ptr_byte).is_exclusive());
}
