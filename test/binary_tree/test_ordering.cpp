
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/counter_allocator.h>
#include <dst/binary_tree/tree.h>
#include <dst/binary_tree/mixin/ordering.h>

#include <boost/test/unit_test.hpp>

namespace dst_test
{

using tree = dst::binary_tree::tree<int,
                                    dst::counter_allocator<int>,
                                    dst::binary_tree::Ordering>;

BOOST_AUTO_TEST_SUITE(test_binary_tree_ordering)

BOOST_AUTO_TEST_CASE(test_allocator_used_by_internal_vectors)
{
  // $      |      $
  // $      4      $
  // $    /   \    $
  // $   2     6   $
  // $  / \   / \  $
  // $ 1   3 5   7 $
  tree t({{1, 2, 3}, 4, {5, 6, 7}});

  const auto allocated_memory_1 = t.get_allocator().allocated();

  BOOST_TEST(allocated_memory_1 > 0);

  BOOST_TEST(t.order(t.begin(), --t.end()));

  const auto allocated_memory_2 = t.get_allocator().allocated();

  BOOST_TEST(allocated_memory_2 > allocated_memory_1);

  BOOST_TEST(!t.order(right(t.root()), left(t.root())));

  BOOST_TEST(t.get_allocator().allocated() == allocated_memory_2);
}

BOOST_AUTO_TEST_SUITE_END()
}
