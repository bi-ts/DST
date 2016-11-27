
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cassert>
#include <cstddef> // std::size_t, std::uint8_t
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
construct(const Allocator& alloc, T& obj, Args&&... args)
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
construct(const Allocator& alloc, T& obj, Args&&... args)
{
  std::allocator_traits<Allocator>::construct(
    alloc, std::addressof(obj), std::forward<Args>(args)...);
}

template <typename T, typename Allocator>
typename std::enable_if<
  !std::is_same<typename std::allocator_traits<Allocator>::value_type,
                T>::value>::type
destroy(const Allocator& alloc, T& obj)
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
destroy(const Allocator& alloc, T& obj)
{
  std::allocator_traits<Allocator>::destroy(alloc, std::addressof(obj));
}

template <typename Allocator>
void* allocate_aligned(const Allocator& alloc,
                       std::size_t size,
                       std::size_t alignment)
{
  using allocator_type = typename std::allocator_traits<
    Allocator>::template rebind_alloc<std::uint8_t>;

  allocator_type allocator(alloc);

  const auto p_mem_block = allocator.allocate(size + alignment);

  const auto offset =
    alignment - reinterpret_cast<std::uintptr_t>(p_mem_block) % alignment;

  const auto p_object = p_mem_block + offset;

  const auto p_meta_data = p_object - 1;

  assert(p_meta_data < p_object && p_meta_data >= p_mem_block);

  *p_meta_data = offset;

  return p_object;
}

template <typename Allocator>
void deallocate_aligned(const Allocator& alloc,
                        void* p_object,
                        std::size_t size,
                        std::size_t alignment)
{
  const auto p_object_bytes = static_cast<std::uint8_t*>(p_object);

  const auto p_meta_data = p_object_bytes - 1;

  const std::size_t offset = *p_meta_data;

  assert(offset <= alignment);

  const auto p_mem_block = p_object_bytes - offset;

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
    std::allocator_traits<allocator_type>::construct(
      allocator, std::addressof(*p_object), std::forward<Args>(args)...);
  }
  catch (...)
  {
    allocator.deallocate(p_object, 1);

    throw;
  }

  return p_object;
}

template <typename T, typename Allocator>
void delete_object(const Allocator& alloc,
                   typename std::allocator_traits<
                     Allocator>::template rebind_traits<T>::pointer p_object)
{
  using allocator_type =
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  allocator_type allocator(alloc);

  try
  {
    std::allocator_traits<allocator_type>::destroy(allocator,
                                                   std::addressof(*p_object));
  }
  catch (...)
  {
    allocator.deallocate(p_object, 1);

    throw;
  }

  allocator.deallocate(p_object, 1);
}
} // memory
} // dst
