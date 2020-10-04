
//          Copyright Maksym V. Bilinets 2015 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include <dst/allocator/counter_allocator.h>
#include <dst/allocator/wary_allocator.h>
#include <dst/binary_tree/balanced_tree.h>
#include <dst/binary_tree/mixin/ordering.h>

#include <boost/test/unit_test.hpp>

namespace dst_test
{

#ifdef _LIBCPP_VERSION // We use libc++
template <typename T>
using test_allocator = dst::counter_allocator<T, dst::wary_allocator<T>>;
#else
template <typename T> using test_allocator = dst::counter_allocator<T>;
#endif

using ordering_tree =
  dst::binary_tree::balanced_tree<int,
                                  test_allocator<int>,
                                  dst::binary_tree::AVL,
                                  dst::binary_tree::Ordering>;

BOOST_AUTO_TEST_SUITE(test_binary_tree_ordering)

BOOST_AUTO_TEST_CASE(test_allocator_used_by_internal_vectors)
{
  // $      |      $
  // $      3      $
  // $    /   \    $
  // $   2     4   $
  // $  /       \  $
  // $ 1         5 $
  ordering_tree tree;

  const auto it_3 = tree.insert_left(tree.nil(), 3);
  const auto it_2 = tree.insert_left(it_3, 2);
  const auto it_4 = tree.insert_right(it_3, 4);
  const auto it_1 = tree.insert_left(it_2, 1);
  const auto it_5 = tree.insert_right(it_4, 5);

  const auto allocated_memory_1 = tree.get_allocator().allocated();

  BOOST_TEST(allocated_memory_1 > 0u);

  BOOST_TEST(tree.order(it_2, it_4));

  const auto allocated_memory_2 = tree.get_allocator().allocated();

  BOOST_TEST(allocated_memory_2 > allocated_memory_1);

  BOOST_TEST(!tree.order(it_5, it_1));

  const auto allocated_memory_3 = tree.get_allocator().allocated();

  BOOST_TEST(allocated_memory_3 > allocated_memory_2);

  BOOST_TEST(tree.order(it_2, it_5));

  BOOST_TEST(tree.get_allocator().allocated() == allocated_memory_3);
}

BOOST_AUTO_TEST_SUITE_END()
}
