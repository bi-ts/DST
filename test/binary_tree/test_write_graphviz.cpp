
//          Copyright Maksym V. Bilinets 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/binary_tree/initializer_tree.h>
#include <dst/binary_tree/write_graphviz.h>

#include <boost/test/unit_test.hpp>

#include <sstream>

BOOST_AUTO_TEST_SUITE(test_binary_tree_write_graphviz)

BOOST_AUTO_TEST_CASE(test_write_graphviz)
{
  // $    |    $
  // $    3    $
  // $  /   \  $
  // $ 1     5 $
  // $  \   /  $
  // $   2 4   $
  const dst::binary_tree::initializer_tree<int> tree(
    {{{}, 1, 2}, 3, {4, 5, {}}});

  std::stringstream expected;

  expected << "digraph G {" << std::endl
           << "\t0 [label=\"3\"]" << std::endl
           << "\t0:w->1" << std::endl
           << "\t0:e->2" << std::endl
           << "\t1 [label=\"1\"]" << std::endl
           << "\t1:e->3" << std::endl
           << "\t2 [label=\"5\"]" << std::endl
           << "\t2:w->4" << std::endl
           << "\t3 [label=\"2\"]" << std::endl
           << "\t4 [label=\"4\"]" << std::endl
           << "}" << std::endl;

  std::stringstream actual;
  dst::binary_tree::write_graphviz(actual, tree.root());

  BOOST_CHECK_EQUAL(expected.str(), actual.str());
}

BOOST_AUTO_TEST_CASE(test_write_graphviz_highlight_single_node)
{
  // $    |      $
  // $    3      $
  // $  /   \    $
  // $ 1    (5)  $
  // $  \   / \  $
  // $   2 4   6 $
  const dst::binary_tree::initializer_tree<int> tree(
    {{{}, 1, 2}, 3, {4, 5, 6}});

  std::stringstream expected;

  expected << "digraph G {" << std::endl
           << "\t0 [label=\"3\"]" << std::endl
           << "\t0:w->1" << std::endl
           << "\t0:e->2" << std::endl
           << "\t1 [label=\"1\"]" << std::endl
           << "\t1:e->3" << std::endl
           << "\t2 [label=\"5\", color=red]" << std::endl
           << "\t2:w->4" << std::endl
           << "\t2:e->5" << std::endl
           << "\t3 [label=\"2\"]" << std::endl
           << "\t4 [label=\"4\"]" << std::endl
           << "\t5 [label=\"6\"]" << std::endl
           << "}" << std::endl;

  std::stringstream actual;
  dst::binary_tree::write_graphviz(actual, tree.root(), right(tree.root()));

  BOOST_CHECK_EQUAL(expected.str(), actual.str());
}
BOOST_AUTO_TEST_SUITE_END()

