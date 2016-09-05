
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <cassert>  // assert
#include <iterator> // std::iterator_traits, std::forward_iterator_tag

namespace dst
{

template <typename Iterator, typename IteratorCategory, typename T = void>
using enable_for_iterator_category =
  typename std::enable_if<std::is_convertible<typename std::iterator_traits<
                                                Iterator>::iterator_category,
                                              IteratorCategory>::value,
                          T>::type;

template <typename Iterator, typename T = void>
using enable_for_input_iterator =
  enable_for_iterator_category<Iterator, std::input_iterator_tag>;

template <typename Derived, typename IteratorCategory, typename T>
class iterator_facade;

template <typename Derived, typename T>
class iterator_facade<Derived, std::output_iterator_tag, T>
  : public std::iterator<std::output_iterator_tag, void, void, void, void>
{
public:
  iterator_facade& operator*()
  {
    return derived_();
  }

  Derived& operator++()
  {
    return derived_();
  }

  Derived& operator++(int)
  {
    return derived_();
  }

  friend bool operator!=(const Derived& lhs, const Derived& rhs)
  {
    return !(lhs == rhs);
  }

  Derived& operator=(const T& v)
  {
    derived_().output(v);

    return derived_();
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
};

template <typename Derived, typename T>
class iterator_facade<Derived, std::forward_iterator_tag, T>
  : public std::iterator<std::forward_iterator_tag, T>
{
public:
  typename iterator_facade::reference operator*() const
  {
    return derived_().value();
  }

  typename iterator_facade::pointer operator->() const
  {
    return std::addressof(derived_().value());
  }

  Derived& operator++()
  {
    derived_().move_forward();

    return derived_();
  }

  Derived operator++(int)
  {
    const auto prev = derived_();

    derived_().move_forward();

    return prev;
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
};

template <typename Derived, typename T>
class iterator_facade<Derived, std::bidirectional_iterator_tag, T>
  : public std::iterator<std::bidirectional_iterator_tag, T>
{
public:
  typename iterator_facade::reference operator*() const
  {
    return derived_().value();
  }

  typename iterator_facade::pointer operator->() const
  {
    return std::addressof(derived_().value());
  }

  Derived& operator++()
  {
    derived_().move_forward();

    return derived_();
  }

  Derived operator++(int)
  {
    const auto prev = derived_();

    derived_().move_forward();

    return prev;
  }

  Derived& operator--()
  {
    derived_().move_back();

    return derived_();
  }

  Derived operator--(int)
  {
    const auto prev = derived_();

    derived_().move_back();

    return prev;
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
};

} // dst

