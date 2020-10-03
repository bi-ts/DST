
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/detail/wary_ptr_factory.h>
#include <dst/allocator/wary_ptr.h>

#include "tester_wary_ptr.h"

#include <gtest/gtest.h>

#include <cstdint>

using namespace dst;

namespace
{
class Test_wary_ptr_obj_ptr : public ::testing::Test,
                              public dst::test::Tester_wary_ptr
{
public:
  struct ObjBase1
  {
    std::int64_t field_1;
  };

  struct ObjBase2
  {
    std::int64_t field_2;
  };

  struct Obj : public ObjBase1, public ObjBase2
  {
    std::int64_t field_3;
  };

  struct OtherObj
  {
    OtherObj(const wary_ptr<ObjBase1>& ptr)
    : p_base(ptr)
    {
    }

    const wary_ptr<ObjBase1> p_base;
  };

public:
  Test_wary_ptr_obj_ptr()
  : value()
  , ptr(detail::wary_ptr_factory::create_associated_ptr(&value, 1))
  {
    value.field_1 = 0;
    value.field_2 = 0;
    value.field_3 = 0;
  }

  ~Test_wary_ptr_obj_ptr() noexcept(true)
  {
    neutralize(ptr);
  }

  Obj value;
  dst::wary_ptr<Obj> ptr;
};
}

/// @fn dst::wary_ptr<T>::operator->()
/// @test @b Test_wary_ptr_obj_ptr.indirect_member_selector <br>
///          Tests object's field modification via member selector operator.
TEST_F(Test_wary_ptr_obj_ptr, indirect_member_selector)
{
  ptr->field_1 = 841;

  EXPECT_EQ(841, ptr->field_1);
}

/// @fn wary_ptr<T>::wary_ptr(const wary_ptr<U>& other)
/// @test @b Test_wary_ptr_obj_ptr.wary_ptr_implicit_cast <br>
///       Tests if a wary_ptr to a class can be implicitly casted to both
///       its base classes, use count is incremented properly, base class
///       fields can be midified via pointer to base class.
TEST_F(Test_wary_ptr_obj_ptr, wary_ptr_implicit_cast)
{
  using dst::wary_ptr;

  wary_ptr<ObjBase1> ptr_base_1 = nullptr;

  ptr_base_1 = ptr;

  EXPECT_TRUE(ptr == ptr_base_1);

  EXPECT_EQ(2, state(ptr).info()->use_count());

  wary_ptr<ObjBase2> ptr_base_2 = nullptr;
  ptr_base_2 = ptr;

  EXPECT_TRUE(ptr == ptr_base_2);

  EXPECT_EQ(3, state(ptr).info()->use_count());

  ptr_base_2->field_2 = 28;

  EXPECT_EQ(28, value.field_2);
}

/// @fn wary_ptr<T>::wary_ptr(wary_ptr<U>&& other)
/// @test @b Test_wary_ptr_obj_ptr.implicit_cast_rvalue <br>
///       Tests if a rvalue wary_ptr can be implicitly casted to a pointer
///       to its base class.
TEST_F(Test_wary_ptr_obj_ptr, implicit_cast_rvalue)
{
  using dst::wary_ptr;

  const auto ptr_copy = ptr;

  OtherObj obj(ptr_copy);

  EXPECT_TRUE(obj.p_base == ptr);
}

/// @fn wary_ptr<T>::operator==(const wary_ptr<U>& other) const
/// @test @b Test_wary_ptr_obj_ptr.compare_with_base <br>
///       Tests if pointers to both base classes compare equal to the derived
///       class.
TEST_F(Test_wary_ptr_obj_ptr, compare_with_base)
{
  using dst::wary_ptr;

  wary_ptr<ObjBase1> ptr_base_1 = static_cast<wary_ptr<ObjBase1>>(ptr);

  EXPECT_TRUE(ptr == ptr_base_1);

  wary_ptr<ObjBase2> ptr_base_2 = static_cast<wary_ptr<ObjBase2>>(ptr);

  EXPECT_TRUE(ptr == ptr_base_2);
}

/// @fn wary_ptr<T>::down_cast(const dst::wary_ptr<S>&)
/// @test @b Test_wary_ptr_obj_ptr.invalid_down_cast <br>
///       Tests if invalid downcasting throws an exception.
TEST_F(Test_wary_ptr_obj_ptr, invalid_down_cast)
{
  using dst::wary_ptr;

  ObjBase1 obj_base_1;

  wary_ptr<ObjBase1> p_base(
    detail::wary_ptr_factory::create_associated_ptr(&obj_base_1, 1));

  EXPECT_THROW(down_cast<wary_ptr<Obj>>(p_base), std::logic_error);

  neutralize(p_base);
}

/// @fn wary_ptr<T>::down_cast(const dst::wary_ptr<S>&)
/// @test @b Test_wary_ptr_obj_ptr.valid_down_cast <br>
///       Tests valid downcasting.
TEST_F(Test_wary_ptr_obj_ptr, valid_down_cast)
{
  using dst::wary_ptr;

  wary_ptr<ObjBase1> p_base = ptr;

  EXPECT_TRUE(ptr == down_cast<wary_ptr<Obj>>(p_base));
}
