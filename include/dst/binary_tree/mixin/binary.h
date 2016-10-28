
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/allocator/utility.h>
#include <dst/binary_tree/initializer_tree.h>
#include <dst/binary_tree/algorithm.h>
#include <dst/binary_tree/mixin.h>
#include <dst/utility.h>

#include <algorithm> // std::for_each
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <memory>  // std::allocator, std::allocator_traits, std::addressof
#include <utility> // std::swap

namespace dst
{

namespace binary_tree
{

namespace mixin
{

template <typename T,
          typename M,
          typename Allocator,
          template <typename, typename, typename> class Base>
class binary : public Allocator, public Base<T, M, Allocator>
{
private:
  struct node
  {
    using pointer = typename std::allocator_traits<
      Allocator>::template rebind_traits<node>::pointer;

    node(const T& v, pointer parent, pointer left, pointer right)
    : data(v)
    , p_parent(parent)
    , p_left(left)
    , p_right(right)
    {
    }

    pair_or_single<T, M> data;
    pointer p_parent;
    pointer p_left;
    pointer p_right;
  };

  template <typename U>
  class tree_iterator_base : public iterator_facade<tree_iterator_base<U>,
                                                    binary_tree_iterator_tag,
                                                    U>
  {
  public:
    friend class binary;
    friend class iterator_facade<tree_iterator_base<U>,
                                 binary_tree_iterator_tag,
                                 U>;

  public:
    tree_iterator_base()
    : p_node_(nullptr)
    {
    }

    tree_iterator_base(
      const tree_iterator_base<typename std::remove_const<U>::type>& other)
    : p_node_(other.p_node_)
    {
    }

    friend bool operator==(const tree_iterator_base& lhs,
                           const tree_iterator_base& rhs)
    {
      return lhs.p_node_ == rhs.p_node_;
    }

  private:
    explicit tree_iterator_base(typename node::pointer p_node)
    : p_node_(p_node)
    {
    }

    bool nil() const
    {
      return p_node_ == nullptr || p_node_->p_left == p_node_;
    }

    U& value() const
    {
      return p_node_->data.first();
    }

    void move_to_parent()
    {
      p_node_ = p_node_->p_parent;
    }

    void move_left()
    {
      p_node_ = p_node_->p_left;
    }

    void move_right()
    {
      p_node_ = p_node_->p_right;
    }

  private:
    typename node::pointer p_node_;
  };

  template <typename BinaryTreeIterator>
  class iterator_base
    : public iterator_facade<
        iterator_base<BinaryTreeIterator>,
        std::bidirectional_iterator_tag,
        typename std::iterator_traits<BinaryTreeIterator>::value_type>
  {
  private:
    friend iterator_facade<
      iterator_base<BinaryTreeIterator>,
      std::bidirectional_iterator_tag,
      typename std::iterator_traits<BinaryTreeIterator>::value_type>;

  public:
    explicit iterator_base(BinaryTreeIterator position = BinaryTreeIterator())
    : position_(position)
    {
    }

    template <
      typename OtherIterator,
      typename = typename std::enable_if<
        std::is_convertible<OtherIterator, BinaryTreeIterator>::value>::type>
    iterator_base(const iterator_base<OtherIterator>& other)
    : iterator_base(static_cast<BinaryTreeIterator>(other.base()))
    {
    }

    BinaryTreeIterator base() const
    {
      return position_;
    }

    friend bool operator==(const iterator_base& lhs, const iterator_base& rhs)
    {
      return lhs.position_ == rhs.position_;
    }

  private:
    typename iterator_base::reference value() const
    {
      return *position_;
    }

    void move_forward()
    {
      position_ = successor(position_);
    }

    void move_back()
    {
      if (!position_)
        position_ = maximum(BinaryTreeIterator(position_.p_node_->p_right));
      else
        position_ = predecessor(position_);
    }

  private:
    BinaryTreeIterator position_;
  };

protected:
  using tree_category = unbalanced_binary_tree_tag;

  using tree_iterator = tree_iterator_base<T>;
  using const_tree_iterator = tree_iterator_base<const T>;

  using value_type = T;
  using reference = T&;
  using const_reference = const T&;
  using iterator = iterator_base<tree_iterator>;
  using const_iterator = iterator_base<const_tree_iterator>;
  using difference_type =
    typename std::iterator_traits<tree_iterator>::difference_type;
  using size_type = typename std::make_unsigned<difference_type>::type;

  using allocator_type = Allocator;

protected:
  explicit binary(const allocator_type& allocator = allocator_type())
  : allocator_type(allocator)
  , allocated_nodes_count_(0)
  , p_nil_(new_node_(value_type(), nullptr, nullptr, nullptr))
  {
    p_nil_->p_left = p_nil_;
    p_nil_->p_right = p_nil_;
  }

  binary(const binary& other)
  : binary(other,
           std::allocator_traits<
             allocator_type>::select_on_container_copy_construction(other))
  {
  }

  binary(binary&& other)
  : binary(other.get_allocator())
  {
    swap_(other, std::true_type());
  }

  explicit binary(const binary& other, const allocator_type& allocator)
  : binary(allocator)
  {
    p_nil_->p_right = copy_subtree_(other.root(), p_nil_);
  }

  binary(binary&& other, const allocator_type& allocator)
  : binary(allocator)
  {
    if (allocator == other.get_allocator())
    {
      swap_(other, std::false_type());
    }
    else
    {
      binary tmp(other, allocator);
      swap_(tmp, std::false_type());
    }
  }

  binary(const initializer_tree<T>& init, const allocator_type& allocator)
  : binary(allocator)
  {
    p_nil_->p_right = copy_subtree_(init.root(), p_nil_);
  }

  ~binary()
  {
    assert((p_nil_ == nullptr) == (allocated_nodes_count_ == 0));

    if (p_nil_ == nullptr)
      return;

    clear();

    delete_node_(p_nil_);
  }

  allocator_type get_allocator() const
  {
    return *this;
  }

  tree_iterator root()
  {
    assert(p_nil_ != nullptr);

    return tree_iterator(p_nil_->p_right);
  }

  const_tree_iterator root() const
  {
    assert(p_nil_ != nullptr);

    return const_tree_iterator(p_nil_->p_right);
  }

  tree_iterator nil()
  {
    return tree_iterator(p_nil_);
  }

  const_tree_iterator nil() const
  {
    return const_tree_iterator(p_nil_);
  }

  iterator begin()
  {
    return iterator(minimum(root()));
  }

  const_iterator begin() const
  {
    return const_iterator(minimum(root()));
  }

  iterator end()
  {
    return iterator(nil());
  }

  const_iterator end() const
  {
    return const_iterator(nil());
  }

  binary& operator=(const binary& other)
  {
    copy_assignment_(
      other,
      typename std::allocator_traits<
        allocator_type>::propagate_on_container_copy_assignment());

    return *this;
  }

  binary& operator=(binary&& other)
  {
    move_assignment_(
      std::move(other),
      typename std::allocator_traits<
        allocator_type>::propagate_on_container_move_assignment());

    return *this;
  }

  template <typename... Args>
  tree_iterator emplace_left(const_tree_iterator position, Args&&... args)
  {
    const auto p_node = position.p_node_;

    if (!position)
    {
      assert(!root());

      p_node->p_right = new_node_(
        value_type(std::forward<Args>(args)...), p_node, p_nil_, p_nil_);

      return tree_iterator(p_node->p_right);
    }

    assert(!left(position));

    p_node->p_left = new_node_(
      value_type(std::forward<Args>(args)...), p_node, p_nil_, p_nil_);

    return tree_iterator(p_node->p_left);
  }

  template <typename... Args>
  tree_iterator emplace_right(const_tree_iterator position, Args&&... args)
  {
    const auto p_node = position.p_node_;

    if (!position)
    {
      assert(!root());

      p_node->p_right = new_node_(
        value_type(std::forward<Args>(args)...), p_node, p_nil_, p_nil_);

      return tree_iterator(p_node->p_right);
    }

    assert(!right(position));

    p_node->p_right = new_node_(
      value_type(std::forward<Args>(args)...), p_node, p_nil_, p_nil_);

    return tree_iterator(p_node->p_right);
  }

  void erase(const_tree_iterator position, const_tree_iterator hint)
  {
    // Sometimes this is not true! investigate (avl tree rebalancing)!
    // assert(!!position && !!left(position) && !!right(position));
    assert(!!hint && (!left(hint) || !right(hint)));
    assert(successor(position) == hint || predecessor(position) == hint);

    const auto p_x = position.p_node_;
    const auto p_y = hint.p_node_;

    const auto p_y_parent = p_y->p_parent;

    assert((p_y_parent->p_left == p_y) != (p_y_parent->p_right == p_y));

    const auto p_y_child = p_y->p_left != p_nil_ ? p_y->p_left : p_y->p_right;

    if (p_y_parent->p_left == p_y)
      p_y_parent->p_left = p_y_child;
    else
      p_y_parent->p_right = p_y_child;

    p_y_child->p_parent = p_y_parent;

    p_y->p_left = p_x->p_left;
    p_y->p_right = p_x->p_right;
    p_y->p_parent = p_x->p_parent;

    const auto p_x_parent = p_x->p_parent;

    assert((p_x_parent->p_left == p_x) != (p_x_parent->p_right == p_x));

    if (p_x_parent->p_left == p_x)
      p_x_parent->p_left = p_y;
    else
      p_x_parent->p_right = p_y;

    p_x->p_right->p_parent = p_y;
    p_x->p_left->p_parent = p_y;

    swap_metadata_(p_x->data, p_y->data);

    delete_node_(p_x);
  }

  void erase(const_tree_iterator position)
  {
    assert(!!position);

    assert(!left(position) || !right(position));

    const auto p_x = position.p_node_;

    const auto p_x_parent = p_x->p_parent;

    assert((p_x_parent->p_left == p_x) != (p_x_parent->p_right == p_x));

    const auto p_x_child = p_x->p_left != p_nil_ ? p_x->p_left : p_x->p_right;

    if (p_x_parent->p_left == p_x)
      p_x_parent->p_left = p_x_child;
    else
      p_x_parent->p_right = p_x_child;

    p_x_child->p_parent = p_x_parent;

    delete_node_(p_x);
  }

  void clear()
  {
    assert(p_nil_ != nullptr);
    assert(allocated_nodes_count_ >= 1);

    auto it = begin_postorder_depth_first_search(root());
    auto it_end = end_postorder_depth_first_search(nil());

    while (it != it_end)
    {
      delete_node_((it++).base().p_node_);
    }

    p_nil_->p_right = p_nil_;
  }

  void swap(binary& other)
  {
    swap_(other,
          typename std::allocator_traits<
            allocator_type>::propagate_on_container_swap());
  }

  size_type size() const
  {
    assert(allocated_nodes_count_ >= 1);

    return allocated_nodes_count_ - 1;
  }

  size_type max_size() const
  {
    using node_allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

    return node_allocator_type(*this).max_size();
  }

  bool empty() const
  {
    return size() == 0;
  }

  static auto metadata(const_tree_iterator position)
    -> decltype(position.p_node_->data.second())
  {
    return position.p_node_->data.second();
  }

  tree_iterator iterator_const_cast(const_tree_iterator x)
  {
    return tree_iterator(x.p_node_);
  }

  template <typename U>
  static tree_iterator_base<U>
  root_from_nil(tree_iterator_base<U> nil_position)
  {
    assert(!nil_position);

    return tree_iterator(nil_position.p_node_->p_right);
  }

  // $   |            |   $
  // $   x            y   $
  // $  / \          / \  $
  // $ a   y   =>   x   c $
  // $    / \      / \    $
  // $   b   c    a   b   $
  tree_iterator rotate_left(const_tree_iterator x)
  {
    assert(right(x) != nil());

    const auto p_x = x.p_node_;
    const auto p_y = p_x->p_right;

    p_x->p_right = p_y->p_left;
    p_y->p_left->p_parent = p_x;

    p_y->p_parent = p_x->p_parent;

    if (p_x == p_x->p_parent->p_left)
      p_x->p_parent->p_left = p_y;
    else
      p_x->p_parent->p_right = p_y;

    p_y->p_left = p_x;
    p_x->p_parent = p_y;

    return tree_iterator(p_y);
  }

  // $     |        |     $
  // $     x        y     $
  // $    / \      / \    $
  // $   y   c => a   x   $
  // $  / \          / \  $
  // $ a   b        b   c $
  tree_iterator rotate_right(const_tree_iterator x)
  {
    assert(left(x) != nil());

    const auto p_x = x.p_node_;
    const auto p_y = p_x->p_left;

    p_x->p_left = p_y->p_right;
    p_y->p_right->p_parent = p_x;

    p_y->p_parent = p_x->p_parent;

    if (p_x == p_x->p_parent->p_right)
      p_x->p_parent->p_right = p_y;
    else
      p_x->p_parent->p_left = p_y;

    p_y->p_right = p_x;
    p_x->p_parent = p_y;

    return tree_iterator(p_y);
  }

private:
  typename node::pointer new_node_(const_reference v,
                                   typename node::pointer p_parent,
                                   typename node::pointer p_left,
                                   typename node::pointer p_right)
  {
    const auto p_new_node =
      new_object<node>(get_allocator(), v, p_parent, p_left, p_right);

    ++allocated_nodes_count_;

    return p_new_node;
  }

  void delete_node_(typename node::pointer p_node)
  {
    --allocated_nodes_count_;

    delete_object<node>(get_allocator(), p_node);
  }

  template <typename ConstBinaryTreeIterator>
  typename node::pointer copy_subtree_(ConstBinaryTreeIterator x,
                                       typename node::pointer p_target_parent)
  {
    if (!x)
      return p_nil_;

    const auto p_node = new_node_(*x, p_target_parent, nullptr, nullptr);

    p_node->p_left = copy_subtree_(left(x), p_node);
    p_node->p_right = copy_subtree_(right(x), p_node);

    return p_node;
  }

  typename node::pointer copy_subtree_(const_tree_iterator x,
                                       typename node::pointer p_target_parent)
  {
    if (!x)
      return p_nil_;

    const auto p_node = new_node_(*x, p_target_parent, nullptr, nullptr);

    p_node->p_left = copy_subtree_(left(x), p_node);
    p_node->p_right = copy_subtree_(right(x), p_node);

    copy_metadata_(x.p_node_->data, p_node->data);

    return p_node;
  }

  void copy_assignment_(const binary& other, std::true_type)
  {
    binary tmp(other);
    swap_(tmp, std::true_type());
  }

  void copy_assignment_(const binary& other, std::false_type)
  {
    binary tmp(other, get_allocator());
    swap_(tmp, std::true_type());
  }

  void move_assignment_(binary&& other, std::true_type)
  {
    binary tmp(std::move(other));
    swap_(tmp, std::true_type());
  }

  void move_assignment_(binary&& other, std::false_type)
  {
    binary tmp(std::move(other), get_allocator());
    swap_(tmp, std::true_type());
  }

  void swap_(binary& other, std::true_type)
  {
    std::swap(static_cast<allocator_type&>(*this),
              static_cast<allocator_type&>(other));

    std::swap(p_nil_, other.p_nil_);
    std::swap(allocated_nodes_count_, other.allocated_nodes_count_);
  }

  void swap_(binary& other, std::false_type)
  {
    assert(get_allocator() == other.get_allocator());

    std::swap(p_nil_, other.p_nil_);
    std::swap(allocated_nodes_count_, other.allocated_nodes_count_);
  }

  template <typename U, typename V>
  void swap_metadata_(pair_or_single<U, V>& lhs, pair_or_single<U, V>& rhs)
  {
    std::swap(lhs.second(), rhs.second());
  }

  template <typename U>
  void swap_metadata_(pair_or_single<U, void>& lhs,
                      pair_or_single<U, void>& rhs)
  {
  }

  template <typename U, typename V>
  void copy_metadata_(pair_or_single<U, V>& source,
                      pair_or_single<U, V>& target)
  {
    target.second() = source.second();
  }

  template <typename U>
  void copy_metadata_(pair_or_single<U, void>&, pair_or_single<U, void>&)
  {
  }

public:
  size_type allocated_nodes_count_;
  typename node::pointer p_nil_;
};

} // mixin

class Binary
{
public:
  template <typename T,
            typename M,
            typename Allocator,
            template <typename, typename, typename> class Base>
  using type = mixin::binary<T, M, Allocator, Base>;
};

} // binary_tree

} // dst
