
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "detail/mem_block_info.h"
#include "detail/wary_ptr_base.h"

#include <cassert>     // assert
#include <cstddef>     // std::ptrdiff_t, std::nullptr_t
#include <iostream>    // std::cerr
#include <memory>      // std::addressof, std::pointer_traits
#include <type_traits> // std::is_void, std::conditional, std::enable_if,
                       // std::is_convertible

namespace dst
{

namespace test
{
class Tester_wary_ptr;
}
namespace detail
{
class wary_ptr_factory;
}

/// @class wary_ptr dst/allocator/wary_ptr.h
/// A smart pointer for memory usage diagnositics.
/// The behaviour of wary_ptr tries to stay as close as possible to that of
/// raw pointer.
///
/// @note The pointer in no case takes ownership of the memory it points to.
/// It only observes and does not interfere until a dangerous situation is
/// detected. In this case an exception will be thrown or the application will
/// be terminated (via `std::abort()`).
///
/// @warning This pointer is not suitable for anything except testing because
///          it may terminate the application (see ~wary_ptr()).
///
/// Depending on the amount of information it has about the memory block it
/// points to, each wary_ptr falls into one of the following categories:
/// * @a associated - it is given all necessary information about the memory
///   block it points into, such as:
///   * use count;
///   * address range this memory block takes;
/// * @a loose - it stores a pointer to an object it knows nothing about.
///
/// At the same time, wary_ptr can be in one of the states below:
/// * @a exclusive - it is the only pointer associated with its memory block.
/// * @a non-exclusive - in all other situations:
///   * there are other pointers associated with the block;
///   * the block have been released;
///   * any loose pointer is deemed non-exclusive;
///
/// Run-time memory usage diagnostics level depends on the current state of
/// the pointer.
///
/// If the pointer is associated with a memory block it can perform full range
/// of run-time checks:
/// * Memory leaks detection. Memory leaks happen when the last pointer to
///   a memory block is destroyed.
/// * @a Wild or @a dangling pointers detection. A pointer becomes wild if the
///   memory block, it is associated with, has been released or its current
///   position points beyond that memory block.
/// * Uninitialized pointer usage. Only assignment operation is allowed
///   on uninitialized pointers.
///
/// @note Premature memory deallocation (when a memory block is released while
///       other pointers to this memory block exist) is not treated as an
///       error. wary_ptr has to be less strict because otherwise stl
///       containers would not compile with wary_allocator. (Actually they don't
///       at the moment for some other reasons).
/// @todo Find out why stl container do not compile with wary_allocator.
///
/// @note It's a normal situation if wary_ptr becomes wild. For example, if
///       you want to define half-open range [p_begin, p_end), p_end pointer is
///       often wild, but it can be used in arithmetical operations as long as
///       you don't access the memory it points to (via dereferencing, member
///       selectors etc.).
///
/// If a pointer is not associated with a memory block (a loose pointer) it is
/// not possible to perform any of the run-time checks above. In this case,
/// only attempts to release memory via a loose pointer will be detected as
/// an error.
template <typename T>
class wary_ptr final : public detail::wary_ptr_det::wary_ptr_base<T>
{
public:
  /// @c down_cast function overload for wary_ptr pointer type.
  /// @pre The pointer is associated and valid.
  /// @pre Target element type is the type which the memory was originally
  ///      allocated for.
  template <typename TargetPtr, typename S>
  friend TargetPtr down_cast(const wary_ptr<S>&);

  /// A wary_ptr of different type may need access to wary_ptr's internals
  /// for type conversion.
  template <typename> friend class wary_ptr;

  /// A memory allocator, which associates allocated memory with wary_ptr's.
  template <typename, typename> friend class wary_allocator;

  friend class test::Tester_wary_ptr;

  friend class detail::wary_ptr_factory;

private:
  /// An undefined class to be used instead of void type when reference is
  /// needed.
  /// @see wary_ptr<T>::pointer_to
  struct void_obj;

public:
  /// Default constructor.
  /// Initializes a null pointer.
  wary_ptr();

  /// Null wary_ptr constructor.
  wary_ptr(std::nullptr_t);

  /// Copy constructor.
  /// Copies wary_ptr from @b other and increments use count if the source
  /// pointer is associated with a memory block and has not been released.
  /// @post Newly constructed pointer is equal to @b other and is not
  ///       exclusive.
  wary_ptr(const wary_ptr<T>& other);

  /// Implicit conversion constructor.
  /// Converts given wary_ptr and increments use count if the source
  /// pointer is associated with a memory block and has not been released.
  /// @post Newly constructed pointer is equal to @b other and is not
  ///       exclusive.
  template <
    typename U,
    typename =
      typename std::enable_if<std::is_convertible<U*, T*>::value, void>::type>
  wary_ptr(const wary_ptr<U>& other);

  /// Implicit conversion constructor for rvalues.
  /// Does the same as Implicit conversion constructor for lvalues,
  /// no special optimization for rvalues is done yet.
  /// @todo optimize for rvalues.
  template <
    typename U,
    typename =
      typename std::enable_if<std::is_convertible<U*, T*>::value, void>::type>
  wary_ptr(wary_ptr<U>&& other);

  /// Destructor.
  /// @pre The pointer is not exclusive. Otherwise, it would cause a memory
  ///      leak.
  /// @pre The pointer has not been released previously.
  /// @warning The destructor may terminate the program in case of memory
  ///          misuse. Additional error message will be sent into the standard
  ///          error stream (@c std::cerr).
  ~wary_ptr();

  /// Explicit conversion operator.
  /// Used by static_cast.
  template <typename U> explicit operator wary_ptr<U>() const;

  /// Copy assignment operator.
  /// @note This pointer is allowed to be uninitialized, in this case it will
  ///       be null-initialized before assignment.
  /// @pre @c other pointer is initialized.
  /// @pre This pointer doesn't have exclusive access to its memory block.
  ///      Being non-exclusive is required because otherwise it would cause
  ///      a memory leak after assignment since the last reference to the
  ///      memory block would be lost.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  wary_ptr<T>& operator=(wary_ptr<T> other);

  /// @name Pointer arithmetic operators
  ///@{
  /// All arithmetic operations on wary_ptr's behave exactly the same as on
  /// raw pointers.
  /// @note the pointer may become wild after any of this operations.

  /// Pointer positive shift.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  const wary_ptr<T> operator+(std::ptrdiff_t) const;

  /// Pointer negative shift.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  const wary_ptr<T> operator-(std::ptrdiff_t) const;

  /// Pointers subtraction.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  std::ptrdiff_t operator-(const wary_ptr<T>&) const;

  /// Assignment by sum.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  wary_ptr<T>& operator+=(std::ptrdiff_t);

  /// Assignment by difference.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  wary_ptr<T>& operator-=(std::ptrdiff_t);

  /// Pre-increment operator.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  wary_ptr<T>& operator++();

  /// Pre-decrement operator.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  wary_ptr<T>& operator--();

  /// Post-increment operator.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  const wary_ptr<T> operator++(int);

  /// Post-decrement operator.
  /// @pre This pointer must be initialized.
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  const wary_ptr<T> operator--(int);

  ///@}

  /// Member selection operator.
  /// Disabled for pointers to void because void is not an object.
  /// @pre The pointer is valid (see wary_ptr_state::is_valid()).
  /// @throws std::logic_error
  ///         Thrown in case of pre-conditions violation.
  T* operator->() const;

  /// Casting to bool.
  /// @returns @c true if the pointer is not null, otherwise @c false.
  explicit operator bool() const;

  /// @name Pointers comparison operators
  /// @{
  /// Comparison of wary_ptr's behave the same as that of raw pointers.

  /// Operator "equal to".
  template <typename U> bool operator==(const wary_ptr<U>& other) const;

  /// Operator "inequal to".
  template <typename U> bool operator!=(const wary_ptr<U>& other) const;

  /// Operator "less than".
  template <typename U> bool operator<(const wary_ptr<U>& other) const;

  /// Operator "greater than".
  template <typename U> bool operator>(const wary_ptr<U>& other) const;

  /// Operator "less than or equal to".
  template <typename U> bool operator<=(const wary_ptr<U>& other) const;

  /// Operator "greater than or equal to".
  template <typename U> bool operator>=(const wary_ptr<U>& other) const;

  /// Null test.
  bool operator==(std::nullptr_t) const;

  /// Inequality to null test.
  bool operator!=(std::nullptr_t) const;

  /// Less than null test (always false).
  bool operator<(std::nullptr_t) const;

  /// Less than or equal to null test.
  bool operator<=(std::nullptr_t) const;

  /// Greater than null test.
  bool operator>(std::nullptr_t) const;

  /// Greater than or equal to null test (always true).
  bool operator>=(std::nullptr_t) const;

  /// @}

  /// Creates a loose wary_ptr to the given object.
  /// @param x For non-void type, a reference to the object, newly created
  ///          loose wary_ptr will point to.
  ///          For void pointer type this method is not usable. Create a
  ///          pointer to more specific type and then cast to wary_ptr<void>
  ///          if necessary.
  ///
  /// Example:
  /// @code
  /// int var = 0;
  ///
  /// typedef wary_ptr<int> pointer;
  ///
  /// pointer ptr = std::pointer_traits<pointer>::pointer_to(var);
  ///
  /// wary_ptr<void> ptr_void = static_cast<wary_ptr<void>>(ptr);
  /// @endcode
  ///
  /// @see wary_ptr(const detail::wary_ptr_det::wary_ptr_state<T>&)
  static wary_ptr<T> pointer_to(
    typename std::conditional<std::is_void<T>::value, void_obj, T>::type& x);

  /// Gets a raw pointer from a `wary_ptr` pointer. No runtime checks are done.
  static T* to_address(const wary_ptr<T>& ptr);

protected:
  /// A constructor to be used by pointer_to static method to create a loose
  /// pointer.
  /// @see pointer_to()
  explicit wary_ptr(const detail::wary_ptr_det::wary_ptr_state<T>& state);

  /// @name Methods for friends
  /// @{
  /// @see wary_allocator

  /// Associated wary_ptr constructor.
  /// Initializes a new wary_ptr and associates it with the given memory
  /// block. Newly created wary_ptr points to the beginning of the memory
  /// block.
  /// @param p_array A pointer to a memory block to be associated with this
  ///        pointer (an array of elements of type T). @c nullptr is not
  ///        allowed.
  /// @param elements_num Number of elements of type T in the memory block
  ///        (0 is not allowed). To associate a pointer with a single object,
  ///        pass 1.
  /// @throws std::invalid_argument thrown in case of argument requirements
  ///         violation.
  wary_ptr(T* p_array, std::size_t elements_num);

  /// Disassociates wary_ptr from its memory block.
  /// Marks associated wary_ptr as released.
  /// @pre The pointer is associated with a memory block, which has not been
  ///      released, and points to its beginning.
  /// @returns A reference to wary_ptr's internal state.
  /// @throws std::logic_error
  ///         thrown in case of pre-conditions violation.
  const detail::wary_ptr_det::wary_ptr_state<T>& release() const;

  /// @}

private:
  wary_ptr(const detail::wary_ptr_det::wary_ptr_base<T>& other);

private:
  using detail::wary_ptr_det::wary_ptr_base<T>::state_;
};

template <typename T>
wary_ptr<T>::wary_ptr()
: detail::wary_ptr_det::wary_ptr_base<T>(
    detail::wary_ptr_det::wary_ptr_state<T>(nullptr, nullptr))
{
}

template <typename T>
wary_ptr<T>::wary_ptr(std::nullptr_t)
: detail::wary_ptr_det::wary_ptr_base<T>(
    detail::wary_ptr_det::wary_ptr_state<T>(nullptr, nullptr))
{
}

template <typename T>
wary_ptr<T>::wary_ptr(const wary_ptr<T>& other)
: detail::wary_ptr_det::wary_ptr_base<T>([&]() {
  if (!other.state_.is_initialized())
    throw std::logic_error("copy from uninitialized pointer");
  return other.state_;
}())
{
  if (state_.is_associated())
    state_.info()->incr_use_count();

  assert(*this == other);
  assert(!state_.is_exclusive());
}

template <typename T>
template <typename U, typename>
wary_ptr<T>::wary_ptr(const wary_ptr<U>& other)
: detail::wary_ptr_det::wary_ptr_base<T>(
    detail::wary_ptr_det::wary_ptr_state<T>(other.state_.ptr(),
                                            other.state_.info()))
{
  if (state_.is_associated())
    state_.info()->incr_use_count();

  assert(*this == other);
  assert(!state_.is_exclusive());
}

template <typename T>
template <typename U, typename>
wary_ptr<T>::wary_ptr(wary_ptr<U>&& other)
: detail::wary_ptr_det::wary_ptr_base<T>(
    detail::wary_ptr_det::wary_ptr_state<T>(other.state_.ptr(),
                                            other.state_.info()))
{
  if (state_.is_associated())
    state_.info()->incr_use_count();

  assert(*this == other);
  assert(!state_.is_exclusive());
}

template <typename T> wary_ptr<T>::~wary_ptr()
{
  if (state_.is_exclusive() && !state_.is_released())
  {
    std::cerr << "************************************************" << std::endl
              << "************************************************" << std::endl
              << "**            Memory leak detected            **" << std::endl
              << "**      The program will be terminated!       **" << std::endl
              << "************************************************" << std::endl
              << "************************************************"
              << std::endl;

    assert(!state_.is_exclusive() || state_.is_released());

    std::abort();
  }

  if (state_.is_associated())
  {
    state_.info()->decr_use_count();
  }
}

template <typename T>
template <typename U>
wary_ptr<T>::operator wary_ptr<U>() const
{
  return wary_ptr<U>(detail::wary_ptr_det::wary_ptr_state<U>(
    static_cast<U*>(state_.ptr()), state_.info()));
}

template <typename T>
wary_ptr<T>::wary_ptr(const detail::wary_ptr_det::wary_ptr_base<T>& other)
: detail::wary_ptr_det::wary_ptr_base<T>(
    other.state_.ptr(), other.state_.info(), other.state_.is_released())
{
  if (state_.is_associated())
    state_.info()->incr_use_count();

  assert(*this == other);
  assert(!state_.is_exclusive());
}

template <typename T>
const wary_ptr<T> wary_ptr<T>::operator+(std::ptrdiff_t offset) const
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  wary_ptr<T> tmp(*this);

  tmp += offset;

  return tmp;
}

template <typename T>
const wary_ptr<T> wary_ptr<T>::operator-(std::ptrdiff_t offset) const
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  wary_ptr<T> tmp(*this);

  tmp -= offset;

  return tmp;
}

template <typename T>
std::ptrdiff_t wary_ptr<T>::operator-(const wary_ptr<T>& other) const
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  return state_.ptr() - other.state_.ptr();
}

template <typename T>
wary_ptr<T>& wary_ptr<T>::operator+=(std::ptrdiff_t offset)
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  state_.shift(offset);

  return *this;
}

template <typename T>
wary_ptr<T>& wary_ptr<T>::operator-=(std::ptrdiff_t offset)
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  return (*this) += (-offset);
}

template <typename T> wary_ptr<T>& wary_ptr<T>::operator++()
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  return (*this) += 1;
}

template <typename T> wary_ptr<T>& wary_ptr<T>::operator--()
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  return (*this) += -1;
}

template <typename T> const wary_ptr<T> wary_ptr<T>::operator++(int)
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  wary_ptr<T> tmp(*this);

  ++(*this);

  return tmp;
}

template <typename T> const wary_ptr<T> wary_ptr<T>::operator--(int)
{
  if (!state_.is_initialized())
    throw std::logic_error("uninitialized pointer");

  wary_ptr<T> tmp(*this);

  --(*this);

  return tmp;
}

template <typename T> T* wary_ptr<T>::operator->() const
{
  if (!state_.is_valid())
    throw std::logic_error("invalid pointer");

  return state_.ptr();
}

template <typename T> wary_ptr<T>::operator bool() const
{
  return state_.ptr() != nullptr;
}

template <typename T> wary_ptr<T>& wary_ptr<T>::operator=(wary_ptr<T> other)
{
  if (!other.state_.is_initialized())
    throw std::logic_error("assignment of uninitialized pointer");

  if (state_.is_exclusive())
    throw std::logic_error("exclusive pointer assignment");

  if (!state_.is_initialized())
  {
    new (this) wary_ptr<T>(nullptr);
  }

  if (state_.is_associated())
    state_.info()->decr_use_count();

  state_ = other.state_;

  if (state_.is_associated())
    state_.info()->incr_use_count();

  assert(*this == other);

  return *this;
}

template <typename T>
template <typename U>
bool wary_ptr<T>::operator==(const wary_ptr<U>& other) const
{
  return state_.ptr() == other.state_.ptr();
}

template <typename T>
template <typename U>
bool wary_ptr<T>::operator!=(const wary_ptr<U>& other) const
{
  return state_.ptr() != other.state_.ptr();
}

template <typename T>
template <typename U>
bool wary_ptr<T>::operator<(const wary_ptr<U>& other) const
{
  return state_.ptr() < other.state_.ptr();
}

template <typename T>
template <typename U>
bool wary_ptr<T>::operator>(const wary_ptr<U>& other) const
{
  return state_.ptr() > other.state_.ptr();
}

template <typename T>
template <typename U>
bool wary_ptr<T>::operator<=(const wary_ptr<U>& other) const
{
  return state_.ptr() <= other.state_.ptr();
}

template <typename T>
template <typename U>
bool wary_ptr<T>::operator>=(const wary_ptr<U>& other) const
{
  return state_.ptr() >= other.state_.ptr();
}

template <typename T> bool wary_ptr<T>::operator==(std::nullptr_t) const
{
  return state_.ptr() == nullptr;
}

template <typename T> bool wary_ptr<T>::operator!=(std::nullptr_t) const
{
  return state_.ptr() != nullptr;
}

template <typename T> bool wary_ptr<T>::operator<(std::nullptr_t) const
{
  return false;
}

template <typename T> bool wary_ptr<T>::operator<=(std::nullptr_t) const
{
  return (*this) == nullptr;
}

template <typename T> bool wary_ptr<T>::operator>(std::nullptr_t) const
{
  return (*this) != nullptr;
}

template <typename T> bool wary_ptr<T>::operator>=(std::nullptr_t) const
{
  return true;
}

template <typename T>
wary_ptr<T> wary_ptr<T>::pointer_to(
  typename std::conditional<std::is_void<T>::value, void_obj, T>::type& x)
{
  return wary_ptr<T>(
    detail::wary_ptr_det::wary_ptr_state<T>(std::addressof(x), nullptr));
}

template <typename T> T* wary_ptr<T>::to_address(const wary_ptr<T>& ptr)
{
  return ptr.state_.ptr();
}

template <typename T>
wary_ptr<T>::wary_ptr(const detail::wary_ptr_det::wary_ptr_state<T>& state)
: detail::wary_ptr_det::wary_ptr_base<T>(state)
{
  if (state_.is_associated())
    state_.info()->incr_use_count();
}

template <typename T>
wary_ptr<T>::wary_ptr(T* p_value, std::size_t elements_num)
: detail::wary_ptr_det::wary_ptr_base<T>(
    detail::wary_ptr_det::wary_ptr_state<T>(
      p_value,
      detail::wary_ptr_det::mem_block_info::create(p_value, elements_num)))
{
  assert(state_.ptr() != nullptr);
  assert(state_.info() != nullptr);
  assert(state_.info()->use_count() == 0);

  if (state_.is_associated())
    state_.info()->incr_use_count();

  assert(state_.is_exclusive());
}

template <typename T>
const detail::wary_ptr_det::wary_ptr_state<T>& wary_ptr<T>::release() const
{
  if (!state_.is_associated())
    throw std::logic_error("releasing not associated pointer");

  if (state_.is_released())
    throw std::logic_error("releasing pointer twice");

  if (static_cast<void*>(state_.ptr()) != state_.info()->begin())
    throw std::logic_error("releasing not from the head of the block");

  state_.info()->release();

  return state_;
}

/// @name Comparison with nullptr on the left-hand side.
/// @relates dst::wary_ptr
/// @{
/// All operations implemented via their member-twins with nullptr on the
/// right-hand side.

/// Null test.
template <typename T> bool operator==(std::nullptr_t, const wary_ptr<T>& rhs)
{
  return rhs == nullptr;
}

/// Inequality to null test.
template <typename T> bool operator!=(std::nullptr_t, const wary_ptr<T>& rhs)
{
  return rhs != nullptr;
}

/// Greater than null test.
template <typename T> bool operator<(std::nullptr_t, const wary_ptr<T>& rhs)
{
  return rhs > nullptr;
}

/// Greater than or equal to null test (always true).
template <typename T> bool operator<=(std::nullptr_t, const wary_ptr<T>& rhs)
{
  return rhs >= nullptr;
}

/// Less than null test (always false).
template <typename T> bool operator>(std::nullptr_t, const wary_ptr<T>& rhs)
{
  return rhs < nullptr;
}

/// Less than or equal to null test.
template <typename T> bool operator>=(std::nullptr_t, const wary_ptr<T>& rhs)
{
  return rhs <= nullptr;
}

/// @}

/// @name Type cast helper function overload
/// @relates dst::wary_ptr
/// @{

/// @see wary_ptr::down_cast(const wary_ptr<S>&)
template <typename TargetPtr, typename S>
TargetPtr down_cast(const wary_ptr<S>& ptr)
{
  typedef typename std::pointer_traits<TargetPtr>::element_type element_type;

  if (!ptr.state_.is_associated())
    throw std::logic_error("not associated pointer");

  if (!ptr.state_.is_valid())
    throw std::logic_error("invalid pointer");

  if (typeid(element_type) != ptr.state_.info()->element_typeid())
    throw std::logic_error("bad down cast type");

  return static_cast<TargetPtr>(ptr);
}

/// @}

} // dst

namespace std
{
#ifdef _LIBCPP_VERSION    // We use libc++
#if _LIBCPP_STD_VER <= 17 // C++17 or below
inline namespace _LIBCPP_ABI_NAMESPACE
{
template <typename T>
inline T* __to_raw_pointer(dst::wary_ptr<T> ptr) noexcept(true)
{
  return dst::wary_ptr<T>::to_address(ptr);
}
}
#endif
#endif
}
