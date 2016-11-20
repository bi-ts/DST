
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/utility.h>

#include <cassert>
#include <cstdint>
#include <tuple> // std::tie

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
class avl : public Base<T, pair_or_single<std::int8_t, M>, Allocator>
{
private:
  using base = Base<T, pair_or_single<std::int8_t, M>, Allocator>;

  static_assert(is_unbalanced_binary_tree<typename base::tree_category>::value,
                "Base mixin must be unbalanced");

protected:
  using tree_category = balanced_binary_tree_tag;

  using typename base::tree_iterator;
  using typename base::const_tree_iterator;

  using allocator_type = typename base::allocator_type;

protected:
  avl()
  : base()
  {
  }

  explicit avl(const allocator_type& allocator)
  : base(allocator)
  {
  }

  explicit avl(const avl& other, const allocator_type& allocator)
  : base(other, allocator)
  {
  }

  avl(avl&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  {
  }

  template <typename... Args>
  tree_iterator emplace_left(const_tree_iterator position, Args&&... args)
  {
    const auto x = base::emplace_left(position, std::forward<Args>(args)...);

    after_insertion(parent(x), true);

    return x;
  }

  template <typename... Args>
  tree_iterator emplace_right(const_tree_iterator position, Args&&... args)
  {
    const auto x = base::emplace_right(position, std::forward<Args>(args)...);

    after_insertion(parent(x), false);

    return x;
  }

  void erase(const_tree_iterator position, const_tree_iterator sub)
  {
    assert(!!parent(sub));

    const auto sub_parent = parent(sub);
    const auto left_erasing = left(sub_parent) == sub;
    const auto p = sub_parent == position ? sub : sub_parent;

    base::erase(position, sub);

    after_erasing(p, left_erasing);
  }

  void erase(const_tree_iterator position)
  {
    const auto p = parent(position);
    const auto left_erasing = !!p && left(p) == position;

    base::erase(position);

    after_erasing(p, left_erasing);
  }

  static auto metadata(const_tree_iterator x)
    -> decltype(base::metadata(x).second())
  {
    return base::metadata(x).second();
  }

public:
  static int balance_factor(const_tree_iterator x)
  {
    return base::metadata(x).first();
  }

private:
  std::int8_t& bf(const_tree_iterator x)
  {
    return base::metadata(x).first();
  }

  void after_insertion(const_tree_iterator x, bool left_insertion)
  {
    while (!!x)
    {
      if (left_insertion)
        --bf(x);
      else
        ++bf(x);

      bool height_changed = true;
      std::tie(x, height_changed) = rebalance(x);

      if (height_changed)
        break;

      left_insertion = !!parent(x) && left(parent(x)) == x;
      x = parent(x);
    }
  }

  void after_erasing(const_tree_iterator x, bool left_erasing)
  {
    while (!!x)
    {
      if (left_erasing)
        ++bf(x);
      else
        --bf(x);

      bool height_changed = true;
      std::tie(x, height_changed) = rebalance(x);

      if (!height_changed)
        break;

      left_erasing = !!parent(x) && left(parent(x)) == x;
      x = parent(x);
    }
  }

  // $   |            |   $
  // $   x            y'  $
  // $  / \          / \  $
  // $ a   y   =>   x'  c $
  // $    / \      / \    $
  // $   b   c    a   b   $
  tree_iterator rotate_left(const_tree_iterator x)
  {
    tree_iterator y = base::rotate_left(x);

    bf(x) -= bf(y) * (bf(y) >= 0) + 1;

    bf(y) += bf(x) * (bf(x) < 0) - 1;

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
    tree_iterator y = base::rotate_right(x);

    bf(x) -= bf(y) * (bf(y) < 0) - 1;

    bf(y) += bf(x) * (bf(x) >= 0) + 1;

    return y;
  }

  std::pair<const_tree_iterator, bool> rebalance(const_tree_iterator x)
  {
    assert(!!x);

    if (bf(x) == 2)
    {
      auto y = right(x);

      assert(!!y);

      bool hight_changed = bf(y) != 0;

      if (bf(y) == -1)
      {
        y = rotate_right(y);

        assert(bf(y) >= 0 && bf(y) <= 2);
      }

      x = rotate_left(x);

      assert(bf(x) >= -1 && bf(x) <= 0);

      return std::make_pair(x, hight_changed);
    }
    else if (bf(x) == -2)
    {
      auto y = left(x);

      assert(!!y);

      bool hight_changed = bf(y) != 0;

      if (bf(y) == 1)
      {
        y = rotate_left(y);

        assert(bf(y) >= -2 && bf(y) <= 0);
      }

      x = rotate_right(x);

      assert(bf(x) >= 0 && bf(x) <= 1);

      return std::make_pair(x, hight_changed);
    }

    return std::make_pair(x, bf(x) == 0);
  }
};
} // mixin

class AVL
{
public:
  template <typename T,
            typename M,
            typename Allocator,
            template <typename, typename, typename> class Base>
  using type = mixin::avl<T, M, Allocator, Base>;
};

} // binary_tree

} // dst
