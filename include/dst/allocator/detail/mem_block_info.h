
//          Copyright Maksym V. Bilinets 2015 - 2021.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cstddef>   // std::size_t
#include <memory>    // std::enable_shared_from_this, std::shared_ptr
#include <stdexcept> // std::invalid_argument
#include <typeinfo>  // std::type_info

namespace dst
{
namespace detail
{
namespace wary_ptr_det
{

/// @class mem_block_info "detail/mem_block_info.h"
/// A class that stores information about a dynamicaly allocated memory block.
class mem_block_info final : public std::enable_shared_from_this<mem_block_info>
{
public:
  /// `mem_block_info` factory.
  /// Constructs a `mem_block_info` from an array of elements of any type.
  /// @tparam T Type of elements in the allocated array.
  /// @param p_array Pointer to an array of elements of type T.
  ///        The array must be valid, nullptr is not accepted.
  /// @param elements_num Number of elements in the array.
  ///        Zero-length array is not accepted.
  /// @returns A shared_ptr to newly created `mem_block_info`.
  /// @throws std::invalid_argument Thrown if argument's requirements are not
  ///         satisfied.
  template <typename T>
  static std::shared_ptr<mem_block_info> create(const T* p_array,
                                                std::size_t elements_num);

  /// Gets number of references on this memory block.
  std::size_t use_count() const;

  /// Increments use count
  void incr_use_count();

  /// Decrements use count
  void decr_use_count();

  /// Gets pointer to the first byte of the block.
  const void* begin() const;

  /// Gets pointer to the first byte after the end of the block.
  const void* end() const;

  /// Size of single allocated element.
  std::size_t element_size() const;

  /// @c typeid of allocated element.
  const std::type_info& element_typeid() const;

  /// Number of allocated elements in the memory block.
  std::size_t elements_num() const;

  /// Checks if this memory block has been released.
  bool released() const;

  /// Marks the memory block as released.
  void release();

private:
  template <typename T>
  mem_block_info(const T* p_array, std::size_t elements_num);

private:
  const char* const p_begin_;
  const std::type_info& type_info_;
  const std::size_t element_size_;
  const std::size_t elements_num_;
  std::size_t use_count_;
  bool released_;
};

template <typename T>
std::shared_ptr<mem_block_info> mem_block_info::create(const T* p_array,
                                                       std::size_t elements_num)
{
  return std::shared_ptr<mem_block_info>(
    new mem_block_info(p_array, elements_num));
}

template <typename T>
mem_block_info::mem_block_info(const T* p_array, std::size_t elements_num)
: p_begin_(reinterpret_cast<const char*>(p_array))
, type_info_(typeid(T))
, element_size_(sizeof(T))
, elements_num_(elements_num)
, use_count_(0)
, released_(false)
{
  if (p_array == nullptr)
    throw std::invalid_argument("p_array can't be null");

  if (elements_num == 0)
    throw std::invalid_argument("elements_num  can't be 0");
}

inline std::size_t mem_block_info::use_count() const
{
  // return shared_from_this().use_count();
  return use_count_;
}

inline void mem_block_info::incr_use_count()
{
  ++use_count_;
}

inline void mem_block_info::decr_use_count()
{
  --use_count_;
}

inline const void* mem_block_info::begin() const
{
  return static_cast<const void*>(p_begin_);
}

inline const void* mem_block_info::end() const
{
  return static_cast<const void*>(p_begin_ + element_size_ * elements_num_);
}

inline std::size_t mem_block_info::element_size() const
{
  return element_size_;
}

inline const std::type_info& mem_block_info::element_typeid() const
{
  return type_info_;
}

inline std::size_t mem_block_info::elements_num() const
{
  return elements_num_;
}

inline bool mem_block_info::released() const
{
  return released_;
}

inline void mem_block_info::release()
{
  released_ = true;
}

} // wary_ptr_det
} // detail
} // dst
