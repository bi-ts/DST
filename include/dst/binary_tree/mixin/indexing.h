
//          Copyright Maksym V. Bilinets 2015 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/binary_tree/initializer_tree.h>
#include <dst/binary_tree/mixin.h>
#include <dst/utility.h>

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
class indexing : public Base<T, pair_or_single<std::size_t, M>, Allocator>
{
private:
  using base = Base<T, pair_or_single<std::size_t, M>, Allocator>;

  static_assert(is_unbalanced_binary_tree<typename base::tree_category>::value,
                "Base mixin must be unbalanced");

protected:
  using typename base::const_tree_iterator;
  using typename base::tree_iterator;

  using typename base::const_reference;
  using typename base::reference;

  using typename base::const_iterator;
  using typename base::iterator;

  using typename base::difference_type;
  using typename base::size_type;

  using allocator_type = typename base::allocator_type;

public:
  static std::size_t subtree_size(const_tree_iterator x)
  {
    return rank(x);
  }

  static std::size_t subtree_size(const_iterator x)
  {
    return subtree_size(x.base());
  }

  const_iterator element_at(size_type index) const
  {
    auto position = base::root();

    assert(subtree_size(position) > index);

    while (subtree_size(left(position)) != index)
    {
      const auto left_size = subtree_size(left(position));

      if (left_size < index)
      {
        index -= (left_size + 1);
        position = right(position);
      }
      else
      {
        assert(left_size > index);
        position = left(position);
      }
    }

    return const_iterator(position);
  }

  iterator element_at(size_type index)
  {
    return iterator(base::iterator_const_cast(
      const_cast<const indexing*>(this)->element_at(index).base()));
  }

  const_reference at(size_type index) const
  {
    return *element_at(index);
  }

  reference at(size_type index)
  {
    return *element_at(index);
  }

  const_reference operator[](size_type index) const
  {
    return at(index);
  }

  reference operator[](size_type index)
  {
    return at(index);
  }

  size_type index(const_tree_iterator position) const
  {
    size_type index = subtree_size(left(position));

    for (auto p = parent(position); !!p;
         std::swap(position, p), p = parent(position))
    {
      if (right(p) == position)
      {
        index += subtree_size(left(p)) + 1;
      }
    }

    return index;
  }

  size_type index(const_iterator position) const
  {
    return index(position.base());
  }

protected:
  indexing()
  : base()
  {
  }

  explicit indexing(const allocator_type& allocator)
  : base(allocator)
  {
  }

  explicit indexing(const indexing& other, const allocator_type& allocator)
  : base(other, allocator)
  {
  }

  indexing(indexing&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  {
  }

  indexing(const initializer_tree<T>& init, const allocator_type& allocator)
  : base(init, allocator)
  {
    const auto it_end = base::end();
    for (auto it = base::begin(); it != it_end; ++it)
    {
      for (auto tit = it.base(); !!tit; ++tit)
      {
        ++rank(tit);
      }
    }
  }

  template <typename... Args>
  tree_iterator emplace_left(const_tree_iterator position, Args&&... args)
  {
    const auto new_it =
      base::emplace_left(position, std::forward<Args>(args)...);

    for (auto it = new_it; !!it; ++it)
    {
      ++rank(it);
    }

    return new_it;
  }

  template <typename... Args>
  tree_iterator emplace_right(const_tree_iterator position, Args&&... args)
  {
    const auto new_it =
      base::emplace_right(position, std::forward<Args>(args)...);

    for (auto it = new_it; !!it; ++it)
    {
      ++rank(it);
    }

    return new_it;
  }

  void erase(const_tree_iterator position, const_tree_iterator sub)
  {
    for (auto it = sub; !!it; ++it)
    {
      --rank(it);
    }

    base::erase(position, sub);
  }

  void erase(const_tree_iterator position)
  {
    for (auto it = position; !!it; ++it)
    {
      --rank(it);
    }

    base::erase(position);
  }

  // $   |            |   $
  // $   x            y'  $
  // $  / \          / \  $
  // $ a   y   =>   x'  c $
  // $    / \      / \    $
  // $   b   c    a   b   $
  tree_iterator rotate_left(const_tree_iterator x)
  {
    const auto rank_a = rank(left(x));
    const auto rank_b = rank(left(right(x)));
    const auto rank_c = rank(right(right(x)));

    tree_iterator y = base::rotate_left(x);

    rank(x) = rank_a + rank_b + 1;
    rank(y) = rank(x) + rank_c + 1;

    return y;
  }

  // $     |        |     $
  // $     x        y'    $
  // $    / \      / \    $
  // $   y   c => a   x'  $
  // $  / \          / \  $
  // $ a   b        b   c $
  tree_iterator rotate_right(const_tree_iterator x)
  {
    const auto rank_a = rank(left(left(x)));
    const auto rank_b = rank(right(left(x)));
    const auto rank_c = rank(right(x));

    tree_iterator y = base::rotate_right(x);

    rank(x) = rank_b + rank_c + 1;
    rank(y) = rank_a + rank(x) + 1;

    return y;
  }

  static typename ref_or_void<M>::type metadata(const_tree_iterator x)
  {
    return base::metadata(x).second();
  }

private:
  static std::size_t& rank(const_tree_iterator x)
  {
    return base::metadata(x).first();
  }
};

} // mixin

class Indexing
{
public:
  template <typename T,
            typename M,
            typename Allocator,
            template <typename, typename, typename>
            class Base>
  using type = mixin::indexing<T, M, Allocator, Base>;
};

} // binary_tree

} // dst
