
//          Copyright Maksym V. Bilinets 2015 - 2016.
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

template <
  typename BinaryTreeBranchIterator,
  typename F,
  typename ForwardIterator = BinaryTreeBranchIterator*,
  typename = enable_for_binary_tree_branch_iterator<BinaryTreeBranchIterator>>
void write_graphviz(std::ostream& out,
                    BinaryTreeBranchIterator x,
                    const F& f,
                    ForwardIterator from = nullptr,
                    ForwardIterator to = nullptr)
{
  out << "digraph G {" << std::endl;

  std::queue<BinaryTreeBranchIterator> q;
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

template <
  typename BinaryTreeBranchIterator,
  typename F,
  typename = enable_for_binary_tree_branch_iterator<BinaryTreeBranchIterator>>
void write_graphviz(std::ostream& out,
                    BinaryTreeBranchIterator x,
                    const F& f,
                    BinaryTreeBranchIterator highlight)
{
  write_graphviz(out, x, f, &highlight, &highlight + 1);
}

template <
  typename BinaryTreeBranchIterator,
  typename = enable_for_binary_tree_branch_iterator<BinaryTreeBranchIterator>>
void write_graphviz(
  std::ostream& out,
  BinaryTreeBranchIterator x,
  BinaryTreeBranchIterator highlight = BinaryTreeBranchIterator())
{
  write_graphviz(out,
                 x,
                 [](BinaryTreeBranchIterator x)
                 {
                   return *x;
                 },
                 &highlight,
                 &highlight + 1);
}

} // binary_tree

} // dst
