
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/binary_tree/iterator_facade.h>
#include <dst/binary_tree/mixin.h>

#include <memory> // std::allocator_traits, std::allocator
#include <type_traits>
#include <utility> // std::move
#include <vector>

namespace dst
{

namespace binary_tree
{

template <typename BinaryTreeIterator,
          typename Allocator = std::allocator<BinaryTreeIterator>>
class ordering_algorithm
{
  static_assert(
    std::is_convertible<typename BinaryTreeIterator::iterator_category,
                        binary_tree_iterator_tag>::value,
    "Ordering algorithm requires binary tree iterator");

public:
  using allocator_type = Allocator;
  using binary_tree_iterator = BinaryTreeIterator;

public:
  ordering_algorithm(const allocator_type& allocator = allocator_type())
  : x_to_root_path_(allocator)
  , y_to_root_path_(allocator)
  {
  }

  bool order(binary_tree_iterator x, binary_tree_iterator y) const
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

private:
  using path_container = std::vector<binary_tree_iterator, allocator_type>;

  mutable path_container x_to_root_path_;
  mutable path_container y_to_root_path_;
};

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
    return ordering_.order(x, y);
  }

  bool order(const_iterator x, const_iterator y) const
  {
    return order(x.base(), y.base());
  }

protected:
  ordering()
  : base()
  , ordering_(base::get_allocator())
  {
  }

  explicit ordering(const allocator_type& allocator)
  : base(allocator)
  , ordering_(allocator)
  {
  }

  explicit ordering(const ordering& other, const allocator_type& allocator)
  : base(other, allocator)
  , ordering_(allocator)
  {
  }

  ordering(ordering&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  , ordering_(allocator)
  {
  }

private:
  ordering_algorithm<const_tree_iterator,
                     typename std::allocator_traits<allocator_type>::
                       template rebind_alloc<const_tree_iterator>>
    ordering_;
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
