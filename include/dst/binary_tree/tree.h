
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "mixin.h"
#include "mixin/binary.h"

namespace dst
{

namespace binary_tree
{

template <typename T,
          typename Allocator = std::allocator<T>,
          typename... Mixins>
class tree
  : public fold_mixins<Binary, Mixins...>::template type<T, void, Allocator>
{
private:
  using base =
    typename fold_mixins<Binary, Mixins...>::template type<T, void, Allocator>;

  static_assert(is_unbalanced_binary_tree<typename base::tree_category>::value,
                "Must be unbalanced");

public:
  using typename base::tree_iterator;
  using typename base::const_tree_iterator;

  using typename base::value_type;
  using typename base::reference;
  using typename base::iterator;
  using typename base::const_iterator;
  using typename base::const_reference;
  using typename base::difference_type;
  using typename base::size_type;

  using allocator_type = typename base::allocator_type;

public:
  using base::get_allocator;
  using base::root;
  using base::nil;
  using base::begin;
  using base::end;
  using base::emplace_left;
  using base::emplace_right;
  using base::erase;
  using base::clear;
  using base::swap;
  using base::size;
  using base::max_size;
  using base::empty;
  using base::rotate_left;
  using base::rotate_right;

public:
  explicit tree(const allocator_type& allocator = allocator_type())
  : base(allocator)
  {
  }

  explicit tree(const tree& other, const allocator_type& allocator)
  : base(other, allocator)
  {
  }

  tree(tree&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  {
  }

  tree(const initializer_tree<value_type>& init,
       const allocator_type& allocator = allocator_type())
  : base(init, allocator)
  {
  }

  const_tree_iterator croot() const
  {
    return root();
  }

  const_tree_iterator cnil() const
  {
    return nil();
  }

  const_iterator cbegin() const
  {
    return begin();
  }

  const_iterator cend() const
  {
    return end();
  }

  bool operator==(const tree& other) const
  {
    return this->size() == other.size() &&
           std::equal(cbegin(), cend(), other.cbegin());
  }

  bool operator!=(const tree& other) const
  {
    return !(*this == other);
  }

  tree_iterator insert_left(const_tree_iterator position, const_reference v)
  {
    return emplace_left(position, v);
  }

  tree_iterator insert_right(const_tree_iterator position, const_reference v)
  {
    return emplace_right(position, v);
  }
};

} // binary_tree

} // dst

