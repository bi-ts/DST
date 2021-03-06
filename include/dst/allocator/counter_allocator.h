
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cassert> // assert
#include <cstddef> // std::size_t
#include <memory>  // std::shared_ptr, std::make_shared, std::allocator_traits

namespace dst
{

template <typename T, typename Allocator = std::allocator<T>>
class counter_allocator : private Allocator
{
public:
  using base_allocator_type = Allocator;

  using value_type = T;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using size_type = typename std::allocator_traits<Allocator>::size_type;

  template <typename U> struct rebind
  {
    using other = counter_allocator<
      U,
      typename std::allocator_traits<Allocator>::template rebind_alloc<U>>;
  };

public:
  counter_allocator(const Allocator& allocator = Allocator())
  : Allocator(allocator)
  , p_counter_(std::make_shared<std::size_t>(0))
  {
  }

  template <class U, class A>
  counter_allocator(const counter_allocator<U, A>& other)
  : Allocator(other.base())
  , p_counter_(other.p_counter_)
  {
  }

  pointer allocate(size_type n)
  {
    (*p_counter_) += n * sizeof(T);
    return base_().allocate(n);
  }

  void deallocate(pointer p, size_type n)
  {
    assert(*p_counter_ >= n * sizeof(T));
    (*p_counter_) -= n * sizeof(T);
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
  friend bool operator==(const counter_allocator<U, A>& lhs,
                         const counter_allocator<V, B>& rhs);

public:
  const base_allocator_type& base() const
  {
    return *this;
  }

  size_type allocated() const
  {
    assert(p_counter_);
    return *p_counter_;
  }

private:
  base_allocator_type& base_()
  {
    return *this;
  }

private:
  std::shared_ptr<size_type> p_counter_;

private:
  template <typename, typename> friend class counter_allocator;
};

template <typename U, typename A, typename V, typename B>
bool operator==(const counter_allocator<U, A>& lhs,
                const counter_allocator<V, B>& rhs)
{
  return lhs.base() == rhs.base() && lhs.p_counter_ == rhs.p_counter_;
}

template <typename U, typename A, typename V, typename B>
bool operator!=(const counter_allocator<U, A>& lhs,
                const counter_allocator<V, B>& rhs)
{
  return !(lhs == rhs);
}

} // dst
