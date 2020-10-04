
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/wary_allocator.h>

#include <gtest/gtest.h>

#include <cstdint> // std::int64_t
#include <list>    // std::list
#include <map>     // std::map
#include <vector>  // std::vector

using dst::wary_allocator;
using dst::wary_ptr;

namespace
{
std::size_t g_ctor_calls = 0;
std::size_t g_dtor_calls = 0;

class Base
{
public:
  virtual ~Base()
  {
  }
};

class Obj : public Base
{
public:
  Obj(int val)
  : Base()
  , value(val)
  {
    ++g_ctor_calls;
  }

  virtual ~Obj()
  {
    value = 0;
    ++g_dtor_calls;
  }

  int value;
};

class Test_wary_allocator : public ::testing::Test, public wary_allocator<Obj>
{
public:
  Test_wary_allocator()
  {
    g_ctor_calls = 0;
    g_dtor_calls = 0;
  }
};
}

TEST_F(Test_wary_allocator, equality_test)
{
  wary_allocator<Obj> allocator_another;
  wary_allocator<Obj> allocator_copy = *this;

  EXPECT_FALSE(allocator_another == *this);
  EXPECT_TRUE(allocator_copy == *this);
}

TEST_F(Test_wary_allocator, allocate)
{
  EXPECT_EQ(0, allocated_bytes());
  EXPECT_EQ(0, allocated_objects<Obj>());

  wary_allocator<Obj>::pointer ptr = allocate(1);

  EXPECT_EQ(sizeof(Obj), allocated_bytes());
  EXPECT_EQ(1, allocated_objects<Obj>());

  EXPECT_EQ(0, g_ctor_calls);

  ptr->value = 30;

  EXPECT_EQ(30, ptr->value);

  deallocate(ptr, 1);

  EXPECT_EQ(0, allocated_bytes());
  EXPECT_EQ(0, allocated_objects<Obj>());

  EXPECT_EQ(0, g_dtor_calls);
}

TEST_F(Test_wary_allocator, deallocate_wrong_size)
{
  wary_allocator<Obj>::pointer ptr = allocate(10);

  EXPECT_EQ(10 * sizeof(Obj), allocated_bytes());

  EXPECT_THROW(deallocate(ptr, 1), std::logic_error);

  EXPECT_EQ(10 * sizeof(Obj), allocated_bytes());

  deallocate(ptr, 10);

  EXPECT_EQ(0, allocated_bytes());
}

TEST_F(Test_wary_allocator, construct)
{
  wary_allocator<Obj>::pointer ptr = allocate(1);

  EXPECT_EQ(0, g_ctor_calls);

  std::allocator_traits<wary_allocator<Obj>>::construct(
    *this, std::addressof(*ptr), 13);

  EXPECT_EQ(13, ptr->value);
  EXPECT_EQ(1, g_ctor_calls);
  EXPECT_EQ(0, g_dtor_calls);

  deallocate(ptr, 1);
}

TEST_F(Test_wary_allocator, destroy)
{
  wary_allocator<Obj>::pointer ptr = allocate(1);

  EXPECT_EQ(0, g_ctor_calls);
  EXPECT_EQ(0, g_dtor_calls);

  std::allocator_traits<wary_allocator<Obj>>::construct(
    *this, std::addressof(*ptr), 13);

  EXPECT_EQ(1, g_ctor_calls);

  std::allocator_traits<wary_allocator<Obj>>::destroy(*this,
                                                      std::addressof(*ptr));

  EXPECT_EQ(1, g_dtor_calls);

  deallocate(ptr, 1);
}

TEST_F(Test_wary_allocator, polymorphic_objects)
{
  wary_allocator<Obj>::pointer ptr = allocate(1);

  EXPECT_EQ(1, allocated_objects<Obj>());
  EXPECT_EQ(sizeof(Obj), allocated_bytes());

  wary_allocator<Base> base_allocator(*this);
  wary_allocator<Base>::pointer ptr_base = ptr;

  EXPECT_THROW(base_allocator.deallocate(ptr_base, 1), std::logic_error);

  deallocate(ptr, 1);

  EXPECT_EQ(0, allocated_objects<Obj>());
  EXPECT_EQ(0, allocated_bytes());
}

/*
// Disabled these test because it fails under Xcode 6.3.1 because of
// operator-> call on and invalid pointer (__to_raw_pointer).
TEST_F(Test_wary_allocator, list_with_wary_allocator)
{
  typedef wary_allocator<std::int64_t> alloc_type;

  alloc_type alloc;

  {
    std::list<std::int64_t, wary_allocator<std::int64_t>> list(alloc);

    const std::size_t n = list.get_allocator().allocated_bytes();

    list.push_back(13);

    EXPECT_EQ(0, list.get_allocator().allocated_objects<std::int64_t>());

    EXPECT_EQ(n + sizeof(std::int64_t) + 2 * sizeof(wary_ptr<void>),
              list.get_allocator().allocated_bytes());

    list.clear();
  }

  EXPECT_EQ(0, alloc.allocated_bytes());
}

// This one doesn't compile in Visual Studio
TEST_F(Test_wary_allocator, map_with_wary_allocator)
{
  typedef wary_allocator<std::pair<const std::int64_t, std::int64_t>>
    alloc_type;

  alloc_type alloc;

  {
    std::map<std::int64_t, std::int64_t, std::less<std::int64_t>, alloc_type>
      map(alloc);

    map[0] = 13;

    map.clear();
  }

  EXPECT_EQ(0, alloc.allocated_bytes());
}
*/

#ifdef _LIBCPP_VERSION // We use libc++
TEST_F(Test_wary_allocator, vector_with_wary_allocator)
{
  typedef wary_allocator<std::int64_t> alloc_type;

  alloc_type alloc;

  {
    typedef std::vector<std::int64_t, alloc_type> vec_type;
    vec_type vec(alloc);

    vec.push_back(13);

    EXPECT_EQ(1, alloc.allocated_objects<std::int64_t>());

    EXPECT_EQ(sizeof(std::int64_t), alloc.allocated_bytes());

    vec.clear();

    EXPECT_EQ(0, vec.size());

    EXPECT_EQ(sizeof(std::int64_t), alloc.allocated_bytes());
  }

  EXPECT_EQ(0, alloc.allocated_bytes());
}
#endif
