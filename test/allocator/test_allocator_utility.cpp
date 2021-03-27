
//          Copyright Maksym V. Bilinets 2015 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/counter_allocator.h>
#include <dst/allocator/utility.h>

#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>

namespace
{
bool g_always_true = true;

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
    if (g_always_true) // Suppress warning (Visual Studio):
                       // "destructor never returns, potential memory leak".
    {
      throw 0;
    }
  }
};

static int g_ctor_dtor_counter = 0;

struct ctor_dtor
{
  ctor_dtor()
  : data(42)
  {
    ++g_ctor_dtor_counter;
  }

  ~ctor_dtor()
  {
    --g_ctor_dtor_counter;
    data = 242; // Warn! GCC may optimize this out in an optimized build.
  }

  int data;
};
}

BOOST_AUTO_TEST_SUITE(test_allocator_utility)

BOOST_AUTO_TEST_CASE(test_construct_destroy)
{
  static_assert(sizeof(int) == sizeof(ctor_dtor), "Incompatible data types");

  int data = 0;

  ctor_dtor* p_obj = reinterpret_cast<ctor_dtor*>(&data);

  BOOST_TEST(g_ctor_dtor_counter == 0);
  BOOST_TEST(p_obj->data == 0);

  dst::memory::construct(std::allocator<int>(), *p_obj);

  BOOST_TEST(g_ctor_dtor_counter == 1);
  BOOST_TEST(p_obj->data == 42);

  dst::memory::destroy(std::allocator<int>(), *p_obj);

  BOOST_TEST(g_ctor_dtor_counter == 0);

#if (!defined(NDEBUG) && (!defined(__GNUC__) || defined(__clang__)))
  BOOST_TEST(p_obj->data == 242);
#endif
}

BOOST_AUTO_TEST_CASE(test_allocate_aligned)
{
  dst::counter_allocator<int> allocator;

  for (int i = 1; i <= 255; ++i)
  {
    BOOST_TEST(allocator.allocated() == 0);

    const auto p_data = dst::memory::allocate_aligned(allocator, 1, i);

    BOOST_TEST(allocator.allocated() >= 1u);

    BOOST_TEST(reinterpret_cast<std::intptr_t>(p_data) % i == 0);

    dst::memory::deallocate_aligned(allocator, p_data, 1, i);

    BOOST_TEST(allocator.allocated() == 0);
  }
}

BOOST_AUTO_TEST_CASE(test_new_delete_object)
{
  using allocator_type = dst::counter_allocator<int>;

  allocator_type allocator;

  BOOST_TEST(allocator.allocated() == 0);

  const auto p_vector =
    dst::memory::new_object<std::vector<std::string>>(allocator, 3, "42");

  BOOST_TEST(allocator.allocated() != 0);

  BOOST_TEST(p_vector->size() == 3);
  BOOST_TEST(p_vector->at(1) == "42");

  dst::memory::delete_object<std::vector<std::string>>(allocator, p_vector);

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
    dst::memory::new_object<throw_in_ctor>(allocator);
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

  const auto p_object = dst::memory::new_object<throw_in_dtor>(allocator);

  bool exception_was_thrown = false;
  try
  {
    dst::memory::delete_object<throw_in_dtor>(allocator, p_object);
  }
  catch (...)
  {
    exception_was_thrown = true;
  }

  BOOST_TEST(allocator.allocated() == 0);
  BOOST_TEST(exception_was_thrown);
}

BOOST_AUTO_TEST_SUITE_END()
