
//          Copyright Maksym V. Bilinets 2015 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include "tools/avl_tree_invariant.h"
#include "tools/trees_generator.h"

#include <dst/allocator/global_counter_allocator.h>
#include <dst/binary_tree/balanced_tree.h>

#include <boost/test/unit_test.hpp>

#include <cstddef> // std::size_t
#include <iostream>
#include <iterator> // std::advance
#include <queue>
#include <utility> // std::pair, std::make_pair

namespace dst_test
{
using avl_tree =
  dst::binary_tree::balanced_tree<int, dst::global_counter_allocator<int>>;

avl_tree init_avl_tree(const dst::binary_tree::initializer_tree<int>& init)
{
  avl_tree tree;

  std::queue<std::pair<dst::binary_tree::initializer_tree<int>::tree_iterator,
                       avl_tree::tree_iterator>>
    queue;

  queue.push(
    std::make_pair(init.root(), tree.insert_left(tree.nil(), *init.root())));

  while (!queue.empty())
  {
    const auto p = queue.front();
    queue.pop();

    const auto l = left(p.first);
    const auto r = right(p.first);

    if (!!l)
      queue.push(std::make_pair(l, tree.insert_left(p.second, *l)));

    if (!!r)
      queue.push(std::make_pair(r, tree.insert_right(p.second, *r)));
  }

  return tree;
}

BOOST_AUTO_TEST_SUITE(test_binary_tree_avl)

BOOST_AUTO_TEST_CASE(load_test_insert)
{
  const auto hight = 4;

  const auto fib_tree = generate_fibonacci_tree(hight);
  const auto trees = generate_balanced_trees(hight);

  BOOST_TEST(trees.size() == number_of_balanced_trees(hight));

  for (auto& t : trees)
  {
    BOOST_TEST(avl_tree::allocator_type::allocated() == 0);

    auto tree = init_avl_tree({t, hight + 1, fib_tree});

    BOOST_TEST(topologically_equal(t.root(), left(tree.root())));
    BOOST_TEST(avl_invariant_holds(tree));

    for (std::size_t i = 0; i < t.size(); ++i)
    {
      auto it1 = tree.begin();
      std::advance(it1, i);

      if (!left(it1.base()))
      {
        auto tree_copy = tree;

        auto it = tree_copy.begin();
        std::advance(it, i);

        tree_copy.insert_left(it.base(), 0);

        const auto invariant_holds = avl_invariant_holds(tree_copy);

        if (!invariant_holds)
        {
          auto it = tree.begin();
          std::advance(it, i);
          const auto x = it.base();

          std::cout << "Preceding buggy `insert_left` operation:" << std::endl;
          dst::binary_tree::write_graphviz(
            std::cout, tree.root(), &avl_tree::balance_factor, &x, &x + 1);

          BOOST_TEST(invariant_holds);

          return;
        }
      }

      if (!right(it1.base()))
      {
        auto tree_copy = tree;

        auto it = tree_copy.begin();
        std::advance(it, i);

        tree_copy.insert_right(it.base(), 0);

        const auto invariant_holds = avl_invariant_holds(tree_copy);

        if (!invariant_holds)
        {
          auto it = tree.begin();
          std::advance(it, i);
          const auto x = it.base();

          std::cout << "Preceding buggy `insert_right` operation:" << std::endl;
          dst::binary_tree::write_graphviz(
            std::cout, tree.root(), &avl_tree::balance_factor, &x, &x + 1);

          BOOST_TEST(invariant_holds);

          return;
        }
      }
    }
  }

  BOOST_TEST(avl_tree::allocator_type::allocated() == 0);
}

BOOST_AUTO_TEST_CASE(load_test_erase)
{
  const auto hight = 4;

  const auto fib_tree = generate_fibonacci_tree(hight);
  const auto trees = generate_balanced_trees(hight);

  BOOST_TEST(trees.size() == number_of_balanced_trees(hight));

  for (auto& t : trees)
  {
    BOOST_TEST(avl_tree::allocator_type::allocated() == 0);

    auto tree = init_avl_tree({t, hight + 1, fib_tree});

    BOOST_TEST(topologically_equal(t.root(), left(tree.root())));
    BOOST_TEST(avl_invariant_holds(tree));

    for (std::size_t i = 0; i < t.size(); ++i)
    {
      auto tree_copy = tree;

      auto it = tree_copy.begin();
      std::advance(it, i);

      if (!left(it.base()) || !right(it.base()))
        tree_copy.erase(it.base());
      else
        tree_copy.erase(it.base(), successor(it.base()));

      const auto invariant_holds = avl_invariant_holds(tree_copy);

      if (!invariant_holds)
      {
        auto it = tree.begin();
        std::advance(it, i);
        const auto x = it.base();

        std::cout << "Preceding buggy `erase` operation:" << std::endl;
        dst::binary_tree::write_graphviz(
          std::cout, tree.root(), &avl_tree::balance_factor, &x, &x + 1);

        BOOST_TEST(invariant_holds);

        return;
      }
    }
  }

  BOOST_TEST(avl_tree::allocator_type::allocated() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
}
