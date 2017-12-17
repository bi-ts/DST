
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include "trees_generator.h"

namespace dst_test
{
namespace
{
using trees_container = std::vector<dst::binary_tree::initializer_tree<int>>;

trees_container generate_balanced_trees_(const trees_container& trees_h_minus_2,
                                         const trees_container& trees_h_minus_1)
{
  assert(!trees_h_minus_1.empty());
  assert(!trees_h_minus_2.empty());
  assert(!trees_h_minus_1.front().empty());

  const auto h = *trees_h_minus_1.front().root() + 1;

  assert((h == 2 && trees_h_minus_2.front().empty()) ||
         (*trees_h_minus_2.front().root() == h - 2));

  trees_container tree_h;

  for (auto& t1 : trees_h_minus_1)
  {
    for (auto& t2 : trees_h_minus_2)
    {
      tree_h.push_back({t1, h, t2});
      tree_h.push_back({t2, h, t1});
    }
  }

  for (auto& t1 : trees_h_minus_1)
  {
    for (auto& t2 : trees_h_minus_1)
    {
      tree_h.push_back({t1, h, t2});
    }
  }

  return tree_h;
}
}
} // dst_test

dst::binary_tree::initializer_tree<int>
dst_test::generate_fibonacci_tree(std::size_t h)
{

  if (h == 0)
    return {};

  if (h == 1)
    return {1};

  dst::binary_tree::initializer_tree<int> tree_left = {1};
  dst::binary_tree::initializer_tree<int> tree_right = {};

  for (int i = 1; i != h; ++i)
  {
    tree_right = {tree_left, i + 1, tree_right};
    std::swap(tree_left, tree_right);
  }

  return tree_left;
}

std::vector<dst::binary_tree::initializer_tree<int>>
dst_test::generate_balanced_trees(std::size_t h)
{

  if (h == 0)
    return {{}};

  if (h == 1)
    return {{1}};

  trees_container trees_a = {{}};
  trees_container trees_b = {{1}};

  for (std::size_t current_h = 1; current_h != h; ++current_h)
  {
    trees_a = generate_balanced_trees_(trees_a, trees_b);
    trees_a.swap(trees_b);
  }

  return trees_b;
}

std::size_t dst_test::number_of_balanced_trees(std::size_t h)
{
  if (h <= 1)
    return 1;

  const auto n = number_of_balanced_trees(h - 1);

  return n * (2 * number_of_balanced_trees(h - 2) + n);
}

