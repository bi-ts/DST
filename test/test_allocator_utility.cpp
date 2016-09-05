
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/counter_allocator.h>
#include <dst/allocator/utility.h>

#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace
{
  struct throw_in_ctor
  {
    throw_in_ctor()
    {
      throw 0;
    }
  };

  struct throw_in_dtor
  {
    ~throw_in_dtor() noexcept(false)
    {
      throw 0;
    }
  };
}

TEST(test_allocator_utility, new_delete_object)
{
  using allocator_type = dst::counter_allocator<int>;

  allocator_type allocator;

  EXPECT_EQ(0, allocator.allocated());

  const auto p_vector =
    dst::new_object<std::vector<std::string>>(allocator, 3, "42");

  EXPECT_NE(0, allocator.allocated());

  EXPECT_EQ(3, p_vector->size());
  EXPECT_EQ("42", p_vector->at(1));

  dst::delete_object<std::vector<std::string>>(allocator, p_vector);

  EXPECT_EQ(0, allocator.allocated());
}

TEST(test_allocator_utility, throw_in_ctor)
{
  using allocator_type = dst::counter_allocator<int>;

  allocator_type allocator;

  EXPECT_EQ(0, allocator.allocated());

  try
  {
    const auto p_object =
      dst::new_object<throw_in_ctor>(allocator);
  }
  catch(...)
  {
    EXPECT_EQ(0, allocator.allocated());
    return;
  }

  EXPECT_FALSE("Exception was not thrown");
}

TEST(test_allocator_utility, throw_in_dtor)
{
  using allocator_type = dst::counter_allocator<int>;

  allocator_type allocator;

  EXPECT_EQ(0, allocator.allocated());

  const auto p_object =
    dst::new_object<throw_in_dtor>(allocator);

  try
  {
    dst::delete_object<throw_in_dtor>(allocator, p_object);
  }
  catch(...)
  {
    EXPECT_EQ(0, allocator.allocated());
    return;
  }

  EXPECT_FALSE("Exception was not thrown");
}
