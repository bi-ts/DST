
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "wary_ptr_state.h"

#include <cstddef>     // std::ptrdiff_t
#include <iterator>    // std::iterator, std::random_access_iterator_tag
#include <type_traits> // std::is_void, std::true_type

namespace dst
{
namespace detail
{
namespace wary_ptr_det
{

/// @class wary_ptr_base "detail/wary_ptr_base.h"
/// Base class for wary_ptr.
/// The main purpose of this class is to provide different set of operations
/// depending on whether the type, this pointer points to, is void (possibly
/// cv-qualified) or not.
/// @see wary_ptr_base<T, true>
/// @see wary_ptr_base<T, false>
template <typename T, bool is_void = std::is_void<T>::value>
class wary_ptr_base;

/// @class wary_ptr_base<T, false> "detail/wary_ptr_base.h"
/// A specialization of wary_ptr_base class template for non-void type.
/// Implements a sets of operations which are not available for pointers to
/// void (possibly cv-qualified).
///
/// It contains operations with T& return type. The C++ standard says, that
/// <blockquote>
/// a declarator that specifies the type "reference to cv void" is ill-formed.
/// [$8.3.2-1]
/// </blockquote>
/// Thus, these operations can be declared only for non-void types.

template <typename T>
class wary_ptr_base<T, false>
: public std::iterator<std::random_access_iterator_tag, T>
{
public:
  /// Dereference operator.
  /// @returns A reference to the object the pointer points to.
  /// @pre The pointer is valid (see wary_ptr_state::is_valid()).
  /// @throws std::logic_error
  ///         thrown in case of pre-conditions violation.
  T& operator*() const;

  /// Subscript operator.
  /// @pre The pointer, shifted by the index provided, is valid
  /// (see wary_ptr_state::is_valid()).
  /// @throws std::logic_error
  ///         thrown in case of pre-conditions violation.
  T& operator[](std::ptrdiff_t index) const;

protected:
  /// Constructor.
  wary_ptr_base(const wary_ptr_state<T>& state);

protected:
  /// A field that retains pointer's state.
  wary_ptr_state<T> state_;
};

template <typename T>
wary_ptr_base<T, false>::wary_ptr_base(const wary_ptr_state<T>& state)
: state_(state)
{
}

template <typename T> T& wary_ptr_base<T, false>::operator*() const
{
  if (!state_.is_valid())
    throw std::logic_error("invalid pointer dereference");

  return *state_.ptr();
}

template <typename T>
T& wary_ptr_base<T, false>::operator[](std::ptrdiff_t index) const
{
  wary_ptr_base<T> tmp_ptr(*this);
  tmp_ptr.state_.shift(index);
  return *tmp_ptr;
}

/// @class wary_ptr_base<T, true> "detail/wary_ptr_base.h"
/// A specialization of wary_ptr_base class template for void type (possibly
/// cv-qualified).
/// Provides minimal implementation, no additional operations.
/// @see wary_ptr_base<T, false>
template <typename T> class wary_ptr_base<T, true>
{
protected:
  /// Constructor.
  wary_ptr_base(const wary_ptr_state<T>& state);

protected:
  /// A field that retains pointer's state.
  wary_ptr_state<T> state_;
};

template <typename T>
wary_ptr_base<T, true>::wary_ptr_base(const wary_ptr_state<T>& state)
: state_(state)
{
}

}
}
} // dst::detail::wary_ptr_det
