
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "mem_block_info.h"

#include <array>   // std::array
#include <cstddef> // std::ptrdiff_t
#include <memory>  // std::shared_ptr

namespace dst
{
namespace detail
{
namespace wary_ptr_det
{

/// @class wary_ptr_state "detail/wary_ptr_state.h"
/// Holds wary_ptr's state.
/// Incapsulates wary_ptr's state and provides a set of utility methods for its
/// management and validation.
/// @note This is merely a data storage, it's up to its user to validate
///       and change its internal data via public interface.
template <typename T> class wary_ptr_state final
{
public:
  /// Constructor.
  /// @param p_value Current position of the pointer.
  /// @param p_mem_block_info Information about memory block to be associated
  ///        with this pointer.
  wary_ptr_state(T* p_value,
                 const std::shared_ptr<mem_block_info>& p_mem_block_info);

  /// Copy constructor.
  wary_ptr_state(const wary_ptr_state<T>& other);

  /// Assignment operator.
  wary_ptr_state<T>& operator=(const wary_ptr_state<T>& other);

  /// Null test.
  /// Checks whether this pointer is null.
  /// @note A pointer can be null and associated with a memory block
  ///       simultaneously. In this case it is obviously wild because a valid
  ///       range of memory addresses can not include null. Also, the pointer
  ///       can be uninitialized.
  bool is_null() const;

  /// Checks whether this pointer is initialized and associated with a memory
  /// block.
  /// @note It doesn't take into account whether it has been released or not.
  bool is_associated() const;

  /// Checks if this pointer has exclusive access to its memory block.
  /// Checks whether this pointer is initialized and associated with a memory
  /// block, which has not been released yet, and no other pointers are
  /// associated with the same memory block.
  bool is_exclusive() const;

  /// Checks if this pointer is initialized and associated with a memory
  /// block, which has been released.
  bool is_released() const;

  /// Checks if this pointer is @a wild.
  /// A pointer is deemed wild or dangling in one of the cases below:
  /// * it is associated with a memory block, which has been released;
  /// * its current position points beyond associated memory block;
  /// * there is not enough space between its current position and the
  ///   end of the block for an object of type T;
  /// * it is uninitialized and not null;
  ///
  /// If a pointer is not associated with a memory block (a loose pointer) it
  /// is not possible to detect whether it is wild or not, so it is deemed
  /// not wild.
  ///
  /// @note A null pointer is not wild if no memory block is associated.
  bool is_wild() const;

  /// Checks whether this pointer can be dereferenced, which means it is
  /// neither null nor wild.
  /// @see is_null()
  /// @see is_wild()
  bool is_valid() const;

  /// Checks if wary_ptr_state has been properly initialized.
  /// Can be used to detect situations when memory for wary_ptr was allocated
  /// without calling any constructor.
  bool is_initialized() const;

  /// Gets internal raw pointer.
  T* ptr() const;

  /// Gets pointer to mem_block_info.
  const std::shared_ptr<mem_block_info>& info() const;

  /// Increments pointer's value by offset (can be negative).
  void shift(std::ptrdiff_t offset);

private:
  static const std::size_t g_init_mark_size_ = 16;
  static std::array<char, g_init_mark_size_> g_init_mark_;
  T* p_value_;
  std::shared_ptr<mem_block_info> p_mem_block_info_;
  std::array<char, g_init_mark_size_> init_mark_;
};

template <typename T>
wary_ptr_state<T>::wary_ptr_state(
  T* p_value, const std::shared_ptr<mem_block_info>& p_mem_block_info)
: p_value_(p_value)
, p_mem_block_info_(p_mem_block_info)
, init_mark_(g_init_mark_)
{
}

template <typename T>
wary_ptr_state<T>::wary_ptr_state(const wary_ptr_state<T>& other)
: p_value_(other.p_value_)
, p_mem_block_info_(other.p_mem_block_info_)
, init_mark_(other.init_mark_)
{
}

template <typename T>
wary_ptr_state<T>& wary_ptr_state<T>::operator=(const wary_ptr_state<T>& other)
{
  p_value_ = other.p_value_;
  p_mem_block_info_ = other.p_mem_block_info_;

  init_mark_ = other.init_mark_;

  return *this;
}

template <typename T> bool wary_ptr_state<T>::is_null() const
{
  return p_value_ == nullptr;
}

template <typename T> bool wary_ptr_state<T>::is_associated() const
{
  return is_initialized() && p_mem_block_info_ != nullptr;
}

template <typename T> bool wary_ptr_state<T>::is_exclusive() const
{
  return is_initialized() && !is_released() && is_associated() &&
         p_mem_block_info_->use_count() == 1;
}

template <typename T> bool wary_ptr_state<T>::is_released() const
{
  return is_initialized() && is_associated() && p_mem_block_info_->released();
}

template <typename T> bool wary_ptr_state<T>::is_wild() const
{
  return is_released() ||
         (is_associated() &&
          (static_cast<const void*>(p_value_) < p_mem_block_info_->begin() ||
           static_cast<const void*>(p_value_ + 1) >
             p_mem_block_info_->end())) ||
         (!is_null() && !is_initialized());
}

template <typename T> bool wary_ptr_state<T>::is_valid() const
{
  return !is_null() && !is_wild();
}

template <typename T> bool wary_ptr_state<T>::is_initialized() const
{
  return init_mark_ == g_init_mark_;
}

template <typename T> T* wary_ptr_state<T>::ptr() const
{
  return p_value_;
}

template <typename T>
const std::shared_ptr<mem_block_info>& wary_ptr_state<T>::info() const
{
  return p_mem_block_info_;
}

template <typename T> void wary_ptr_state<T>::shift(std::ptrdiff_t offset)
{
  p_value_ += offset;
}

template <typename T>
std::array<char, wary_ptr_state<T>::g_init_mark_size_>
  wary_ptr_state<T>::g_init_mark_ = {"==INITIALIZED=="}; // 15 chars + '\0'

}
}
} // dst::detail::wary_ptr_det
