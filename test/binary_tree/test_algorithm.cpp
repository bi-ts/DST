
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/binary_tree/tree.h>
#include <dst/binary_tree/algorithm.h>

#include <boost/test/unit_test.hpp>

namespace
{
// $      |    $
// $      0    $
// $    /   \  $
// $   1     2 $
// $  / \   /  $
// $ 3   4 5   $
const dst::binary_tree::initializer_tree<int>
  init_tree({{3, 1, 4}, 0, {5, 2, {}}});

const dst::binary_tree::tree<int> tree(init_tree);
}

BOOST_AUTO_TEST_SUITE(test_binary_tree_algorithm)

BOOST_AUTO_TEST_CASE(test_leaf)
{
  BOOST_TEST(!leaf(init_tree.root()));
  BOOST_TEST(!leaf(right(init_tree.root())));
  BOOST_TEST(leaf(left(right(init_tree.root()))));
}

BOOST_AUTO_TEST_CASE(test_maximum)
{
  BOOST_TEST(*maximum(init_tree.root()) == 2);
}

BOOST_AUTO_TEST_CASE(test_minimum)
{
  BOOST_TEST(*minimum(init_tree.root()) == 3);
}

BOOST_AUTO_TEST_CASE(test_parent)
{
  BOOST_TEST(*parent(left(tree.root())) == 0);
  BOOST_TEST(*parent(right(tree.root())) == 0);
  BOOST_TEST(!parent(tree.root()));
}

BOOST_AUTO_TEST_CASE(test_roll_down_left)
{
  BOOST_TEST(*roll_down_left(init_tree.root()) == 3);
}

BOOST_AUTO_TEST_CASE(test_roll_down_right)
{
  BOOST_TEST(*roll_down_right(init_tree.root()) == 5);
}

BOOST_AUTO_TEST_CASE(test_root)
{
  for (auto it = tree.begin(); it != tree.end(); ++it)
  {
    BOOST_TEST((root(it.base()) == tree.root()));
  }
}

BOOST_AUTO_TEST_CASE(test_sibling)
{
  BOOST_TEST(*sibling(left(tree.root())) == 2);
  BOOST_TEST(*sibling(right(tree.root())) == 1);
  BOOST_TEST(!sibling(left(right(tree.root()))));
}

BOOST_AUTO_TEST_CASE(test_topologically_equal)
{
  // $      |    $
  // $      0    $
  // $    /   \  $
  // $   1     2 $
  // $  / \   /  $
  // $ 3   4 5   $
  dst::binary_tree::initializer_tree<int> a({{3, 1, 4}, 0, {5, 2, {}}});

  BOOST_TEST(topologically_equal(init_tree.root(), a.root()));

  // $    |      $
  // $    4      $
  // $  /   \    $
  // $ 3     5   $
  // $  \   / \  $
  // $   1 0   2 $
  dst::binary_tree::initializer_tree<int> b({{{}, 3, 1}, 4, {0, 5, 2}});

  BOOST_TEST(!topologically_equal(init_tree.root(), b.root()));

  // $   | $
  // $   4 $
  // $  /  $
  // $ 3   $
  // $  \  $
  // $   1 $
  dst::binary_tree::initializer_tree<int> c({{{}, 3, 1}, 4, {}});

  BOOST_TEST(!topologically_equal(init_tree.root(), c.root()));

  // $ | $
  dst::binary_tree::initializer_tree<int> d;

  BOOST_TEST(!topologically_equal(init_tree.root(), d.root()));

  // $ | $
  dst::binary_tree::initializer_tree<int> e;

  BOOST_TEST(topologically_equal(d.root(), e.root()));
}

BOOST_AUTO_TEST_CASE(test_successor)
{
  auto it = left(left(tree.root()));

  BOOST_TEST(*it == 3);

  it = successor(it);

  BOOST_TEST(*it == 1);

  it = successor(it);

  BOOST_TEST(*it == 4);

  it = successor(it);

  BOOST_TEST(*it == 0);

  it = successor(it);

  BOOST_TEST(*it == 5);

  it = successor(it);

  BOOST_TEST(*it == 2);

  BOOST_TEST(!successor(it));
}

BOOST_AUTO_TEST_CASE(test_predecessor)
{
  auto it = right(tree.root());

  BOOST_TEST(*it == 2);

  it = predecessor(it);

  BOOST_TEST(*it == 5);

  it = predecessor(it);

  BOOST_TEST(*it == 0);

  it = predecessor(it);

  BOOST_TEST(*it == 4);

  it = predecessor(it);

  BOOST_TEST(*it == 1);

  it = predecessor(it);

  BOOST_TEST(*it == 3);

  BOOST_TEST(!predecessor(it));
}

BOOST_AUTO_TEST_CASE(test_order)
{
  const auto it_0 = tree.root();
  const auto it_1 = left(it_0);
  const auto it_2 = right(it_0);
  const auto it_3 = left(it_1);
  const auto it_4 = right(it_1);
  const auto it_5 = left(it_2);

  BOOST_TEST(order(it_4, it_5));
  BOOST_TEST(order(it_5, it_4) == false);

  BOOST_TEST(order(it_3, it_4));
  BOOST_TEST(order(it_4, it_3) == false);

  BOOST_TEST(order(it_3, it_0));
  BOOST_TEST(order(it_0, it_3) == false);

  BOOST_TEST(order(it_5, it_5) == false);
}

BOOST_AUTO_TEST_CASE(test_inorder_depth_first_search)
{
  std::vector<int> sequence(begin_inorder_depth_first_search(tree.root()),
                            end_inorder_depth_first_search(tree.nil()));

  BOOST_TEST(sequence == std::vector<int>({3, 1, 4, 0, 5, 2}),
             boost::test_tools::per_element());
}

BOOST_AUTO_TEST_CASE(test_postorder_depth_first_search)
{
  std::vector<int> sequence(begin_postorder_depth_first_search(tree.root()),
                            end_postorder_depth_first_search(tree.nil()));

  BOOST_TEST(sequence == std::vector<int>({3, 4, 1, 5, 2, 0}),
             boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()

