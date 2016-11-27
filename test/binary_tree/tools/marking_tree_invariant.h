
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
template <typename Container, typename BinaryTreeIterator, typename Flag>
int check_marking_subtree_invariant(std::vector<BinaryTreeIterator>& bad_nodes,
                                    BinaryTreeIterator x,
                                    Flag f)
{
  if (!x)
    return 0;

  const auto left_count =
    check_marking_subtree_invariant<Container>(bad_nodes, left(x), f);
  const auto right_count =
    check_marking_subtree_invariant<Container>(bad_nodes, right(x), f);

  const auto actual = left_count + right_count + Container::marked(x, f);
  const auto expected = Container::marked_nodes(x, f);

  if (expected != actual)
  {
    bad_nodes.push_back(x);
  }

  return actual;
}
} // detail

template <typename Container, typename Flag>
bool marking_invariant_holds(Container container,
                             Flag f,
                             bool print_to_stdout = true)
{
  std::vector<typename Container::tree_iterator> bad_nodes;
  detail::check_marking_subtree_invariant<Container>(
    bad_nodes, container.root(), f);

  if (bad_nodes.size() > 0 && print_to_stdout)
  {
    std::cout << "Bad Marking tree:" << std::endl;

    dst::binary_tree::write_graphviz(std::cout,
                                     container.root(),
                                     [f](typename Container::tree_iterator x)
                                     {
                                       return Container::marked_nodes(x, f);
                                     },
                                     bad_nodes.begin(),
                                     bad_nodes.end());
  }

  return bad_nodes.size() == 0;
}
} // dst_test
