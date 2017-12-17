
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include "tools/trees_generator.h"

#include <dst/binary_tree/algorithm.h>
#include <dst/binary_tree/initializer_tree.h>
#include <dst/binary_tree/write_graphviz.h>

#include <boost/test/unit_test.hpp>

#include <iostream>

BOOST_AUTO_TEST_SUITE(test_binary_tree_initializer_tree)

BOOST_AUTO_TEST_CASE(test_initializer_tree_construction)
{
  // $     |     $
  // $     4     $
  // $    / \    $
  // $   2   5   $
  // $  / \   \  $
  // $ 1   3   6 $
  const dst::binary_tree::initializer_tree<int> tree(
    {{1, 2, 3}, 4, {{}, 5, 6}});

  BOOST_TEST(tree.size() == 6);
  BOOST_TEST(tree.empty() == false);

  BOOST_TEST(*tree.root() == 4);
  BOOST_TEST(*left(tree.root()) == 2);
  BOOST_TEST(*right(tree.root()) == 5);
  BOOST_TEST(*left(left(tree.root())) == 1);
  BOOST_TEST(*right(left(tree.root())) == 3);
  BOOST_TEST((left(right(tree.root())) == tree.nil()));
  BOOST_TEST(*right(right(tree.root())) == 6);

  BOOST_TEST((left(left(left(tree.root()))) == tree.nil()));
  BOOST_TEST((right(left(left(tree.root()))) == tree.nil()));
}

BOOST_AUTO_TEST_CASE(test_generate_fibonacci_tree)
{
  // $            5       $
  // $          /   \     $
  // $        4       3   $
  // $      /   \    / \  $
  // $     3     2  2   1 $
  // $    / \   /  /      $
  // $   2   1 1  1       $
  // $  /                 $
  // $ 1                  $
  const dst::binary_tree::initializer_tree<int> fibonacci_tree(
    {{{{1, 2, {}}, 3, 1}, 4, {1, 2, {}}}, 5, {{1, 2, {}}, 3, 1}});

  const auto generated_tree = dst_test::generate_fibonacci_tree(5);

  const auto is_fibonacci_tree =
    topologically_equal(generated_tree.root(), fibonacci_tree.root());

  if (!is_fibonacci_tree)
  {
    std::cout << "Not a Fibonacci tree:" << std::endl;
    dst::binary_tree::write_graphviz(std::cout, generated_tree.root());
  }

  BOOST_TEST(is_fibonacci_tree);
}

BOOST_AUTO_TEST_SUITE_END()
