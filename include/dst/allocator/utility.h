
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cassert>
#include <cstddef> // std::size_t, std::uint8_t
#include <limits>  // std::numeric_limits
#include <memory>  // std::allocator_traits, std::addressof
#include <utility> // std::forward

namespace dst
{

namespace memory
{

template <typename T, typename Allocator, typename... Args>
typename std::enable_if<
  !std::is_same<typename std::allocator_traits<Allocator>::value_type,
                T>::value>::type
construct(Allocator alloc, T& obj, Args&&... args)
{
  using allocator_type =
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  allocator_type allocator(alloc);

  std::allocator_traits<allocator_type>::construct(
    allocator, std::addressof(obj), std::forward<Args>(args)...);
}

template <typename T, typename Allocator, typename... Args>
typename std::enable_if<
  std::is_same<typename std::allocator_traits<Allocator>::value_type,
               T>::value>::type
construct(Allocator alloc, T& obj, Args&&... args)
{
  std::allocator_traits<Allocator>::construct(
    alloc, std::addressof(obj), std::forward<Args>(args)...);
}

template <typename T, typename Allocator>
typename std::enable_if<
  !std::is_same<typename std::allocator_traits<Allocator>::value_type,
                T>::value>::type
destroy(Allocator alloc, T& obj)
{
  using allocator_type =
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  allocator_type allocator(alloc);

  std::allocator_traits<allocator_type>::destroy(allocator,
                                                 std::addressof(obj));
}

template <typename T, typename Allocator>
typename std::enable_if<
  std::is_same<typename std::allocator_traits<Allocator>::value_type,
               T>::value>::type
destroy(Allocator alloc, T& obj)
{
  std::allocator_traits<Allocator>::destroy(alloc, std::addressof(obj));
}

template <typename Allocator>
void* allocate_aligned(const Allocator& alloc,
                       std::size_t size,
                       std::size_t alignment)
{
  assert(alignment <= std::numeric_limits<std::uint8_t>::max());

  using allocator_type = typename std::allocator_traits<
    Allocator>::template rebind_alloc<std::uint8_t>;

  allocator_type allocator(alloc);

  std::uint8_t* const p_mem_block = allocator.allocate(size + alignment);

  const std::size_t offset =
    alignment - reinterpret_cast<std::uintptr_t>(p_mem_block) % alignment;

  const auto p_object = p_mem_block + offset;

  std::uint8_t* const p_meta_data = p_object - 1;

  assert(p_meta_data < p_object && p_meta_data >= p_mem_block);

  *p_meta_data = static_cast<std::uint8_t>(offset);

  return p_object;
}

template <typename Allocator>
void deallocate_aligned(const Allocator& alloc,
                        void* p_object,
                        std::size_t size,
                        std::size_t alignment)
{
  assert(alignment <= std::numeric_limits<std::uint8_t>::max());

  std::uint8_t* const p_object_bytes = static_cast<std::uint8_t*>(p_object);

  std::uint8_t* const p_meta_data = p_object_bytes - 1;

  const std::size_t offset = *p_meta_data;

  assert(offset <= alignment);

  std::uint8_t* const p_mem_block = p_object_bytes - offset;

  using allocator_type = typename std::allocator_traits<
    Allocator>::template rebind_alloc<std::uint8_t>;

  allocator_type allocator(alloc);

  allocator.deallocate(p_mem_block, size + alignment);
}

template <typename T, typename Allocator, typename... Args>
typename std::allocator_traits<Allocator>::template rebind_traits<T>::pointer
new_object(const Allocator& alloc, Args&&... args)
{
  using allocator_type =
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  allocator_type allocator(alloc);

  const auto p_object = allocator.allocate(1);

  try
  {
    construct(allocator, *p_object, std::forward<Args>(args)...);
  }
  catch (...)
  {
    allocator.deallocate(p_object, 1);

    throw;
  }

  return p_object;
}

template <typename T, typename Allocator>
void delete_object(
  const Allocator& alloc,
  typename std::allocator_traits<Allocator>::template rebind_traits<T>::pointer
    p_object)
{
  using allocator_type =
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  allocator_type allocator(alloc);

  try
  {
    destroy(allocator, *p_object);
  }
  catch (...)
  {
    allocator.deallocate(p_object, 1);

    throw;
  }

  allocator.deallocate(p_object, 1);
}
} // memory

/// A function for static downcasting of pointers. Should be used to cast
/// base class pointers to most-derived class pointers.
/// For raw pointers it performs simple @c static_cast. Custom pointer types
/// can overload this function to provide some run-time checks.
/// @warning This function should never be used via fully qualified name,
///          like @c dst::down_cast, because otherwise, for custome pointer
///          types in nested namespaces, argument dependent lookup will fail.
template <typename T, typename U> T down_cast(const U& ptr)
{
  return static_cast<T>(ptr);
}
} // dst
