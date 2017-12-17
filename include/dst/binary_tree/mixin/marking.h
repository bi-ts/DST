
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <dst/binary_tree/initializer_tree.h>
#include <dst/binary_tree/mixin.h>
#include <dst/utility.h>

namespace dst
{

namespace binary_tree
{

namespace mixin
{

namespace detail
{

class marking_base_tag
{
};

template <typename T,
          typename M,
          typename Allocator,
          template <typename, typename, typename> class Base,
          bool>
class marking_base : public Base<T, M, Allocator>, public marking_base_tag
{
private:
  using base = Base<T, M, Allocator>;

  struct undefined;

protected:
  using typename base::const_tree_iterator;

  using allocator_type = typename base::allocator_type;

public:
  void mark(undefined&);

  void unmark(undefined&);

  void marked(undefined&);

  void marked_nodes(undefined&);

  void begin_marked(undefined&);

  void end_marked(undefined&);

  void rbegin_marked(undefined&);

  void rend_marked(undefined&);

protected:
  marking_base()
  : base()
  {
  }

  explicit marking_base(const allocator_type& allocator)
  : base(allocator)
  {
  }

  explicit marking_base(const marking_base& other,
                        const allocator_type& allocator)
  : base(other, allocator)
  {
  }

  marking_base(marking_base&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  {
  }

  marking_base(const initializer_tree<T>& init, const allocator_type& allocator)
  : base(init, allocator)
  {
  }
};

template <typename T,
          typename M,
          typename Allocator,
          template <typename, typename, typename> class Base>
class marking_base<T, M, Allocator, Base, true> : public Base<T, M, Allocator>
{
private:
  using base = Base<T, M, Allocator>;

protected:
  using allocator_type = typename base::allocator_type;

public:
  using base::mark;
  using base::unmark;
  using base::marked;
  using base::marked_nodes;
  using base::begin_marked;
  using base::end_marked;
  using base::rbegin_marked;
  using base::rend_marked;

protected:
  explicit marking_base(const allocator_type& allocator)
  : base(allocator)
  {
  }

  explicit marking_base(const marking_base& other,
                        const allocator_type& allocator)
  : base(other, allocator)
  {
  }

  marking_base(marking_base&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  {
  }

  marking_base(const initializer_tree<T>& init, const allocator_type& allocator)
  : base(init, allocator)
  {
  }
};
}

template <typename Flag,
          typename T,
          typename M,
          typename Allocator,
          template <typename, typename, typename> class Base>
class marking
  : public detail::marking_base<
      T,
      pair_or_single<std::size_t, M>,
      Allocator,
      Base,
      std::is_base_of<detail::marking_base_tag, Base<T, M, Allocator>>::value>
{
private:
  using base = detail::marking_base<
    T,
    pair_or_single<std::size_t, M>,
    Allocator,
    Base,
    std::is_base_of<detail::marking_base_tag, Base<T, M, Allocator>>::value>;

  template <typename BinaryTreeIterator>
  class marked_iterator_base
    : public iterator_facade<
        marked_iterator_base<BinaryTreeIterator>,
        std::bidirectional_iterator_tag,
        typename std::iterator_traits<BinaryTreeIterator>::value_type>
  {
  private:
    friend iterator_facade<
      marked_iterator_base<BinaryTreeIterator>,
      std::bidirectional_iterator_tag,
      typename std::iterator_traits<BinaryTreeIterator>::value_type>;

  public:
    explicit marked_iterator_base(
      BinaryTreeIterator position = BinaryTreeIterator())
    : position_(position)
    {
    }

    template <
      typename OtherIterator,
      typename = typename std::enable_if<
        std::is_convertible<OtherIterator, BinaryTreeIterator>::value>::type>
    marked_iterator_base(const marked_iterator_base<OtherIterator>& other)
    : marked_iterator_base(static_cast<BinaryTreeIterator>(other.base()))
    {
    }

    BinaryTreeIterator base() const
    {
      return position_;
    }

    friend bool operator==(const marked_iterator_base& lhs,
                           const marked_iterator_base& rhs)
    {
      return lhs.position_ == rhs.position_;
    }

  private:
    typename marked_iterator_base::reference value() const
    {
      return *position_;
    }

    void move_forward()
    {
      assert(!!position_);

      auto position = position_;

      if (rank(right(position)) > 0)
      {
        position_ = marked_minimum(right(position));
      }
      else
      {
        auto p = parent(position);

        while (!!p && (position == right(p) || rank(position) == rank(p)))
        {
          position = p;
          p = parent(p);
        }

        assert(!p || rank(p) > rank(position));

        if (!p || marked(p, Flag()))
          position_ = p;
        else
          position_ = marked_minimum(right(p));
      }
    }

    void move_back()
    {
      if (!position_)
        position_ = marked_maximum(base::root_from_nil(position_));
      else
      {
        assert(!!position_);

        auto position = position_;

        if (rank(left(position)) > 0)
        {
          position_ = marked_maximum(left(position));
        }
        else
        {
          auto p = parent(position);

          while (!!p && (position == left(p) || rank(position) == rank(p)))
          {
            position = p;
            p = parent(p);
          }

          assert(!p || rank(p) > rank(position));

          if (!p || marked(p, Flag()))
            position_ = p;
          else
            position_ = marked_maximum(left(p));
        }
      }
    }

  private:
    BinaryTreeIterator position_;
  };

protected:
  using tree_category = unbalanced_binary_tree_tag;

  using typename base::tree_iterator;
  using typename base::const_tree_iterator;
  using typename base::iterator;
  using typename base::const_iterator;

  using allocator_type = typename base::allocator_type;

  using base::root;
  using base::nil;

  using marked_iterator = marked_iterator_base<tree_iterator>;
  using const_marked_iterator = marked_iterator_base<const_tree_iterator>;

  using reverse_marked_iterator = std::reverse_iterator<marked_iterator>;
  using const_reverse_marked_iterator =
    std::reverse_iterator<const_marked_iterator>;

public:
  using base::mark;
  using base::unmark;
  using base::marked;
  using base::marked_nodes;
  using base::begin_marked;
  using base::end_marked;
  using base::rbegin_marked;
  using base::rend_marked;

public:
  bool mark(const_tree_iterator x, Flag f = Flag())
  {
    if (marked(x, f))
      return false;

    for (; !!x; ++x)
    {
      ++rank(x);
    }

    return true;
  }

  bool mark(const_iterator x, Flag f = Flag())
  {
    return mark(x.base(), f);
  }

  bool unmark(const_tree_iterator x, Flag f = Flag())
  {
    if (!marked(x, f))
      return false;

    for (; !!x; ++x)
    {
      --rank(x);
    }

    return true;
  }

  bool unmark(const_iterator x, Flag f = Flag())
  {
    return unmark(x.base(), f);
  }

  static bool marked(const_tree_iterator x, Flag = Flag())
  {
    assert(rank(left(x)) + rank(right(x)) <= rank(x));
    assert(rank(x) - (rank(left(x)) + rank(right(x))) <= 1);

    return rank(left(x)) + rank(right(x)) < rank(x);
  }

  static bool marked(const_iterator x, Flag f = Flag())
  {
    return marked(x.base(), f);
  }

  static std::size_t marked_nodes(const_tree_iterator x, Flag = Flag())
  {
    return base::metadata(x).first();
  }

  marked_iterator begin_marked(Flag f = Flag())
  {
    if (rank(base::root()) == 0)
      return end_marked(f);

    return marked_iterator(marked_minimum(base::root()));
  }

  const_marked_iterator begin_marked(Flag f = Flag()) const
  {
    if (rank(base::root()) == 0)
      return end_marked(f);

    return const_marked_iterator(marked_minimum(base::root()));
  }

  marked_iterator end_marked(Flag f = Flag())
  {
    return marked_iterator(base::end().base());
  }

  const_marked_iterator end_marked(Flag f = Flag()) const
  {
    return const_marked_iterator(base::end().base());
  }

  reverse_marked_iterator rbegin_marked(Flag f = Flag())
  {
    return reverse_marked_iterator(end_marked(f));
  }

  const_reverse_marked_iterator rbegin_marked(Flag f = Flag()) const
  {
    return const_reverse_marked_iterator(end_marked(f));
  }

  reverse_marked_iterator rend_marked(Flag f = Flag())
  {
    return reverse_marked_iterator(begin_marked(f));
  }

  const_reverse_marked_iterator rend_marked(Flag f = Flag()) const
  {
    return const_reverse_marked_iterator(begin_marked(f));
  }

  template <typename BinaryTreeIterator>
  static BinaryTreeIterator marked_minimum(BinaryTreeIterator x)
  {
    assert(!!x && rank(x) > 0);

    while (!!x && rank(x) > 0)
    {
      if (rank(left(x)) > 0)
        x = left(x);
      else if (rank(right(x)) == rank(x))
        x = right(x);
      else
        break;
    }

    assert(!!x && marked(x, Flag()));

    return x;
  }

  template <typename BinaryTreeIterator>
  static BinaryTreeIterator marked_maximum(BinaryTreeIterator x)
  {
    assert(!!x && rank(x) > 0);

    while (!!x && rank(x) > 0)
    {
      if (rank(right(x)) > 0)
        x = right(x);
      else if (rank(left(x)) > 0)
        x = left(x);
      else
        break;
    }

    assert(!!x && rank(x) == 1);

    return x;
  }

protected:
  marking()
  : base()
  {
  }

  explicit marking(const allocator_type& allocator)
  : base(allocator)
  {
  }

  explicit marking(const marking& other, const allocator_type& allocator)
  : base(other, allocator)
  {
  }

  marking(marking&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  {
  }

  marking(const initializer_tree<T>& init, const allocator_type& allocator)
  : base(init, allocator)
  {
  }

  void erase(const_tree_iterator position, const_tree_iterator sub)
  {
    unmark(position, Flag());

    if (marked(sub, Flag()))
    {
      unmark(sub, Flag());

      base::erase(position, sub);

      mark(sub, Flag());
    }
    else
    {
      base::erase(position, sub);
    }
  }

  void erase(const_tree_iterator position)
  {
    unmark(position, Flag());

    base::erase(position);
  }

  // $   |            |   $
  // $   x            y'  $
  // $  / \          / \  $
  // $ a   y   =>   x'  c $
  // $    / \      / \    $
  // $   b   c    a   b   $
  tree_iterator rotate_left(const_tree_iterator x)
  {
    bool x_marked = marked(x, Flag());
    bool y_marked = marked(right(x), Flag());

    tree_iterator y = base::rotate_left(x);

    rank(x) = rank(left(x)) + rank(right(x)) + x_marked;
    rank(y) = rank(left(y)) + rank(right(y)) + y_marked;

    return y;
  }

  // $     |        |     $
  // $     x        y'    $
  // $    / \      / \    $
  // $   y   c => a   x'  $
  // $  / \          / \  $
  // $ a   b        b   c $
  tree_iterator rotate_right(const_tree_iterator x)
  {
    const bool x_marked = marked(x, Flag());
    const bool y_marked = marked(left(x), Flag());

    tree_iterator y = base::rotate_right(x);

    rank(x) = rank(left(x)) + rank(right(x)) + x_marked;
    rank(y) = rank(left(y)) + rank(right(y)) + y_marked;

    return y;
  }

  static typename ref_or_void<M>::type metadata(const_tree_iterator x)
  {
    return base::metadata(x).second();
  }

private:
  static std::size_t& rank(const_tree_iterator x)
  {
    return base::metadata(x).first();
  }
};

} // mixin

enum default_marking_color_t
{
  default_marking_color
};

template <typename Flag = default_marking_color_t> class Marking
{
public:
  template <typename T,
            typename M,
            typename Allocator,
            template <typename, typename, typename> class Base>
  using type = mixin::marking<Flag, T, M, Allocator, Base>;
};

} // binary_tree

} // dst
