
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/counter_allocator.h>
#include <dst/allocator/utility.h>

#include <boost/test/unit_test.hpp>

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

BOOST_AUTO_TEST_SUITE(test_allocator_utility)

BOOST_AUTO_TEST_CASE(test_new_delete_object)
{
  using allocator_type = dst::counter_allocator<int>;

  allocator_type allocator;

  BOOST_TEST(allocator.allocated() == 0);

  const auto p_vector =
    dst::new_object<std::vector<std::string>>(allocator, 3, "42");

  BOOST_TEST(allocator.allocated() != 0);

  BOOST_TEST(p_vector->size() == 3);
  BOOST_TEST(p_vector->at(1) == "42");

  dst::delete_object<std::vector<std::string>>(allocator, p_vector);

  BOOST_TEST(allocator.allocated() == 0);
}

BOOST_AUTO_TEST_CASE(test_throw_in_ctor)
{
  using allocator_type = dst::counter_allocator<int>;

  allocator_type allocator;

  BOOST_TEST(allocator.allocated() == 0);

  bool exception_was_thrown = false;

  try
  {
    dst::new_object<throw_in_ctor>(allocator);
  }
  catch (...)
  {
    exception_was_thrown = true;
  }

  BOOST_TEST(allocator.allocated() == 0);
  BOOST_TEST(exception_was_thrown);
}

BOOST_AUTO_TEST_CASE(test_throw_in_dtor)
{
  using allocator_type = dst::counter_allocator<int>;

  allocator_type allocator;

  BOOST_TEST(allocator.allocated() == 0);

  const auto p_object = dst::new_object<throw_in_dtor>(allocator);

  bool exception_was_thrown = false;
  try
  {
    dst::delete_object<throw_in_dtor>(allocator, p_object);
  }
  catch (...)
  {
    exception_was_thrown = true;
  }

  BOOST_TEST(allocator.allocated() == 0);
  BOOST_TEST(exception_was_thrown);
}

BOOST_AUTO_TEST_SUITE_END()
