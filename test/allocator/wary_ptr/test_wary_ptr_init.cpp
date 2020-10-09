
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/detail/wary_ptr_factory.h>
#include <dst/allocator/wary_ptr.h>

#include "tester_wary_ptr.h"

#include <gtest/gtest.h>

#include <memory>      // std::pointer_traits
#include <type_traits> // std::is_same

using namespace dst;

namespace
{
class Test_wary_ptr_init : public ::testing::Test,
                           public dst::test::Tester_wary_ptr
{
public:
  Test_wary_ptr_init()
  : value(0)
  {
  }

  int value;
};
}

/// @fn detail::wary_ptr_factory::create_associated_ptr
/// @test @b Test_wary_ptr_init.ctor_checks_value_pointer <br>
///       Checks if std::invalid_argument is thrown if value pointer is
///       @c nullptr.
TEST_F(Test_wary_ptr_init, ctor_checks_value_pointer)
{
  EXPECT_THROW(detail::wary_ptr_factory::create_associated_ptr<int>(nullptr, 1),
               std::invalid_argument);
}

/// @fn detail::wary_ptr_factory::create_associated_ptr
/// @test @b Test_wary_ptr_init.ctor_checks_mem_block_size <br>
///       Checks if std::invalid_argument is thrown if memory block size is 0.
TEST_F(Test_wary_ptr_init, ctor_checks_mem_block_size)
{
  EXPECT_THROW(detail::wary_ptr_factory::create_associated_ptr(&value, 0),
               std::invalid_argument);
}

/// @fn wary_ptr<T>::wary_ptr(std::nullptr_t)
/// @test @b Test_wary_ptr_init.static_cast_nullptr <br>
///       Tests @c static_cast<wary_ptr<int>>(@c nullptr)
TEST_F(Test_wary_ptr_init, static_cast_nullptr)
{
  wary_ptr<int> ptr(static_cast<wary_ptr<int>>(nullptr));

  EXPECT_EQ(nullptr, ptr);
}

/// @fn wary_ptr<T>::wary_ptr()
/// @test @b Test_wary_ptr_init.default_ctor <br>
///       Checks if the default constructor constructs a null pointer which is
///       equal to a raw pointer constructed with its default constructor.
TEST_F(Test_wary_ptr_init, default_ctor)
{
  typedef int* raw_pointer;
  wary_ptr<int> ptr;

  EXPECT_EQ(nullptr, state(ptr).ptr());
  EXPECT_EQ(raw_pointer(), state(ptr).ptr());
}

/// @fn wary_ptr<T>::~wary_ptr
/// @test @b Test_wary_ptr_init.dtor_detects_mem_leak <br>
///       Tests if @c the program is terminated when the last pointer to
///       memory block is destroyed.
TEST_F(Test_wary_ptr_init, dtor_detects_mem_leak)
{
  ASSERT_DEATH(detail::wary_ptr_factory::create_associated_ptr(&value, 1),
               "Memory leak detected");
}

/// @fn detail::wary_ptr_factory::create_associated_ptr
/// @test @b Test_wary_ptr_init.ctor_increments_use_count <br>
///       Checks that use count is incremented after an associated wary_ptr
///       is constructed.
TEST_F(Test_wary_ptr_init, ctor_increments_use_count)
{
  wary_ptr<int> ptr =
    detail::wary_ptr_factory::create_associated_ptr(&value, 1);

  EXPECT_EQ(1, state(ptr).info()->use_count());

  dst::detail::wary_ptr_factory::release(ptr);
}

/// @fn detail::wary_ptr_factory::create_associated_ptr
/// @test @b Test_wary_ptr_init.ptr_properties_after_init <br>
///       Checks if newly created wary_ptr is:
///       * not null
///       * associated
///       * exclusive
///       * not released
///       * not wild
///       * valid
///       * initialized
TEST_F(Test_wary_ptr_init, ptr_properties_after_init)
{
  wary_ptr<int> ptr =
    detail::wary_ptr_factory::create_associated_ptr(&value, 1);

  EXPECT_FALSE(state(ptr).is_null());
  EXPECT_TRUE(state(ptr).is_associated());
  EXPECT_TRUE(state(ptr).is_exclusive());
  EXPECT_FALSE(state(ptr).is_released());
  EXPECT_FALSE(state(ptr).is_wild());
  EXPECT_TRUE(state(ptr).is_valid());
  EXPECT_TRUE(state(ptr).is_initialized());

  detail::wary_ptr_factory::release(ptr);
}

/// @fn wary_ptr<T>::wary_ptr(const detail::wary_ptr_det::wary_ptr_state<T>&)
/// @test @b Test_wary_ptr_init.loose_ptr_properties_after_init <br>
///       Checks if newly created via std::pointer_traits<>::pointer_to loose
///       wary_ptr is:
///       * not null
///       * not associated
///       * not exclusive
///       * not released
///       * not wild
///       * valid
///       * initialized
TEST_F(Test_wary_ptr_init, loose_ptr_properties_after_init)
{
  wary_ptr<int> ptr = std::pointer_traits<wary_ptr<int>>::pointer_to(value);

  EXPECT_FALSE(state(ptr).is_null());
  EXPECT_FALSE(state(ptr).is_associated());
  EXPECT_FALSE(state(ptr).is_exclusive());
  EXPECT_FALSE(state(ptr).is_released());
  EXPECT_FALSE(state(ptr).is_wild());
  EXPECT_TRUE(state(ptr).is_valid());
  EXPECT_TRUE(state(ptr).is_initialized());
}

/// @fn wary_ptr<T>::wary_ptr(std::nullptr_t)
/// @test @b Test_wary_ptr_init.null_ptr_properties_after_init <br>
///       Checks if newly created wary_ptr is:
///       * null
///       * not associated
///       * not exclusive
///       * not released
///       * not wild
///       * not valid
///       * initialized
TEST_F(Test_wary_ptr_init, null_ptr_properties_after_init)
{
  wary_ptr<int> ptr(nullptr);

  EXPECT_TRUE(state(ptr).is_null());
  EXPECT_FALSE(state(ptr).is_associated());
  EXPECT_FALSE(state(ptr).is_exclusive());
  EXPECT_FALSE(state(ptr).is_released());
  EXPECT_FALSE(state(ptr).is_wild());
  EXPECT_FALSE(state(ptr).is_valid());
  EXPECT_TRUE(state(ptr).is_initialized());
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_init.uninitialized <br>
///       Checks if uninitialized wary_ptr can be detected.
TEST_F(Test_wary_ptr_init, uninitialized)
{
  char buffer[sizeof(wary_ptr<int>)] = {0};

  memset(&buffer[0], 0xAB, sizeof(buffer));

  wary_ptr<int>& ptr = reinterpret_cast<wary_ptr<int>&>(*buffer);

  EXPECT_FALSE(state(ptr).is_initialized());

  EXPECT_THROW(*ptr, std::logic_error);
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_init.uninitialized_after_destruction <br>
///       Makes sure that the memory taken by a `wary_ptr` doesn't contain
///       initialization mark after destruction.
TEST_F(Test_wary_ptr_init, uninitialized_after_destruction)
{
  wary_ptr<const int> ptr =
    detail::wary_ptr_factory::create_associated_ptr(&value, 1);

  EXPECT_TRUE(state(ptr).is_initialized());

  neutralize(ptr);

  ptr.~wary_ptr();

  EXPECT_FALSE(state(ptr).is_initialized());
}

/// @fn wary_ptr<T>::operator=(wary_ptr<T>)
/// @test @b Test_wary_ptr_init.uninitialized_assign <br>
///       Checks if uninitialized wary_ptr can be assigned.
TEST_F(Test_wary_ptr_init, uninitialized_assign)
{
  char buffer[sizeof(wary_ptr<int>)] = {0};

  memset(&buffer[0], 0xAB, sizeof(buffer));

  wary_ptr<int>& uninit_ptr = reinterpret_cast<wary_ptr<int>&>(*buffer);

  EXPECT_FALSE(state(uninit_ptr).is_null());
  EXPECT_FALSE(state(uninit_ptr).is_initialized());

  wary_ptr<int> ptr =
    detail::wary_ptr_factory::create_associated_ptr(&value, 1);

  uninit_ptr = ptr;

  EXPECT_FALSE(state(uninit_ptr).is_null());
  EXPECT_TRUE(state(uninit_ptr).is_associated());
  EXPECT_FALSE(state(uninit_ptr).is_exclusive());
  EXPECT_FALSE(state(uninit_ptr).is_released());
  EXPECT_FALSE(state(uninit_ptr).is_wild());
  EXPECT_TRUE(state(uninit_ptr).is_valid());
  EXPECT_TRUE(state(uninit_ptr).is_initialized());

  detail::wary_ptr_factory::release(ptr);
}

/// @class dst::wary_ptr
/// @test @b Test_wary_ptr_init.ptr_to_const <br>
///       Regression test, checks if pointer to const is properly initialized.
TEST_F(Test_wary_ptr_init, ptr_to_const)
{
  wary_ptr<const int> ptr =
    detail::wary_ptr_factory::create_associated_ptr(&value, 1);

  EXPECT_FALSE(state(ptr).is_null());
  EXPECT_TRUE(state(ptr).is_associated());
  EXPECT_TRUE(state(ptr).is_exclusive());
  EXPECT_FALSE(state(ptr).is_released());
  EXPECT_FALSE(state(ptr).is_wild());
  EXPECT_TRUE(state(ptr).is_valid());
  EXPECT_TRUE(state(ptr).is_initialized());

  neutralize(ptr);
}
