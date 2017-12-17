
//          Copyright Maksym V. Bilinets 2015 - 2017.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include "tools/indexing_tree_invariant.h"
#include "tools/trees_generator.h"

#include <dst/allocator/global_counter_allocator.h>
#include <dst/binary_tree/tree.h>
#include <dst/binary_tree/mixin/indexing.h>

#include <boost/test/unit_test.hpp>

#include <cstddef> // std::size_t
#include <iostream>
#include <iterator> // std::advance
#include <numeric>  // std::iota

namespace dst_test
{

using indexed_tree =
  dst::binary_tree::tree<int, std::allocator<int>, dst::binary_tree::Indexing>;

BOOST_AUTO_TEST_SUITE(test_binary_tree_indexing)

BOOST_AUTO_TEST_CASE(test_indexed_tree_rotations)
{
  const auto tree_height = 10;

  indexed_tree t = generate_fibonacci_tree(tree_height);

  BOOST_TEST((std::distance(t.cbegin().base(), t.croot()) + 1) == tree_height);

  std::iota(t.begin(), t.end(), 0);

  BOOST_TEST(indexing_invariant_holds(t));

  auto it = t.croot();
  while (!!it)
  {
    it = t.rotate_right(it);

    while (!!it && !left(it))
    {
      it = right(it);
    }

    BOOST_TEST(indexing_invariant_holds(t));

    for (indexed_tree::size_type idx = 0; idx < t.size(); ++idx)
    {
      auto it = t.element_at(idx);
      BOOST_CHECK_EQUAL(*it, idx);
      BOOST_CHECK_EQUAL(t.index(it), idx);
      BOOST_CHECK_EQUAL(t.at(idx), idx);
      BOOST_CHECK_EQUAL(t[idx], idx);
    }
  }

  for (auto it = t.croot(); !!it && !!right(it); it = t.rotate_left(it))
  {
    BOOST_TEST(indexing_invariant_holds(t));

    for (indexed_tree::size_type idx = 0; idx < t.size(); ++idx)
    {
      auto it = t.element_at(idx);
      BOOST_CHECK_EQUAL(*it, idx);
      BOOST_CHECK_EQUAL(t.index(it), idx);
      BOOST_CHECK_EQUAL(t.at(idx), idx);
      BOOST_CHECK_EQUAL(t[idx], idx);
    }
  }
}

BOOST_AUTO_TEST_SUITE_END()
}
