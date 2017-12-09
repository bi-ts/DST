
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//        (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "iterator_facade.h"

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
  class const_tree_iterator
    : public iterator_facade<const_tree_iterator,
                             binary_tree_branch_iterator_tag,
                             const T>
  {
  public:
    friend class initializer_tree;
    friend class iterator_facade<const_tree_iterator,
                                 binary_tree_branch_iterator_tag,
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
                  const std::shared_ptr<node>& p_left = nullptr,
                  const std::shared_ptr<node>& p_right = nullptr)
    : value_(value)
    , p_left_(p_left)
    , p_right_(p_right)
    {
    }

  private:
    T value_;
    std::shared_ptr<node> p_left_;
    std::shared_ptr<node> p_right_;
  };

public:
  using tree_iterator = const_tree_iterator;

  using value_type = T;
  using reference = const T&;
  using const_reference = const T&;
  using difference_type = typename tree_iterator::difference_type;
  using size_type = typename std::make_unsigned<difference_type>::type;

public:
  initializer_tree(const T& v)
  : p_root_(new node(v))
  , size_(1)
  {
  }

  initializer_tree(const initializer_tree& left,
                   const T& v,
                   const initializer_tree& right)
  : p_root_(new node(v, left.p_root_, right.p_root_))
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

  size_type size() const
  {
    return size_;
  }

  bool empty() const
  {
    return size_ == 0;
  }

private:
  std::shared_ptr<node> p_root_;
  size_type size_;
};

} // binary_tree

} // dst
