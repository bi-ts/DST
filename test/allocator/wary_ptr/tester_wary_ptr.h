
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/allocator/wary_ptr.h>

namespace dst
{
namespace test
{
/// @cond
class Tester_wary_ptr
{
protected:
  Tester_wary_ptr()
  {
  }

protected:
  template <typename T>
  const dst::detail::wary_ptr_det::wary_ptr_state<T>&
  state(const dst::wary_ptr<T>& ptr)
  {
    return ptr.state_;
  }

  template <typename T> void neutralize(const dst::wary_ptr<T>& ptr)
  {
    if (ptr.state_.is_associated())
      ptr.state_.info()->release();
  }
};
/// @endcond

}
}
