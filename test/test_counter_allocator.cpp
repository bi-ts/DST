
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/counter_allocator.h>

#include <boost/test/unit_test.hpp>

#include <list>
#include <memory> // std::allocator_traits

BOOST_AUTO_TEST_SUITE(test_counter_allocator)

BOOST_AUTO_TEST_CASE(test_allocate_int)
{
  dst::counter_allocator<int> allocator;

  BOOST_TEST(allocator.allocated() == 0);

  int* p_int = allocator.allocate(1);

  BOOST_TEST(allocator.allocated() == sizeof(int));

  int* p_ints = allocator.allocate(10);

  BOOST_TEST(allocator.allocated() == 11 * sizeof(int));

  allocator.deallocate(p_int, 1);

  BOOST_TEST(allocator.allocated() == 10 * sizeof(int));

  allocator.deallocate(p_ints, 10);

  BOOST_TEST(allocator.allocated() == 0);
}

BOOST_AUTO_TEST_CASE(test_rebind)
{
  dst::counter_allocator<int> int_allocator;

  std::allocator_traits<dst::counter_allocator<int>>::rebind_alloc<double>
    double_allocator(int_allocator);

  BOOST_TEST(int_allocator.allocated() == 0);
  BOOST_TEST(double_allocator.allocated() == 0);

  int* p_int = int_allocator.allocate(1);

  BOOST_TEST(int_allocator.allocated() == sizeof(int));
  BOOST_TEST(double_allocator.allocated() == sizeof(int));

  double* p_double = double_allocator.allocate(2);

  BOOST_TEST(int_allocator.allocated() == 2 * sizeof(double) + sizeof(int));
  BOOST_TEST(double_allocator.allocated() == 2 * sizeof(double) + sizeof(int));

  int_allocator.deallocate(p_int, 1);

  BOOST_TEST(int_allocator.allocated() == 2 * sizeof(double));
  BOOST_TEST(double_allocator.allocated() == 2 * sizeof(double));

  double_allocator.deallocate(p_double, 2);

  BOOST_TEST(int_allocator.allocated() == 0);
  BOOST_TEST(double_allocator.allocated() == 0);
}

BOOST_AUTO_TEST_CASE(test_with_std_list)
{
  dst::counter_allocator<int> allocator;

  BOOST_TEST(allocator.allocated() == 0);

  {
    std::list<int, dst::counter_allocator<int>> l(allocator);

    l.assign({1, 2, 3, 4, 5, 6, 7, 8, 9, 0});

    BOOST_TEST(allocator.allocated() != 0);
  }

  BOOST_TEST(allocator.allocated() == 0);
}

BOOST_AUTO_TEST_CASE(test_base_allocator)
{
  dst::counter_allocator<int> base_allocator;
  dst::counter_allocator<int, dst::counter_allocator<int>> allocator(
    base_allocator);

  BOOST_TEST((allocator.base() == base_allocator));
  BOOST_TEST((allocator.base() != dst::counter_allocator<int>()));

  BOOST_TEST(base_allocator.allocated() == 0);
  BOOST_TEST(allocator.allocated() == 0);

  int* p_ints = allocator.allocate(10);

  BOOST_TEST(base_allocator.allocated() == 10 * sizeof(int));
  BOOST_TEST(allocator.allocated() == 10 * sizeof(int));

  allocator.deallocate(p_ints, 10);

  BOOST_TEST(base_allocator.allocated() == 0);
  BOOST_TEST(allocator.allocated() == 0);
}

BOOST_AUTO_TEST_CASE(test_void_allocator)
{
  dst::counter_allocator<int> allocator;

  BOOST_TEST(allocator.allocated() == 0);

  std::allocator_traits<dst::counter_allocator<void>>::pointer p_void =
    allocator.allocate(2);

  BOOST_TEST(allocator.allocated() == 2 * sizeof(int));

  allocator.deallocate(
    static_cast<std::allocator_traits<dst::counter_allocator<int>>::pointer>(
      p_void),
    2);

  BOOST_TEST(allocator.allocated() == 0);
}

BOOST_AUTO_TEST_SUITE_END()

