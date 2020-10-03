
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/allocator/wary_ptr.h>

namespace dst
{
namespace detail
{

/// @class wary_ptr_factory "detail/wary_ptr_factory.h"
/// A factory for wary_ptr's.
/// @note wary_ptr_factory is a wary_ptr's friend.
///       It creates instances of wary_ptr's using its protected constructors.
class wary_ptr_factory final
{
public:
  /// Creates an associated wary_ptr.
  /// @copydetails wary_ptr::wary_ptr(T*, std::size_t elements_num)
  template <typename T>
  static wary_ptr<T> create_associated_ptr(T* p_array, std::size_t elements_num)
  {
    return dst::wary_ptr<T>(p_array, elements_num);
  }

  /// @copydoc wary_ptr::release()
  template <typename T>
  static const detail::wary_ptr_det::wary_ptr_state<T>&
  release(const wary_ptr<T>& ptr)
  {
    return ptr.release();
  }
};

}
} // dst::detail
