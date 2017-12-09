
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//        (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/utility.h>

#include <boost/test/unit_test.hpp>

#include <type_traits>

BOOST_AUTO_TEST_SUITE(test_utility)

BOOST_AUTO_TEST_CASE(test_ref_or_void)
{
  int value = 0;

  BOOST_TEST(value == 0);

  dst::ref_or_void<int>::type int_ref = value;

  int_ref = 42;

  BOOST_TEST(value == 42);

  BOOST_TEST(std::is_void<dst::ref_or_void<void>::type>::value);
}

BOOST_AUTO_TEST_CASE(test_pair_or_single_is_nothrow_constructible_destructible)
{
  using pair = dst::pair_or_single<int, float>;
  using single = dst::pair_or_single<int, void>;

  BOOST_TEST(std::is_nothrow_default_constructible<pair>::value);
  BOOST_TEST(std::is_nothrow_default_constructible<single>::value);

  BOOST_TEST(std::is_nothrow_destructible<pair>::value);
  BOOST_TEST(std::is_nothrow_destructible<single>::value);
}

BOOST_AUTO_TEST_CASE(test_pair_or_single_is_nothrow_destructible)
{
  struct throwing_ctor
  {
    throwing_ctor() noexcept(false)
    {
    }
  };

  BOOST_TEST(!std::is_nothrow_default_constructible<throwing_ctor>::value);
  BOOST_TEST(std::is_nothrow_destructible<throwing_ctor>::value);

  using pair_1 = dst::pair_or_single<throwing_ctor, float>;
  using pair_2 = dst::pair_or_single<int, throwing_ctor>;
  using single = dst::pair_or_single<throwing_ctor, void>;

  BOOST_TEST(!std::is_nothrow_default_constructible<pair_1>::value);
  BOOST_TEST(!std::is_nothrow_default_constructible<pair_2>::value);
  BOOST_TEST(!std::is_nothrow_default_constructible<single>::value);

  BOOST_TEST(std::is_nothrow_destructible<pair_1>::value);
  BOOST_TEST(std::is_nothrow_destructible<pair_2>::value);
  BOOST_TEST(std::is_nothrow_destructible<single>::value);
}

BOOST_AUTO_TEST_CASE(test_pair_or_single_is_throw_constructible_destructible)
{
  struct throwing_ctor_dtor
  {
    throwing_ctor_dtor() noexcept(false)
    {
    }

    ~throwing_ctor_dtor() noexcept(false)
    {
    }
  };

  BOOST_TEST(!std::is_nothrow_default_constructible<throwing_ctor_dtor>::value);
  BOOST_TEST(!std::is_nothrow_destructible<throwing_ctor_dtor>::value);

  using pair_1 = dst::pair_or_single<throwing_ctor_dtor, float>;
  using pair_2 = dst::pair_or_single<int, throwing_ctor_dtor>;
  using single = dst::pair_or_single<throwing_ctor_dtor, void>;

  BOOST_TEST(!std::is_nothrow_default_constructible<pair_1>::value);
  BOOST_TEST(!std::is_nothrow_default_constructible<pair_2>::value);
  BOOST_TEST(!std::is_nothrow_default_constructible<single>::value);

  BOOST_TEST(!std::is_nothrow_destructible<pair_1>::value);
  BOOST_TEST(!std::is_nothrow_destructible<pair_2>::value);
  BOOST_TEST(!std::is_nothrow_destructible<single>::value);
}

BOOST_AUTO_TEST_SUITE_END()
