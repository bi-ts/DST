
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/global_counter_allocator.h>

#include <boost/test/unit_test.hpp>

#include <list>
#include <memory> // std::allocator_traits

BOOST_AUTO_TEST_SUITE(test_global_counter_allocator)

BOOST_AUTO_TEST_CASE(test_allocate_int)
{
  using allocator_type = dst::global_counter_allocator<int>;

  allocator_type allocator;

  BOOST_TEST(allocator_type::allocated() == 0);

  int* p_int = allocator.allocate(1);

  BOOST_TEST(allocator_type::allocated() == sizeof(int));

  int* p_ints = allocator.allocate(10);

  BOOST_TEST(allocator_type::allocated() == 11 * sizeof(int));

  allocator.deallocate(p_int, 1);

  BOOST_TEST(allocator_type::allocated() == 10 * sizeof(int));

  allocator.deallocate(p_ints, 10);

  BOOST_TEST(allocator_type::allocated() == 0);
}

BOOST_AUTO_TEST_CASE(test_void_allocator)
{
  using int_allocator_type = dst::global_counter_allocator<int>;
  using void_allocator_type = dst::global_counter_allocator<void>;

  int_allocator_type allocator;

  BOOST_TEST(void_allocator_type::allocated() == 0);

  std::allocator_traits<void_allocator_type>::pointer p_void =
    allocator.allocate(3);

  BOOST_TEST(void_allocator_type::allocated() == 3 * sizeof(int));

  allocator.deallocate(
    static_cast<std::allocator_traits<int_allocator_type>::pointer>(p_void), 3);

  BOOST_TEST(void_allocator_type::allocated() == 0);
}

BOOST_AUTO_TEST_SUITE_END()

