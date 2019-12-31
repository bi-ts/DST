
//          Copyright Maksym V. Bilinets 2015 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <memory> // std::allocator_traits

namespace dst
{

namespace binary_tree
{

namespace mixin
{

template <typename T,
          typename M,
          typename Allocator,
          template <typename, typename, typename>
          class Base>
class ordering : public Base<T, M, Allocator>
{
private:
  using base = Base<T, M, Allocator>;

  static_assert(is_balanced_binary_tree<typename base::tree_category>::value,
                "Ordering mixin must be based on a balanced binary tree");

protected:
  using typename base::const_iterator;
  using typename base::const_tree_iterator;

  using allocator_type = typename base::allocator_type;

public:
  bool order(const_tree_iterator x, const_tree_iterator y) const
  {
    if (x == y)
      return false;

    if (!y)
    {
      assert(!!x);
      return true;
    }

    if (!x)
    {
      assert(!!y);
      return false;
    }

    x_to_root_path_.clear();
    y_to_root_path_.clear();

    for (; !!x; ++x)
    {
      x_to_root_path_.push_back(x);
    }

    for (; !!y; ++y)
    {
      y_to_root_path_.push_back(y);
    }

    assert(x_to_root_path_.back() == y_to_root_path_.back());

    auto it_x = x_to_root_path_.crbegin();
    auto it_y = y_to_root_path_.crbegin();

    for (; it_x != x_to_root_path_.crend() && it_y != y_to_root_path_.crend();
         ++it_x, ++it_y)
    {
      if (*it_x != *it_y)
        return (left(parent(*it_x)) == *it_x);
    }

    if (it_x != x_to_root_path_.crend())
    {
      assert(it_y == y_to_root_path_.crend());
      return (left(parent(*it_x)) == *it_x);
    }

    assert(it_x == x_to_root_path_.crend());
    assert(it_y != y_to_root_path_.crend());

    return (right(parent(*it_y)) == *it_y);
  }

  bool order(const_iterator x, const_iterator y) const
  {
    return order(x.base(), y.base());
  }

protected:
  ordering()
  : base()
  , x_to_root_path_(base::get_allocator())
  , y_to_root_path_(base::get_allocator())
  {
  }

  explicit ordering(const allocator_type& allocator)
  : base(allocator)
  , x_to_root_path_(allocator)
  , y_to_root_path_(allocator)
  {
  }

  explicit ordering(const ordering& other, const allocator_type& allocator)
  : base(other, allocator)
  , x_to_root_path_(allocator)
  , y_to_root_path_(allocator)
  {
  }

  ordering(ordering&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  , x_to_root_path_(allocator)
  , y_to_root_path_(allocator)
  {
  }

private:
  using path_container =
    std::vector<const_tree_iterator,
                typename std::allocator_traits<
                  allocator_type>::template rebind_alloc<const_tree_iterator>>;

  mutable path_container x_to_root_path_;
  mutable path_container y_to_root_path_;
};

} // mixin

class Ordering
{
public:
  template <typename T,
            typename M,
            typename Allocator,
            template <typename, typename, typename>
            class Base>
  using type = mixin::ordering<T, M, Allocator, Base>;
};

} // binary_tree

} // dst
