
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cassert>   // assert
#include <cstddef>   // std::size_t
#include <map>       // std::map
#include <typeindex> // std::type_index
#include <typeinfo>  // std::type_info

namespace dst
{
namespace detail
{
namespace wary_alloc_det
{

/// @class wary_alloc_counter_data "detail/wary_alloc_counter_data.h"
/// A class that stores statistics of memory allocations done by
/// wary_allocator. Is used by wary_alloc_counter.
class wary_alloc_counter_data final
{
public:
  /// Constructor.
  wary_alloc_counter_data();

  /// Destructor.
  ~wary_alloc_counter_data();

  /// Total number of allocations.
  std::size_t allocations_count;

  /// Number of bytes currently allocated.
  std::size_t allocated_bytes;

  /// Memory usage peak.
  std::size_t allocated_bytes_max;

  /// Total number of allocated bytes.
  std::size_t allocated_bytes_total;

  /// Number of currently allocated objects of each type.
  std::map<std::type_index, std::size_t> objects_count;
};

inline wary_alloc_counter_data::wary_alloc_counter_data()
: allocations_count(0)
, allocated_bytes(0)
, allocated_bytes_max(0)
, allocated_bytes_total(0)
{
}

inline wary_alloc_counter_data::~wary_alloc_counter_data()
{
  assert(allocated_bytes == 0);
}

}
}
}
