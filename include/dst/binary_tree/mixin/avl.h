
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/utility.h>

#include <cassert>
#include <cstdint>

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
  explicit avl(const allocator_type& allocator = allocator_type())
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

    after_insertion(x);

    return x;
  }

  template <typename... Args>
  tree_iterator emplace_right(const_tree_iterator position, Args&&... args)
  {
    const auto x = base::emplace_right(position, std::forward<Args>(args)...);

    after_insertion(x);

    return x;
  }

  void erase(const_tree_iterator position, const_tree_iterator hint)
  {
    before_erasing(hint);

    base::erase(position, hint);
  }

  void erase(const_tree_iterator position)
  {
    before_erasing(position);

    base::erase(position);
  }

  static auto metadata(const_tree_iterator x)
    -> decltype(base::metadata(x).second())
  {
    return base::metadata(x).second();
  }

public:
  static std::int8_t balance_factor(const_tree_iterator x)
  {
    return base::metadata(x).first();
  }

private:
  std::int8_t& bf(const_tree_iterator x)
  {
    return base::metadata(x).first();
  }

  void after_insertion(const_tree_iterator x)
  {
    auto n = x;
    auto p = parent(n);

    while (!!p)
    {
      if (rebalance(p, n))
        break;

      if (bf(p) == 0)
        break;

      n = p;
      p = parent(n);
    }
  }

  void before_erasing(const_tree_iterator x)
  {
    auto p = parent(x);

    while (!!p)
    {
      if (rebalance(p, sibling(x)))
        break;

      if (bf(p) != 0)
        break;

      x = p;
      p = parent(x);
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

  bool rebalance(const_tree_iterator p, const_tree_iterator n)
  {
    assert(!!p);
    assert(left(p) == n || right(p) == n);

    if (n == left(p))
    {
      assert(n == left(p));

      assert(bf(p) >= -1 && bf(p) <= 1);

      --bf(p);

      assert(bf(p) >= -2 && bf(p) <= 0);

      if (bf(p) == -2)
      {
        if (bf(n) == 1)
        {
          n = rotate_left(n);

          assert(bf(n) >= -2 && bf(n) <= 0);
        }

        p = rotate_right(p);

        assert(bf(p) >= -1 && bf(p) <= 1);

        return true;
      }
    }
    else
    {
      assert(n == right(p));

      assert(bf(p) >= -1 && bf(p) <= 1);

      ++bf(p);

      assert(bf(p) >= 0 && bf(p) <= 2);

      if (bf(p) == 2)
      {
        if (bf(n) == -1)
        {
          n = rotate_right(n);

          assert(bf(n) >= 0 && bf(n) <= 2);
        }

        p = rotate_left(p);

        assert(bf(p) >= -1 && bf(p) <= 1);

        return true;
      }
    }

    return false;
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

