
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

#include <algorithm> // std::min
#include <cassert>
#include <iterator>
#include <limits>
#include <memory> // std::allocator_traits
#include <type_traits>
#include <utility>

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
  static_assert(std::is_same<T, typename Allocator::value_type>::value,
                "Allocator's `value_type` must be `T`");

  struct node;

  using node_pointer = typename std::allocator_traits<
    Allocator>::template rebind_traits<node>::pointer;

  struct links
  {
    node_pointer p_parent;
    node_pointer p_left;
    node_pointer p_right;
  };

  struct node
  {
    template <typename... Args>
    node(node_pointer p_parent,
         node_pointer p_left,
         node_pointer p_right,
         Args&&... args)
    : value(std::forward<Args>(args)...)
    , data({p_parent, p_left, p_right})
    {
    }

    node_pointer& parent()
    {
      return data.first().p_parent;
    }

    node_pointer& left()
    {
      return data.first().p_left;
    }

    node_pointer& right()
    {
      return data.first().p_right;
    }

    T value;
    pair_or_single<links, M> data;
  };

  template <typename U>
  class tree_iterator_base
    : public iterator_facade<tree_iterator_base<U>, binary_tree_iterator_tag, U>
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
    explicit tree_iterator_base(node_pointer p_node)
    : p_node_(p_node)
    {
    }

    bool nil() const
    {
      return p_node_ == nullptr || p_node_->left() == p_node_;
    }

    U& value() const
    {
      return p_node_->value;
    }

    void move_to_parent()
    {
      p_node_ = p_node_->parent();
    }

    void move_left()
    {
      p_node_ = p_node_->left();
    }

    void move_right()
    {
      p_node_ = p_node_->right();
    }

  private:
    node_pointer p_node_;
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
        position_ = maximum(BinaryTreeIterator(position_.p_node_->right()));
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
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer =
    typename std::allocator_traits<Allocator>::const_pointer;
  using iterator = iterator_base<tree_iterator>;
  using const_iterator = iterator_base<const_tree_iterator>;
  using difference_type =
    typename std::iterator_traits<tree_iterator>::difference_type;
  using size_type = typename std::make_unsigned<difference_type>::type;

  using allocator_type = Allocator;

protected:
  binary()
  : allocator_type()
  , size_(0)
  , p_nil_()
  {
    p_nil_ = new_nil_node_();
  }

  explicit binary(const allocator_type& allocator)
  : allocator_type(allocator)
  , size_(0)
  , p_nil_()
  {
    p_nil_ = new_nil_node_();
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
    p_nil_->right() = copy_subtree_(other.root(), p_nil_);
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
      p_nil_->right() = move_subtree_(other.root(), p_nil_);
    }
  }

  binary(const initializer_tree<T>& init, const allocator_type& allocator)
  : binary(allocator)
  {
    p_nil_->right() = copy_subtree_(init.root(), p_nil_);
  }

  ~binary() noexcept(
    std::is_nothrow_destructible<decltype(p_nil_->data)>::value)
  {
    if (p_nil_ == nullptr)
      return;

    clear();

    delete_nil_node_(p_nil_,
                     std::is_nothrow_destructible<decltype(p_nil_->data)>());
  }

  allocator_type get_allocator() const
  {
    return *this;
  }

  tree_iterator root()
  {
    assert(p_nil_ != nullptr);

    return tree_iterator(p_nil_->right());
  }

  const_tree_iterator root() const
  {
    assert(p_nil_ != nullptr);

    return const_tree_iterator(p_nil_->right());
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

      p_node->right() =
        new_node_(p_node, p_nil_, p_nil_, std::forward<Args>(args)...);

      return tree_iterator(p_node->right());
    }

    assert(!left(position));

    p_node->left() =
      new_node_(p_node, p_nil_, p_nil_, std::forward<Args>(args)...);

    return tree_iterator(p_node->left());
  }

  template <typename... Args>
  tree_iterator emplace_right(const_tree_iterator position, Args&&... args)
  {
    const auto p_node = position.p_node_;

    if (!position)
    {
      assert(!root());

      p_node->right() =
        new_node_(p_node, p_nil_, p_nil_, std::forward<Args>(args)...);

      return tree_iterator(p_node->right());
    }

    assert(!right(position));

    p_node->right() =
      new_node_(p_node, p_nil_, p_nil_, std::forward<Args>(args)...);

    return tree_iterator(p_node->right());
  }

  void erase(const_tree_iterator position, const_tree_iterator sub)
  {
    assert(!!position && !!left(position) && !!right(position));
    assert(!!sub && (!left(sub) || !right(sub)));
    assert(successor(position) == sub || predecessor(position) == sub);

    const auto p_x = position.p_node_;
    const auto p_y = sub.p_node_;

    const auto p_y_parent = p_y->parent();

    assert((p_y_parent->left() == p_y) != (p_y_parent->right() == p_y));

    const auto p_y_child = p_y->left() != p_nil_ ? p_y->left() : p_y->right();

    if (p_y_parent->left() == p_y)
      p_y_parent->left() = p_y_child;
    else
      p_y_parent->right() = p_y_child;

    p_y_child->parent() = p_y_parent;

    p_y->left() = p_x->left();
    p_y->right() = p_x->right();
    p_y->parent() = p_x->parent();

    const auto p_x_parent = p_x->parent();

    assert((p_x_parent->left() == p_x) != (p_x_parent->right() == p_x));

    if (p_x_parent->left() == p_x)
      p_x_parent->left() = p_y;
    else
      p_x_parent->right() = p_y;

    p_x->right()->parent() = p_y;
    p_x->left()->parent() = p_y;

    swap_metadata_(p_x->data, p_y->data);

    delete_node_(p_x);
  }

  void erase(const_tree_iterator position)
  {
    assert(!!position);

    assert(!left(position) || !right(position));

    const auto p_x = position.p_node_;

    const auto p_x_parent = p_x->parent();

    assert((p_x_parent->left() == p_x) != (p_x_parent->right() == p_x));

    const auto p_x_child = p_x->left() != p_nil_ ? p_x->left() : p_x->right();

    if (p_x_parent->left() == p_x)
      p_x_parent->left() = p_x_child;
    else
      p_x_parent->right() = p_x_child;

    p_x_child->parent() = p_x_parent;

    delete_node_(p_x);
  }

  void clear()
  {
    assert(p_nil_ != nullptr);

    auto it = begin_postorder_depth_first_search(root());
    auto it_end = end_postorder_depth_first_search(nil());

    while (it != it_end)
    {
      delete_node_((it++).base().p_node_);
    }

    p_nil_->right() = p_nil_;
  }

  void swap(binary& other)
  {
    swap_(other,
          typename std::allocator_traits<
            allocator_type>::propagate_on_container_swap());
  }

  size_type size() const
  {
    return size_;
  }

  size_type max_size() const
  {
    using node_allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

    node_allocator_type node_allocator(*this);

    return std::min<size_type>(
      std::allocator_traits<node_allocator_type>::max_size(node_allocator),
      std::numeric_limits<difference_type>::max());
  }

  bool empty() const
  {
    return size() == 0;
  }

  static typename ref_or_void<M>::type metadata(const_tree_iterator position)
  {
    return position.p_node_->data.second();
  }

  tree_iterator iterator_const_cast(const_tree_iterator x)
  {
    return tree_iterator(x.p_node_);
  }

  template <typename U>
  static tree_iterator_base<U> root_from_nil(tree_iterator_base<U> nil_position)
  {
    assert(!nil_position);

    return tree_iterator(nil_position.p_node_->right());
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
    const auto p_y = p_x->right();

    p_x->right() = p_y->left();
    p_y->left()->parent() = p_x;

    p_y->parent() = p_x->parent();

    if (p_x == p_x->parent()->left())
      p_x->parent()->left() = p_y;
    else
      p_x->parent()->right() = p_y;

    p_y->left() = p_x;
    p_x->parent() = p_y;

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
    const auto p_y = p_x->left();

    p_x->left() = p_y->right();
    p_y->right()->parent() = p_x;

    p_y->parent() = p_x->parent();

    if (p_x == p_x->parent()->right())
      p_x->parent()->right() = p_y;
    else
      p_x->parent()->left() = p_y;

    p_y->right() = p_x;
    p_x->parent() = p_y;

    return tree_iterator(p_y);
  }

private:
  node_pointer new_nil_node_()
  {
    using node_allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

    node_allocator_type node_allocator(*this);

    const auto p_nil = node_allocator.allocate(1);

    try
    {
      memory::construct(
        get_allocator(), p_nil->data, links{nullptr, p_nil, p_nil});
    }
    catch (...)
    {
      node_allocator.deallocate(p_nil, 1);

      throw;
    }

    return p_nil;
  }

  void delete_nil_node_(node_pointer p_node, std::true_type) noexcept
  {
    using node_allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

    node_allocator_type node_allocator(get_allocator());

    memory::destroy(get_allocator(), p_nil_->data);

    node_allocator.deallocate(p_nil_, 1);
  }

  void delete_nil_node_(node_pointer p_node, std::false_type) noexcept(false)
  {
    using node_allocator_type =
      typename std::allocator_traits<Allocator>::template rebind_alloc<node>;

    node_allocator_type node_allocator(get_allocator());

    try
    {
      memory::destroy(get_allocator(), p_nil_->data);
    }
    catch (...)
    {
      node_allocator.deallocate(p_nil_, 1);

      throw;
    }

    node_allocator.deallocate(p_nil_, 1);
  }

  template <typename... Args>
  node_pointer new_node_(node_pointer p_parent,
                         node_pointer p_left,
                         node_pointer p_right,
                         Args&&... args)
  {
    const auto p_new_node = memory::new_object<node>(
      get_allocator(), p_parent, p_left, p_right, std::forward<Args>(args)...);

    ++size_;

    return p_new_node;
  }

  void delete_node_(node_pointer p_node)
  {
    --size_;

    memory::delete_object<node>(get_allocator(), p_node);
  }

  template <typename ConstBinaryTreeIterator>
  node_pointer copy_subtree_(ConstBinaryTreeIterator x,
                             node_pointer p_target_parent)
  {
    if (!x)
      return p_nil_;

    const auto p_node = new_node_(p_target_parent, nullptr, nullptr, *x);

    p_node->left() = copy_subtree_(left(x), p_node);
    p_node->right() = copy_subtree_(right(x), p_node);

    return p_node;
  }

  node_pointer copy_subtree_(const_tree_iterator x,
                             node_pointer p_target_parent)
  {
    if (!x)
      return p_nil_;

    const auto p_node = new_node_(p_target_parent, nullptr, nullptr, *x);

    p_node->left() = copy_subtree_(left(x), p_node);
    p_node->right() = copy_subtree_(right(x), p_node);

    copy_metadata_(x.p_node_->data, p_node->data);

    return p_node;
  }

  node_pointer move_subtree_(tree_iterator x, node_pointer p_target_parent)
  {
    if (!x)
      return p_nil_;

    const auto p_node =
      new_node_(p_target_parent, nullptr, nullptr, std::move(*x));

    p_node->left() = move_subtree_(left(x), p_node);
    p_node->right() = move_subtree_(right(x), p_node);

    copy_metadata_(x.p_node_->data, p_node->data);

    return p_node;
  }

  void copy_assignment_(const binary& other, std::true_type)
  {
    binary tmp(other, other.get_allocator());
    swap_(tmp, std::true_type());
  }

  void copy_assignment_(const binary& other, std::false_type)
  {
    binary tmp(other, get_allocator());
    swap_(tmp, std::true_type());
  }

  void move_assignment_(binary&& other, std::true_type)
  {
    binary tmp(std::move(other), other.get_allocator());
    swap_(tmp, std::true_type());
  }

  void move_assignment_(binary&& other, std::false_type)
  {
    binary tmp(std::move(other), get_allocator());
    swap_(tmp, std::true_type());
  }

  void swap_(binary& other, std::true_type)
  {
    allocator_type& a = *this;
    allocator_type& b = other;

    std::swap(a, b);

    std::swap(p_nil_, other.p_nil_);
    std::swap(size_, other.size_);
  }

  void swap_(binary& other, std::false_type)
  {
    assert(get_allocator() == other.get_allocator());

    std::swap(p_nil_, other.p_nil_);
    std::swap(size_, other.size_);
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
  size_type size_;
  node_pointer p_nil_;
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
