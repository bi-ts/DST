
//          Copyright Maksym V. Bilinets 2015 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
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
int check_indexing_subtree_invariant(std::vector<BinaryTreeIterator>& bad_nodes,
                                     BinaryTreeIterator x)
{
  if (!x)
    return 0;

  const auto left_count =
    check_indexing_subtree_invariant<Container>(bad_nodes, left(x));
  const auto right_count =
    check_indexing_subtree_invariant<Container>(bad_nodes, right(x));

  const auto actual = Container::subtree_size(x);
  const auto expected = left_count + right_count + 1;

  if (actual != expected)
  {
    bad_nodes.push_back(x);
  }

  return actual;
}
} // detail

template <typename Container>
bool indexing_invariant_holds(Container container, bool print_to_stdout = true)
{
  std::vector<typename Container::tree_iterator> bad_nodes;
  detail::check_indexing_subtree_invariant<Container>(bad_nodes,
                                                      container.root());

  if (bad_nodes.size() > 0 && print_to_stdout)
  {
    std::cout << "Bad Indexing tree:" << std::endl;

    dst::binary_tree::write_graphviz(
      std::cout,
      container.root(),
      [](typename Container::tree_iterator x) {
        return Container::subtree_size(x);
      },
      bad_nodes.begin(),
      bad_nodes.end());
  }

  return bad_nodes.size() == 0;
}
} // dst_test
