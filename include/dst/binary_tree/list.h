
//          Copyright Maksym V. Bilinets 2015 - 2019.
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include "mixin.h"
#include "mixin/avl.h"
#include "mixin/binary.h"

namespace dst
{

namespace binary_tree
{

template <typename T,
          typename Allocator = std::allocator<T>,
          typename Mixin = AVL,
          typename... Mixins>
class list : public fold_mixins<Binary, Mixin, Mixins...>::
               template type<T, void, Allocator>
{
private:
  using base = typename fold_mixins<Binary, Mixin, Mixins...>::
    template type<T, void, Allocator>;

  static_assert(is_balanced_binary_tree<typename base::tree_category>::value,
                "Must be balanced");

public:
  using typename base::const_tree_iterator;
  using typename base::tree_iterator;

  using typename base::const_iterator;
  using typename base::const_pointer;
  using typename base::const_reference;
  using typename base::difference_type;
  using typename base::iterator;
  using typename base::pointer;
  using typename base::reference;
  using typename base::size_type;
  using typename base::value_type;

  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  using allocator_type = typename base::allocator_type;

public:
  using base::begin;
  using base::clear;
  using base::empty;
  using base::end;
  using base::get_allocator;
  using base::max_size;
  using base::nil;
  using base::root;
  using base::size;

public:
  list()
  : base()
  {
  }

  explicit list(const allocator_type& allocator)
  : base(allocator)
  {
  }

  explicit list(const list& other, const allocator_type& allocator)
  : base(other, allocator)
  {
  }

  list(list&& other, const allocator_type& allocator)
  : base(std::move(other), allocator)
  {
  }

  list(size_type n,
       const_reference v,
       allocator_type allocator = allocator_type())
  : base(allocator)
  {
    for (size_type i = 0; i < n; ++i)
    {
      push_back(std::move(v));
    }
  }

  list(size_type n)
  : base()
  {
    for (size_type i = 0; i < n; ++i)
    {
      emplace_back();
    }
  }

  template <typename InputIterator,
            typename = enable_for_input_iterator<InputIterator>>
  list(InputIterator from,
       InputIterator to,
       allocator_type allocator = allocator_type())
  : list(allocator)
  {
    insert(cend(), from, to);
  }

  list(const std::initializer_list<value_type>& init,
       const allocator_type& allocator = allocator_type())
  : list(allocator)
  {
    std::copy(std::begin(init), std::end(init), std::back_inserter(*this));
  }

  list& operator=(const std::initializer_list<value_type>& init)
  {
    *this = list(init, base::get_allocator());

    return *this;
  }

  template <typename... Args>
  iterator emplace(const_iterator position, Args&&... args)
  {
    if (!position.base())
      return iterator(base::emplace_right(maximum(base::root()),
                                          std::forward<Args>(args)...));

    if (!left(position.base()))
      return iterator(
        base::emplace_left(position.base(), std::forward<Args>(args)...));

    return iterator(base::emplace_right(maximum(left(position.base())),
                                        std::forward<Args>(args)...));
  }

  iterator insert(const_iterator position, const_reference v)
  {
    return emplace(position, v);
  }

  iterator insert(const_iterator position, value_type&& v)
  {
    return emplace(position, std::move(v));
  }

  iterator insert(const_iterator position,
                  const std::initializer_list<value_type>& init)
  {
    return insert(position, std::begin(init), std::end(init));
  }

  iterator insert(const_iterator position, size_type n, const_reference v)
  {
    auto pos = iterator(base::iterator_const_cast(position.base()));

    try
    {
      for (size_type i = 0; i < n; ++i)
      {
        pos = emplace(pos, v);
      }
    }
    catch (...)
    {
      erase(pos, position);
      throw;
    }

    return pos;
  }

  template <typename InputIterator,
            typename = enable_for_input_iterator<InputIterator>>
  iterator insert(const_iterator position, InputIterator from, InputIterator to)
  {
    auto pos = iterator(base::iterator_const_cast(position.base()));

    try
    {
      if (from != to)
      {
        pos = emplace(position, *(from++));
      }

      for (; from != to; ++from)
      {
        emplace(position, *from);
      }
    }
    catch (...)
    {
      erase(pos, position);
      throw;
    }

    return pos;
  }

  template <typename... Args>
  iterator emplace_after(const_iterator position, Args&&... args)
  {
    assert(!!position.base());

    if (!right(position.base()))
      return iterator(
        base::emplace_right(position.base(), std::forward<Args>(args)...));

    return iterator(base::emplace_left(minimum(right(position.base())),
                                       std::forward<Args>(args)...));
  }

  iterator insert_after(const_iterator position, const_reference v)
  {
    return emplace_after(position, v);
  }

  iterator insert_after(const_iterator position, value_type&& v)
  {
    return emplace_after(position, std::move(v));
  }

  iterator insert_after(const_iterator position,
                        const std::initializer_list<value_type>& init)
  {
    return insert_after(position, std::begin(init), std::end(init));
  }

  iterator insert_after(const_iterator position, size_type n, const_reference v)
  {
    auto pos = iterator(base::iterator_const_cast(position.base()));

    for (size_type i = 0; i < n; ++i)
    {
      pos = insert_after(pos, v);
    }

    return pos;
  }

  template <typename InputIterator,
            typename = enable_for_input_iterator<InputIterator>>
  iterator
  insert_after(const_iterator position, InputIterator from, InputIterator to)
  {
    auto pos = iterator(base::iterator_const_cast(position.base()));

    for (; from != to; ++from)
    {
      pos = insert_after(pos, *from);
    }

    return pos;
  }

  iterator erase(const_iterator position)
  {
    assert(position != cend());

    const auto x = base::iterator_const_cast(position.base());
    const auto y = successor(x);

    if (!!left(x) && !!right(x))
      base::erase(x, y);
    else
      base::erase(x);

    return iterator(y);
  }

  iterator erase(const_iterator from, const_iterator to)
  {
    while (from != to)
    {
      erase(from++);
    }

    return iterator(base::iterator_const_cast(to.base()));
  }

  template <typename InputIterator,
            typename = enable_for_input_iterator<InputIterator>>
  void assign(InputIterator from, InputIterator to)
  {
    *this = list(from, to, base::get_allocator());
  }

  void assign(const std::initializer_list<value_type>& init)
  {
    *this = list(init, base::get_allocator());
  }

  void assign(size_type n, const_reference v)
  {
    *this = list(n, v, base::get_allocator());
  }

  template <typename... Args> reference emplace_back(Args&&... args)
  {
    return *emplace(cend(), std::forward<Args>(args)...);
  }

  void push_back(const_reference v)
  {
    insert(cend(), v);
  }

  void push_back(value_type&& v)
  {
    insert(cend(), std::move(v));
  }

  void pop_back()
  {
    erase(--cend());
  }

  template <typename... Args> reference emplace_front(Args&&... args)
  {
    return *emplace(cbegin(), std::forward<Args>(args)...);
  }

  void push_front(const_reference v)
  {
    insert(cbegin(), v);
  }

  void push_front(value_type&& v)
  {
    insert(cbegin(), std::forward<value_type>(v));
  }

  void pop_front()
  {
    erase(cbegin());
  }

  const_tree_iterator croot() const
  {
    return root();
  }

  const_tree_iterator cnil() const
  {
    return nil();
  }

  const_iterator cbegin() const
  {
    return begin();
  }

  const_iterator cend() const
  {
    return end();
  }

  reference front()
  {
    return *begin();
  }

  const_reference front() const
  {
    return *begin();
  }

  reference back()
  {
    return *(--end());
  }

  const_reference back() const
  {
    return *(--end());
  }

  bool operator==(const list& other) const
  {
    return this->size() == other.size() &&
           std::equal(cbegin(), cend(), other.begin());
  }

  bool operator!=(const list& other) const
  {
    return !(*this == other);
  }

  void swap(list& other)
  {
    base::swap(other);
  }

  friend void swap(list& lhs, list& rhs)
  {
    lhs.swap(rhs);
  }

  template <typename Predicate> void remove_if(Predicate p)
  {
    auto position = cbegin();
    auto last = cend();
    while (position != last)
    {
      const auto prev = position++;
      if (p(*prev))
        erase(prev);
    }
  }

  void remove(const_reference value)
  {
    remove_if([value](const_reference v) { return v == value; });
  }

  void resize(size_type count, value_type value = value_type())
  {
    if (size() < count)
    {
      insert(end(), count - size(), value);
    }
    else if (size() > count)
    {
      auto it = begin();
      std::advance(it, count);
      erase(it, end());
    }
  }

  void reverse()
  {
    std::reverse(begin(), end());
  }
};

} // binary_tree

} // dst
