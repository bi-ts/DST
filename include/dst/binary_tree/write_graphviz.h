
//          Copyright Maksym V. Bilinets 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "iterator_facade.h"

#include <ostream>
#include <queue>

namespace dst
{

namespace binary_tree
{

template <typename BinaryTreeChildrenIterator,
          typename F,
          typename ForwardIterator = BinaryTreeChildrenIterator*,
          typename = enable_for_binary_tree_children_iterator<
            BinaryTreeChildrenIterator>>
void write_graphviz(std::ostream& out,
                    BinaryTreeChildrenIterator x,
                    const F& f,
                    ForwardIterator from = nullptr,
                    ForwardIterator to = nullptr)
{
  out << "digraph G {" << std::endl;

  std::queue<BinaryTreeChildrenIterator> q;
  q.push(x);

  int idx = 0;
  while (!q.empty())
  {
    const auto x = q.front();
    q.pop();

    out << "\t" << idx << " [label=\"" << f(x) << "\"";

    if (std::find(from, to, x) != to)
    {
      out << ", color=red";
    }

    out << "]" << std::endl;

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

template <typename BinaryTreeChildrenIterator,
          typename = enable_for_binary_tree_children_iterator<
            BinaryTreeChildrenIterator>>
void write_graphviz(
  std::ostream& out,
  BinaryTreeChildrenIterator x,
  BinaryTreeChildrenIterator highlight = BinaryTreeChildrenIterator())
{
  write_graphviz(out,
                 x,
                 [](BinaryTreeChildrenIterator x)
                 {
                   return *x;
                 },
                 &highlight,
                 &highlight + 1);
}

} // binary_tree

} // dst
