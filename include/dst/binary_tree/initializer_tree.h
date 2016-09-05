
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "iterator_facade.h"

#include "algorithm.h"

#include <memory> // std::shared_ptr

namespace dst
{

namespace binary_tree
{

template <typename T> class initializer_tree
{
private:
  class node;

public:
  class const_tree_iterator : public iterator_facade<const_tree_iterator,
                                                     binary_tree_iterator_tag,
                                                     const T>
  {
  public:
    friend class initializer_tree;
    friend class iterator_facade<const_tree_iterator,
                                 binary_tree_iterator_tag,
                                 const T>;

  public:
    const_tree_iterator()
    : p_node_(nullptr)
    {
    }

    friend bool operator==(const const_tree_iterator& lhs,
                           const const_tree_iterator& rhs)
    {
      return lhs.p_node_ == rhs.p_node_;
    }

  private:
    explicit const_tree_iterator(node* p_node)
    : p_node_(p_node)
    {
    }

    bool nil() const
    {
      return p_node_ == nullptr;
    }

    const T& value() const
    {
      return p_node_->value_;
    }

    void move_to_parent()
    {
      p_node_ = p_node_->p_parent_;
    }

    void move_left()
    {
      p_node_ = p_node_->p_left_.get();
    }

    void move_right()
    {
      p_node_ = p_node_->p_right_.get();
    }

  private:
    node* p_node_;
  };

private:
  class node
  {
  public:
    friend const_tree_iterator;

  public:
    explicit node(const T& value,
                  std::shared_ptr<node>&& p_left = nullptr,
                  std::shared_ptr<node>&& p_right = nullptr)
    : value_(value)
    , p_parent_(nullptr)
    , p_left_(std::move(p_left))
    , p_right_(std::move(p_right))
    {
      if (p_left_ != nullptr)
        p_left_->p_parent_ = this;

      if (p_right_ != nullptr)
        p_right_->p_parent_ = this;
    }

  private:
    T value_;
    node* p_parent_;
    std::shared_ptr<node> p_left_;
    std::shared_ptr<node> p_right_;
  };

public:
  using tree_iterator = const_tree_iterator;

  using value_type = T;
  using reference = const T&;
  using const_reference = const T&;
  using difference_type =
    typename std::iterator_traits<tree_iterator>::difference_type;
  using size_type = typename std::make_unsigned<difference_type>::type;
  using const_iterator =
    inorder_depth_first_search_iterator<const_tree_iterator>;
  using iterator = const_iterator;

public:
  initializer_tree(const T& v)
  : p_root_(new node(v))
  , size_(1)
  {
  }

  initializer_tree(initializer_tree&& left,
                   const T& v,
                   initializer_tree&& right)
  : p_root_(new node(v, std::move(left.p_root_), std::move(right.p_root_)))
  , size_(1 + left.size_ + right.size_)
  {
  }

  initializer_tree()
  : p_root_(nullptr)
  , size_(0)
  {
  }

  const_tree_iterator root() const
  {
    return const_tree_iterator(p_root_.get());
  }

  const_tree_iterator nil() const
  {
    return const_tree_iterator(nullptr);
  }

  const_iterator begin() const
  {
    return begin_inorder_depth_first_search(root());
  }

  const_iterator end() const
  {
    return end_inorder_depth_first_search(nil());
  }

  size_type size() const
  {
    return p_root_ != nullptr ? size_ : 0;
  }

  bool empty() const
  {
    return p_root_ == nullptr;
  }

private:
  std::shared_ptr<node> p_root_;
  size_type size_;
};

} // binary_tree

} // dst

