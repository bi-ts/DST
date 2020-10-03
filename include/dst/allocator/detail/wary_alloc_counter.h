
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cstddef>   // std::size_t
#include <memory>    // std::shared_ptr, std::make_shared
#include <stdexcept> // std::logic_error

#include "wary_alloc_counter_data.h"

namespace dst
{
namespace detail
{
namespace wary_alloc_det
{

/// @class wary_alloc_counter "detail/wary_alloc_counter.h"
/// A class that gathers and stores statistics of memory allocations done by
/// wary_allocator. Is meant to be used as a counter policy for wary_allocator.
class wary_alloc_counter
{
public:
  /// Constructor.
  wary_alloc_counter();

  /// Total number of allocations.
  std::size_t allocations_count() const;

  /// Number of bytes currently allocated.
  std::size_t allocated_bytes() const;

  /// Memory usage peak.
  std::size_t allocated_bytes_max() const;

  /// Total number of allocated bytes.
  std::size_t allocated_bytes_total() const;

  /// Number of allocated objects of the given type.
  /// @tparam T Type of the object.
  template <typename T> std::size_t allocated_objects() const;

  /// Equality test.
  bool operator==(const wary_alloc_counter& other) const;

  /// Inequality test.
  bool operator!=(const wary_alloc_counter& other) const;

protected:
  /// The method is called by wary_allocator to notify wary_alloc_counter
  /// about the number of newly allocated objects.
  /// @param objects_num Number of the objects that has been allocated.
  /// @tparam T The type of the objects that has been allocated.
  template <typename T> void notify_objects_allocated(std::size_t objects_num);

  /// The method is called by `wary_allocator` to notify `wary_alloc_counter`
  /// about deallocated objects.
  /// @param type_info `type_id` of deallocated objects.
  /// @param objects_num Number of the objects that has been deallocated.
  /// @param object_size Number of bytes single object takes.
  /// @pre Can be called only after notification about corresponding
  ///      memory allocation.
  /// Throws std::logic_error thrown in case of detected misuse.
  void notify_objects_deallocated(const std::type_info& type_info,
                                  std::size_t objects_num,
                                  std::size_t object_size);

private:
  std::shared_ptr<wary_alloc_counter_data> p_data_;
};

inline wary_alloc_counter::wary_alloc_counter()
: p_data_(std::make_shared<wary_alloc_counter_data>())
{
}

inline std::size_t wary_alloc_counter::allocations_count() const
{
  return p_data_->allocations_count;
}

inline std::size_t wary_alloc_counter::allocated_bytes() const
{
  return p_data_->allocated_bytes;
}

inline std::size_t wary_alloc_counter::allocated_bytes_max() const
{
  return p_data_->allocated_bytes_max;
}

inline std::size_t wary_alloc_counter::allocated_bytes_total() const
{
  return p_data_->allocated_bytes_total;
}

template <typename T> std::size_t wary_alloc_counter::allocated_objects() const
{
  if (p_data_->objects_count.find(typeid(T)) == p_data_->objects_count.end())
    return 0;

  return p_data_->objects_count[typeid(T)];
}

inline bool wary_alloc_counter::
operator==(const wary_alloc_counter& other) const
{
  return p_data_ == other.p_data_;
}

inline bool wary_alloc_counter::
operator!=(const wary_alloc_counter& other) const
{
  return !(*this == other);
}

template <typename T>
void wary_alloc_counter::notify_objects_allocated(std::size_t objects_num)
{
  ++p_data_->allocations_count;
  p_data_->allocated_bytes += sizeof(T) * objects_num;

  if (p_data_->allocated_bytes_max < p_data_->allocated_bytes)
    p_data_->allocated_bytes_max = p_data_->allocated_bytes;

  p_data_->allocated_bytes_total += sizeof(T) * objects_num;

  if (p_data_->objects_count.find(typeid(T)) == p_data_->objects_count.end())
    p_data_->objects_count[typeid(T)] = 0;

  p_data_->objects_count[typeid(T)] += objects_num;
}

inline void
wary_alloc_counter::notify_objects_deallocated(const std::type_info& type_info,
                                               std::size_t objects_num,
                                               std::size_t object_size)
{
  if (objects_num == 0)
    throw std::logic_error("objects_num can't be 0");

  if (object_size == 0)
    throw std::logic_error("object_size can't be 0");

  if (p_data_->objects_count.find(type_info) == p_data_->objects_count.end())
    throw std::logic_error("memory misuse");

  if (p_data_->objects_count[type_info] == 0)
    throw std::logic_error("memory misuse");

  if (p_data_->allocated_bytes < object_size * objects_num)
    throw std::logic_error("memory misuse");

  p_data_->allocated_bytes -= object_size * objects_num;

  p_data_->objects_count[type_info] -= objects_num;
}

}
}
} // dst::detail::wary_alloc_det
