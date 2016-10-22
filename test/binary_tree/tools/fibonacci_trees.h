
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/binary_tree/initializer_tree.h>

#include <vector>

namespace dst
{

std::vector<binary_tree::initializer_tree<int>>
generate_fibonacci_trees(std::size_t h);

} // dst
