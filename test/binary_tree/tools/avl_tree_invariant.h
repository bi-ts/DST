
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/binary_tree/algorithm.h>
#include <dst/binary_tree/write_graphviz.h>

#include <iostream>
#include <utility>
#include <vector>

namespace dst_test
{
namespace detail
{
template <typename Container, typename BinaryTreeIterator>
int check_avl_subtree_invariant(std::vector<BinaryTreeIterator>& bad_nodes,
                                BinaryTreeIterator x)
{
  if (!x)
    return 0;

  const auto left_height =
    check_avl_subtree_invariant<Container>(bad_nodes, left(x));
  const auto right_height =
    check_avl_subtree_invariant<Container>(bad_nodes, right(x));

  const auto height = std::max(left_height, right_height) + 1;

  const int actual_bf = Container::balance_factor(x);
  const int expected_bf = right_height - left_height;

  if (expected_bf < -1 || expected_bf > 1 || expected_bf != actual_bf)
  {
    bad_nodes.push_back(x);
  }

  return height;
}
} // detail

template <typename Container>
bool avl_invariant_holds(Container container, bool print_to_stdout = true)
{
  std::vector<typename Container::tree_iterator> bad_nodes;
  detail::check_avl_subtree_invariant<Container>(bad_nodes, container.root());

  if (bad_nodes.size() > 0 && print_to_stdout)
  {
    std::cout << "Bad AVL tree:" << std::endl;

    dst::binary_tree::write_graphviz(std::cout,
                                     container.root(),
                                     &Container::balance_factor,
                                     bad_nodes.begin(),
                                     bad_nodes.end());
  }

  return bad_nodes.size() == 0;
}
} // dst_test
