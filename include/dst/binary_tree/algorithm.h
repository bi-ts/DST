
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "iterator_facade.h"

#include <cassert>     // assert
#include <iterator>    // std::iterator_traits, std::iterator
#include <type_traits> // std::is_convertible, std::enable_if
#include <vector>

namespace dst
{

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
bool leaf(BinaryTreeIterator position)
{
  assert(!!position);

  return !left(position) && !right(position);
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator maximum(BinaryTreeIterator root)
{
  if (!!root)
  {
    while (!!right(root))
    {
      root = right(root);
    }
  }

  return root;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator minimum(BinaryTreeIterator root)
{
  if (!!root)
  {
    while (!!left(root))
    {
      root = left(root);
    }
  }

  return root;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator parent(BinaryTreeIterator position)
{
  assert(!!position);

  auto p = position;

  return ++p;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator root(BinaryTreeIterator position)
{
  assert(!!position);

  auto it = position;
  ++it;

  while (!!it)
  {
    ++it;
    ++position;
  }

  return position;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator sibling(BinaryTreeIterator position)
{
  assert(!!position);
  assert(!!parent(position));

  const auto p = parent(position);

  if (position == left(p))
    return right(p);

  assert(position = right(p));

  return left(p);
}

template <typename BinaryTreeIteratorA,
          typename BinaryTreeIteratorB,
          typename = enable_for_binary_tree_iterator<BinaryTreeIteratorA>,
          typename = enable_for_binary_tree_iterator<BinaryTreeIteratorB>>
bool topologically_equal(BinaryTreeIteratorA x, BinaryTreeIteratorB y)
{
  if (!x && !y)
    return true;

  if (!x || !y)
    return false;

  return (*x == *y && topologically_equal(left(x), left(y)) &&
          topologically_equal(right(x), right(y)));
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator roll_down_left(BinaryTreeIterator root)
{
  if (!root)
    return root;

  while (!leaf(root))
  {
    root = !!left(root) ? left(root) : right(root);
  }

  return root;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator roll_down_right(BinaryTreeIterator root)
{
  if (!root)
    return root;

  while (!leaf(root))
  {
    root = !!right(root) ? right(root) : left(root);
  }

  return root;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator successor(BinaryTreeIterator position)
{
  assert(!!position);

  if (!!right(position))
    return minimum(right(position));

  auto p = parent(position);

  while (!!p && position == right(p))
  {
    position = p;
    p = parent(p);
  }

  return p;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
BinaryTreeIterator predecessor(BinaryTreeIterator position)
{
  assert(!!position);

  if (!!left(position))
    return maximum(left(position));

  auto p = parent(position);

  while (!!p && position == left(p))
  {
    position = p;
    p = parent(p);
  }

  return p;
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
bool order(BinaryTreeIterator x, BinaryTreeIterator y)
{
  if (x == y)
    return false;

  if (!y)
    return !!x;
  
  if (!x)
    return false;
  
  std::vector<BinaryTreeIterator> x_to_root_path;
  std::vector<BinaryTreeIterator> y_to_root_path;

  for (; !!x; ++x)
  {
    x_to_root_path.push_back(x);
  }

  for (; !!y; ++y)
  {
    y_to_root_path.push_back(y);
  }

  assert(x_to_root_path.back() == y_to_root_path.back());

  auto it_x = x_to_root_path.crbegin(), it_y = y_to_root_path.crbegin();

  for (;
       it_x != x_to_root_path.crend() && it_y != y_to_root_path.crend();
       ++it_x, ++it_y)
  {
    if (*it_x != *it_y)
      return (left(parent(*it_x)) == *it_x);
  }

  if (it_x != x_to_root_path.crend())
    return (left(parent(*it_x)) == *it_x);

  if (it_y != y_to_root_path.crend())
    return (right(parent(*it_y)) == *it_y);

  assert(false);

  return false;
};

template <typename BinaryTreeIterator>
class inorder_depth_first_search_iterator
  : public iterator_facade<
      inorder_depth_first_search_iterator<BinaryTreeIterator>,
      std::forward_iterator_tag,
      typename std::iterator_traits<BinaryTreeIterator>::value_type>
{
private:
  friend iterator_facade<
    inorder_depth_first_search_iterator<BinaryTreeIterator>,
    std::forward_iterator_tag,
    typename std::iterator_traits<BinaryTreeIterator>::value_type>;

public:
  inorder_depth_first_search_iterator(
    BinaryTreeIterator position = BinaryTreeIterator())
  : position_(position)
  {
  }

  template <
    typename OtherIterator,
    typename = typename std::enable_if<
      std::is_convertible<OtherIterator, BinaryTreeIterator>::value>::type>
  inorder_depth_first_search_iterator(
    const inorder_depth_first_search_iterator<OtherIterator>& other)
  : inorder_depth_first_search_iterator(other.base())
  {
  }

  BinaryTreeIterator base() const
  {
    return position_;
  }

  friend bool operator==(const inorder_depth_first_search_iterator& lhs,
                         const inorder_depth_first_search_iterator& rhs)
  {
    return lhs.position_ == rhs.position_;
  }

private:
  typename inorder_depth_first_search_iterator::reference value() const
  {
    return *position_;
  }

  void move_forward()
  {
    position_ = successor(position_);
  }

private:
  BinaryTreeIterator position_;
};

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
inorder_depth_first_search_iterator<BinaryTreeIterator>
begin_inorder_depth_first_search(BinaryTreeIterator root)
{
  return inorder_depth_first_search_iterator<BinaryTreeIterator>(
    minimum(root));
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
inorder_depth_first_search_iterator<BinaryTreeIterator>
end_inorder_depth_first_search(BinaryTreeIterator last)
{
  return inorder_depth_first_search_iterator<BinaryTreeIterator>(last);
}

template <typename BinaryTreeIterator>
class postorder_depth_first_search_iterator
  : public iterator_facade<
      postorder_depth_first_search_iterator<BinaryTreeIterator>,
      std::forward_iterator_tag,
      typename std::iterator_traits<BinaryTreeIterator>::value_type>
{
private:
  friend iterator_facade<
    postorder_depth_first_search_iterator<BinaryTreeIterator>,
    std::forward_iterator_tag,
    typename std::iterator_traits<BinaryTreeIterator>::value_type>;

public:
  postorder_depth_first_search_iterator(
    BinaryTreeIterator position = BinaryTreeIterator())
  : position_(position)
  {
  }

  template <
    typename OtherIterator,
    typename = typename std::enable_if<
      std::is_convertible<OtherIterator, BinaryTreeIterator>::value>::type>
  postorder_depth_first_search_iterator(
    const postorder_depth_first_search_iterator<OtherIterator>& other)
  : postorder_depth_first_search_iterator(other.base())
  {
  }

  BinaryTreeIterator base() const
  {
    return position_;
  }

  friend bool operator==(const postorder_depth_first_search_iterator& lhs,
                         const postorder_depth_first_search_iterator& rhs)
  {
    return lhs.position_ == rhs.position_;
  }

private:
  typename postorder_depth_first_search_iterator::reference value() const
  {
    return *position_;
  }

  void move_forward()
  {
    const auto p = parent(position_);

    if (!p || right(p) == position_ || !right(p))
      position_ = p;
    else
      position_ = roll_down_left(right(p));
  }

private:
  BinaryTreeIterator position_;
};

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
postorder_depth_first_search_iterator<BinaryTreeIterator>
begin_postorder_depth_first_search(BinaryTreeIterator root)
{
  return postorder_depth_first_search_iterator<BinaryTreeIterator>(
    roll_down_left(root));
}

template <typename BinaryTreeIterator,
          typename = enable_for_binary_tree_iterator<BinaryTreeIterator>>
postorder_depth_first_search_iterator<BinaryTreeIterator>
end_postorder_depth_first_search(BinaryTreeIterator last)
{
  return postorder_depth_first_search_iterator<BinaryTreeIterator>(last);
}

} // dst

