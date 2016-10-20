
//          Copyright Maksym V. Bilinets 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "iterator_facade.h"

#include <queue>
#include <ostream>

namespace dst
{

namespace binary_tree
{

template <
  typename BinaryTreeIterator,
  typename = enable_for_binary_tree_children_iterator<BinaryTreeIterator>>
void write_graphviz(std::ostream& out, BinaryTreeIterator position)
{
  out << "digraph G {" << std::endl;

  std::queue<BinaryTreeIterator> q;
  q.push(position);

  int idx = 0;
  while (!q.empty())
  {
    const auto x = q.front();
    q.pop();

    out << "\t" << idx << " [label=\"" << *x << "\"]" << std::endl;
    if (!!left(x))
    {
      q.push(left(x));
      out << "\t" << idx << ":w->" << idx + q.size() << std::endl;
    }

    if (!!right(x))
    {
      q.push(right(x));
      out << "\t" << idx << ":e->" << idx + q.size() << std::endl;
    }

    ++idx;
  }

  out << "}" << std::endl;
}

} // binary_tree

} // dst
