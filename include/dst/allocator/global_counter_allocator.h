
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//        (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cassert> // assert
#include <cstddef> // std::size_t
#include <memory>  // std::shared_ptr, std::make_shared, std::allocator_traits

namespace dst
{

namespace detail
{

namespace allocator
{

template <bool> struct global_counter
{
  static std::size_t g_count;
};

template <> struct global_counter<false>;

template <bool B> std::size_t global_counter<B>::g_count = 0;
}
}

template <typename T, typename Allocator = std::allocator<T>>
class global_counter_allocator : private Allocator
{
public:
  using base_allocator_type = Allocator;

  using value_type = T;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using size_type = typename std::allocator_traits<Allocator>::size_type;

  template <typename U> struct rebind
  {
    using other = global_counter_allocator<
      U,
      typename std::allocator_traits<Allocator>::template rebind_alloc<U>>;
  };

public:
  global_counter_allocator(const Allocator& allocator = Allocator())
  : Allocator(allocator)
  {
  }

  template <class U>
  global_counter_allocator(const global_counter_allocator<U>& other)
  : Allocator(other.base())
  {
  }

  T* allocate(size_type n)
  {
    detail::allocator::global_counter<true>::g_count += n * sizeof(T);
    return base_().allocate(n);
  }

  void deallocate(T* p, size_type n)
  {
    assert(detail::allocator::global_counter<true>::g_count >= n * sizeof(T));
    detail::allocator::global_counter<true>::g_count -= n * sizeof(T);
    base_().deallocate(p, n);
  }

  template <typename... Args> void construct(pointer p, Args&&... args)
  {
    std::allocator_traits<base_allocator_type>::construct(
      *this, p, std::forward<Args>(args)...);
  }

  void destroy(pointer p)
  {
    std::allocator_traits<base_allocator_type>::destroy(*this, p);
  }

  template <typename U, typename A, typename V, typename B>
  friend bool operator==(const global_counter_allocator<U, A>& lhs,
                         const global_counter_allocator<V, B>& rhs);

public:
  const base_allocator_type& base() const
  {
    return *this;
  }

  static size_type allocated()
  {
    return detail::allocator::global_counter<true>::g_count;
  }

private:
  base_allocator_type& base_()
  {
    return *this;
  }

private:
  template <typename, typename> friend class global_counter_allocator;
};

template <typename U, typename A, typename V, typename B>
bool operator==(const global_counter_allocator<U, A>& lhs,
                const global_counter_allocator<V, B>& rhs)
{
  return lhs.base() == rhs.base();
}

template <typename U, typename A, typename V, typename B>
bool operator!=(const global_counter_allocator<U, A>& lhs,
                const global_counter_allocator<V, B>& rhs)
{
  return !(lhs == rhs);
}

} // dst
