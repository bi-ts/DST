
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

namespace dst
{

template <typename T> class ref_or_void 
{
public:
  using type = T&;
};

template <> class ref_or_void<void>
{
public:
  using type = void;
};

template <typename F, typename S> class pair_or_single
{
public:
  explicit pair_or_single(const F& f = F(), const S& s = S())
  : first_(f)
  , second_(s)
  {
  }

  F& first()
  {
    return first_;
  }

  const F& first() const
  {
    return first_;
  }

  S& second()
  {
    return second_;
  }

  const S& second() const
  {
    return second_;
  }

private:
  F first_;
  S second_;
};

template <typename F> class pair_or_single<F, void>
{
public:
  explicit pair_or_single(const F& f = F())
  : first_(f)
  {
  }

  F& first()
  {
    return first_;
  }

  const F& first() const
  {
    return first_;
  }

  void second() const
  {
  }

private:
  F first_;
};

} // dst

