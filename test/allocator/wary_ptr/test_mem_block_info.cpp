
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <gtest/gtest.h>

#include <dst/allocator/detail/mem_block_info.h>

using namespace dst::detail::wary_ptr_det;

/// @fn mem_block_info::create(T*, std::size_t)
/// @test @b Test_mem_block_info.ctor_arg_check_null <br>
///       Checks if the constructor throws std::invalid_argument when
///       p_array is null.
TEST(Test_mem_block_info, ctor_arg_check_null)
{
  EXPECT_THROW(mem_block_info::create(static_cast<int*>(nullptr), 1),
               std::invalid_argument);
  EXPECT_THROW(mem_block_info::create(static_cast<int*>(nullptr), 1),
               std::invalid_argument);
  // EXPECT_DEATH(mem_block_info::create(static_cast<int*>(nullptr), 1),
  //"Assertion failed: (p_array != nullptr)*");
}

/// @fn mem_block_info::create(T*, std::size_t)
/// @test @b Test_mem_block_info.ctor_arg_check_empty_array <br>
///       Checks if the constructor throws std::invalid_argument when
///       elements_num is zero.
TEST(Test_mem_block_info, ctor_arg_check_empty_array)
{
  int value = 0;

  EXPECT_THROW(mem_block_info::create(&value, 0), std::invalid_argument);
}

/// @class dst::detail::wary_ptr_det::mem_block_info
/// @test @b Test_mem_block_info.public_interface <br>
///       Verifies public interface all at once:
///       * use_count()
///       * incr_use_count()
///       * decr_use_count()
///       * begin()
///       * end()
TEST(Test_mem_block_info, public_interface)
{
  int values[2];
  std::shared_ptr<mem_block_info> p_info = mem_block_info::create(values, 1);

  EXPECT_EQ(0, p_info->use_count());

  p_info->incr_use_count();

  EXPECT_EQ(1, p_info->use_count());

  p_info->incr_use_count();

  EXPECT_EQ(2, p_info->use_count());

  p_info->decr_use_count();

  EXPECT_EQ(1, p_info->use_count());

  EXPECT_EQ(static_cast<void*>(&values[0]), p_info->begin());

  EXPECT_EQ(static_cast<void*>(&values[1]), p_info->end());
}
