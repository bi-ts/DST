
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include "tools/trees_generator.h"

#include <dst/allocator/global_counter_allocator.h>
#include <dst/binary_tree/balanced_tree.h>

#include <boost/test/unit_test.hpp>

#include <queue>

namespace dst_test
{
using avl_tree =
  dst::binary_tree::balanced_tree<int, dst::global_counter_allocator<int>>;

avl_tree init_avl_tree(const dst::binary_tree::initializer_tree<int>& init)
{
  avl_tree tree;

  std::queue<std::pair<dst::binary_tree::initializer_tree<int>::tree_iterator,
                       avl_tree::tree_iterator>> queue;

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

  const auto trees = generate_balanced_trees(hight);

  BOOST_TEST(trees.size() == number_of_balanced_trees(hight));

  for (auto& t : trees)
  {
    BOOST_TEST(avl_tree::allocator_type::allocated() == 0);

    auto tree = init_avl_tree(t);

    BOOST_TEST(topologically_equal(t.root(), tree.root()));
  }

  BOOST_TEST(avl_tree::allocator_type::allocated() == 0);
}

BOOST_AUTO_TEST_CASE(load_test_erase)
{
  const auto hight = 4;

  const auto trees = generate_balanced_trees(hight);

  BOOST_TEST(trees.size() == number_of_balanced_trees(hight));

  for (auto& t : trees)
  {
    BOOST_TEST(avl_tree::allocator_type::allocated() == 0);

    auto tree = init_avl_tree(t);

    BOOST_TEST(topologically_equal(t.root(), tree.root()));
  }

  BOOST_TEST(avl_tree::allocator_type::allocated() == 0);
}

BOOST_AUTO_TEST_SUITE_END()
}
