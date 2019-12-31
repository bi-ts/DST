
//          Copyright Maksym V. Bilinets 2015 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/iterator_facade.h>

#include <cassert>  // assert
#include <iterator> // std::iterator_traits, std::forward_iterator_tag

namespace dst
{

struct binary_tree_branch_iterator_tag
{
};

struct binary_tree_iterator_tag : public binary_tree_branch_iterator_tag,
                                  public std::forward_iterator_tag
{
};

template <typename Iterator, typename T = void>
using enable_for_binary_tree_branch_iterator = typename std::enable_if<
  std::is_convertible<typename Iterator::iterator_category,
                      binary_tree_branch_iterator_tag>::value>;

template <typename Iterator, typename T = void>
using enable_for_binary_tree_iterator =
  enable_for_iterator_category<Iterator, binary_tree_iterator_tag>;

template <typename Derived, typename T>
class iterator_facade<Derived, binary_tree_branch_iterator_tag, T>
: public std::iterator<binary_tree_branch_iterator_tag, T>
{
public:
  bool operator!() const
  {
    return derived_().nil();
  }

  typename iterator_facade::reference operator*() const
  {
    assert(!derived_().nil());

    return derived_().value();
  }

  typename iterator_facade::pointer operator->() const
  {
    assert(!derived_().nil());

    return std::addressof(derived_().value());
  }

  friend Derived left(Derived position)
  {
    assert(!!position);

    static_cast<iterator_facade&>(position).left_();

    return position;
  }

  friend Derived right(Derived position)
  {
    assert(!!position);

    static_cast<iterator_facade&>(position).right_();

    return position;
  }

  friend bool operator!=(const Derived& lhs, const Derived& rhs)
  {
    return !(lhs == rhs);
  }

private:
  Derived& derived_()
  {
    return static_cast<Derived&>(*this);
  }

  const Derived& derived_() const
  {
    return static_cast<const Derived&>(*this);
  }

  void left_()
  {
    derived_().move_left();
  }

  void right_()
  {
    derived_().move_right();
  }
};

template <typename Derived, typename T>
class iterator_facade<Derived, binary_tree_iterator_tag, T>
: public std::iterator<binary_tree_iterator_tag, T>
{
public:
  bool operator!() const
  {
    return derived_().nil();
  }

  typename iterator_facade::reference operator*() const
  {
    assert(!derived_().nil());

    return derived_().value();
  }

  typename iterator_facade::pointer operator->() const
  {
    assert(!derived_().nil());

    return std::addressof(derived_().value());
  }

  Derived& operator++()
  {
    assert(!derived_().nil());

    derived_().move_to_parent();

    return derived_();
  }

  Derived operator++(int)
  {
    assert(!derived_().nil());

    const auto prev = derived_();

    derived_().move_to_parent();

    return prev;
  }

  friend Derived left(Derived position)
  {
    assert(!!position);

    static_cast<iterator_facade&>(position).left_();

    return position;
  }

  friend Derived right(Derived position)
  {
    assert(!!position);

    static_cast<iterator_facade&>(position).right_();

    return position;
  }

  friend bool operator!=(const Derived& lhs, const Derived& rhs)
  {
    return !(lhs == rhs);
  }

private:
  Derived& derived_()
  {
    return static_cast<Derived&>(*this);
  }

  const Derived& derived_() const
  {
    return static_cast<const Derived&>(*this);
  }

  void left_()
  {
    derived_().move_left();
  }

  void right_()
  {
    derived_().move_right();
  }

  void root_()
  {
    derived_().move_to_root();
  }
};

} // dst
