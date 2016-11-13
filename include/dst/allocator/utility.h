
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <memory>  // std::allocator_traits, std::addressof
#include <utility> // std::forward

namespace dst
{

template <typename T, typename Allocator, typename... Args>
void construct(const Allocator& alloc, T& obj, Args&&... args)
{
  using allocator_type =
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  allocator_type allocator(alloc);

  std::allocator_traits<allocator_type>::construct(
    allocator, std::addressof(obj), std::forward<Args>(args)...);
}

template <typename T, typename Allocator>
void destroy(const Allocator& alloc, T& obj)
{
  using allocator_type =
    typename std::allocator_traits<Allocator>::template rebind_alloc<T>;

  allocator_type allocator(alloc);

  std::allocator_traits<allocator_type>::destroy(allocator,
                                                 std::addressof(obj));
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
} // dst

