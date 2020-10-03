
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "wary_ptr.h"

#include "detail/wary_alloc_counter.h"
#include "detail/wary_ptr_factory.h"

#include <cstddef> // std::size_t

namespace dst
{

/// @class wary_allocator dst/allocator/wary_allocator.h
/// Memory allocator which uses wary_ptr to keep track of allocated memory.
/// @todo add tests documentation.
/// @warning Known issue: wary_allocator cannot be used as memory allocator
///          for std::vector and std::list implementation in Xcode (since
///          version 6.3.1).
template <typename T,
          typename counter_policy = detail::wary_alloc_det::wary_alloc_counter>
class wary_allocator : public counter_policy
{
public:
  /// The type to represent the size of any object.
  typedef std::size_t size_type;

  /// Pointer type.
  typedef wary_ptr<T> pointer;

  /// Value type.
  typedef T value_type;

  /// Constructor.
  wary_allocator();

  /// Copy constructor.
  wary_allocator(const wary_allocator&);

  /// Converting constructor.
  template <typename U>
  wary_allocator(const wary_allocator<U, counter_policy>&);

  /// Destructor.
  ~wary_allocator();

  /// Allocates uninitialized block of memory for n objects.
  /// @note The whole allocated block is filled with 0xB0 bytes so it is
  ///       really @a uninitialized. 0xB0 bytes do not have any secret
  ///       meaning, its main purpose is to emitate dirty memory.
  pointer allocate(size_type n, wary_ptr<const void> = nullptr);

  /// Deallocates memory block.
  /// @pre The pointer is associated with a memory block and points to its
  ///      beginning.
  /// @pre The pointer has not been previously released.
  /// @throws std::logic_error
  ///         thrown in case of pre-conditions violation.
  /// @note No destructors are called!
  /// @todo documentation
  void deallocate(const pointer& ptr, size_type elements_num);

private:
  void operator=(const wary_allocator&);
};

template <typename T, typename counter_policy>
wary_allocator<T, counter_policy>::wary_allocator()
: counter_policy()
{
}

template <typename T, typename counter_policy>
wary_allocator<T, counter_policy>::wary_allocator(const wary_allocator& other)
: counter_policy(other)
{
}

template <typename T, typename counter_policy>
template <typename U>
wary_allocator<T, counter_policy>::wary_allocator(
  const wary_allocator<U, counter_policy>& other)
: counter_policy(other)
{
}

template <typename T, typename counter_policy>
wary_allocator<T, counter_policy>::~wary_allocator()
{
}

template <typename T, typename counter_policy>
typename wary_allocator<T, counter_policy>::pointer
wary_allocator<T, counter_policy>::allocate(size_type n, wary_ptr<const void>)
{
  void* p_value = ::operator new(n * sizeof(T));

  memset(p_value, 0xB0, n * sizeof(T));

  counter_policy::template notify_objects_allocated<T>(n);

  return detail::wary_ptr_factory::create_associated_ptr(
    static_cast<T*>(p_value), n);
}

template <typename T, typename counter_policy>
void wary_allocator<T, counter_policy>::deallocate(const pointer& ptr,
                                                   size_type elements_num)
{
  auto p_info = ptr.state_.info();

  if (!ptr.state_.is_associated())
    throw std::logic_error("pointer is not associated");

  if (ptr.state_.is_released())
    throw std::logic_error("releasing pointer twice");

  if (elements_num != p_info->elements_num())
    throw std::logic_error("wrong elements_num");

  if (typeid(T) != p_info->element_typeid())
    throw std::logic_error("wrong element type");

  if (static_cast<const void*>(ptr.state_.ptr()) != p_info->begin())
    throw std::logic_error("wrong array pointer");

  auto data = ptr.release();

  counter_policy::notify_objects_deallocated(
    p_info->element_typeid(), p_info->elements_num(), p_info->element_size());

  ::operator delete(static_cast<void*>(data.ptr()));
}

/// Compares two wary_allocator's.
/// If two allocators compare equal, then any object allocated by one of them
/// can be released by another one.
template <typename T, typename U, typename counter_policy>
bool operator==(const wary_allocator<T, counter_policy>& lhs,
                const wary_allocator<U, counter_policy>& rhs)
{
  return static_cast<const counter_policy&>(lhs) ==
         static_cast<const counter_policy&>(rhs);
}

/// Inequality test for two wary_allocator's.
/// If two allocators compare inequal, then none of the objects allocated by
/// one of them can be released by another one.
template <typename T, typename U, typename counter_policy>
bool operator!=(const wary_allocator<T, counter_policy>& lhs,
                const wary_allocator<U, counter_policy>& rhs)
{
  return static_cast<const counter_policy&>(lhs) !=
         static_cast<const counter_policy&>(rhs);
}

} // dst
