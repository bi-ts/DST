
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#include "tools/marking_tree_invariant.h"
#include "tools/trees_generator.h"

#include <dst/allocator/global_counter_allocator.h>
#include <dst/binary_tree/tree.h>
#include <dst/binary_tree/mixin/marking.h>

#include <boost/test/unit_test.hpp>

#include <cstddef> // std::size_t
#include <iostream>
#include <iterator> // std::advance
#include <numeric>  // std::iota

namespace dst_test
{

enum red_t
{
  red
};

enum green_t
{
  green
};

enum blue_t
{
  blue
};

using rgb_tree = dst::binary_tree::tree<int,
                                        std::allocator<int>,
                                        dst::binary_tree::Marking<red_t>,
                                        dst::binary_tree::Marking<green_t>,
                                        dst::binary_tree::Marking<blue_t>>;

BOOST_AUTO_TEST_SUITE(test_binary_tree_marking)

BOOST_AUTO_TEST_CASE(test_rgb_tree_rotations)
{
  rgb_tree t = generate_fibonacci_tree(10);

  BOOST_TEST((std::distance(t.cbegin().base(), t.croot()) + 1) == 10);

  std::iota(t.begin(), t.end(), 0);

  const auto it_end = t.cend();
  for (auto it = t.cbegin(); it != it_end; ++it)
  {
    t.mark(it, red);

    if (*it % 2)
      t.mark(it, green);

    if (*it % 3)
      t.mark(it, blue);
  }

  std::vector<int> red_nodes(t.begin_marked(red), t.end_marked(red));
  std::vector<int> green_nodes(t.begin_marked(green), t.end_marked(green));
  std::vector<int> blue_nodes(t.begin_marked(blue), t.end_marked(blue));

  BOOST_TEST(rgb_tree::marked_nodes(t.croot(), red) == t.size());
  BOOST_TEST(rgb_tree::marked_nodes(t.croot(), green) == t.size() / 2);
  BOOST_TEST(rgb_tree::marked_nodes(t.croot(), blue) == 2 * t.size() / 3);

  BOOST_TEST(marking_invariant_holds(t, red));
  BOOST_TEST(marking_invariant_holds(t, green));
  BOOST_TEST(marking_invariant_holds(t, blue));

  auto it = t.croot();
  while (!!it)
  {
    it = t.rotate_right(it);

    while (!!it && !left(it))
    {
      it = right(it);
    }

    BOOST_TEST(std::vector<int>(t.begin_marked(red), t.end_marked(red)) ==
                 red_nodes,
               boost::test_tools::per_element());

    BOOST_TEST(std::vector<int>(t.begin_marked(green), t.end_marked(green)) ==
                 green_nodes,
               boost::test_tools::per_element());

    BOOST_TEST(std::vector<int>(t.begin_marked(blue), t.end_marked(blue)) ==
                 blue_nodes,
               boost::test_tools::per_element());

    BOOST_TEST(marking_invariant_holds(t, red));
    BOOST_TEST(marking_invariant_holds(t, green));
    BOOST_TEST(marking_invariant_holds(t, blue));
  }

  BOOST_TEST((std::distance(t.cbegin().base(), t.croot()) + 1) == 1);

  for (auto it = t.croot(); !!it && !!right(it); it = t.rotate_left(it))
  {

    BOOST_TEST(std::vector<int>(t.begin_marked(red), t.end_marked(red)) ==
                 red_nodes,
               boost::test_tools::per_element());

    BOOST_TEST(std::vector<int>(t.begin_marked(green), t.end_marked(green)) ==
                 green_nodes,
               boost::test_tools::per_element());

    BOOST_TEST(std::vector<int>(t.begin_marked(blue), t.end_marked(blue)) ==
                 blue_nodes,
               boost::test_tools::per_element());

    BOOST_TEST(marking_invariant_holds(t, red));
    BOOST_TEST(marking_invariant_holds(t, green));
    BOOST_TEST(marking_invariant_holds(t, blue));
  }

  BOOST_TEST((std::distance(t.cbegin().base(), t.croot()) + 1) == t.size());
}

BOOST_AUTO_TEST_SUITE_END()
}
