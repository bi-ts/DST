
//          Copyright Maksym V. Bilinets 2015 - 2016.
// Distributed under the Boost Software License, Version 1.0.
//        (See accompanying file LICENSE or copy at
//          http://www.boost.org/LICENSE_1_0.txt )

#pragma once

#include <type_traits> // std::is_convertible

namespace dst
{

namespace binary_tree
{

struct unbalanced_binary_tree_tag
{
};

struct balanced_binary_tree_tag
{
};

namespace detail
{

template <typename, typename, typename>
class mixin_stub
{
};

template <template <typename, typename, typename> class Base,
          template <typename,
                    typename,
                    typename,
                    template <typename, typename, typename> class>
          class... List>
class fold_mixins;

template <template <typename, typename, typename> class Base,
          template <typename,
                    typename,
                    typename,
                    template <typename, typename, typename> class> class Head,
          template <typename,
                    typename,
                    typename,
                    template <typename, typename, typename> class>
          class... Tail>
class fold_mixins<Base, Head, Tail...>
{
private:
  template <typename T, typename M, typename Allocator>
  using base_type = Head<T, M, Allocator, Base>;

public:
  template <typename T, typename M, typename Allocator>
  using type =
    typename fold_mixins<base_type, Tail...>::template type<T, M, Allocator>;
};

template <template <typename, typename, typename> class Base,
          template <typename,
                    typename,
                    typename,
                    template <typename, typename, typename> class> class Head>
class fold_mixins<Base, Head>
{
public:
  template <typename T, typename M, typename Allocator>
  using type = Head<T, M, Allocator, Base>;
};

template <template <typename, typename, typename> class Base>
class fold_mixins<Base>
{
public:
  template <typename T, typename M, typename Allocator>
  using type = Base<T, M, Allocator>;
};

}

template <typename... Mixins>
class fold_mixins
{
public:
  template <typename T, typename M, typename Allocator>
  using type = typename detail::fold_mixins<
    detail::mixin_stub,
    Mixins::template type...>::template type<T, M, Allocator>;
};

} // binary_tree

template <typename TreeCategory>
using is_balanced_binary_tree =
  std::is_convertible<TreeCategory, binary_tree::balanced_binary_tree_tag>;

template <typename TreeCategory>
using is_unbalanced_binary_tree =
  std::is_convertible<TreeCategory, binary_tree::unbalanced_binary_tree_tag>;

} // dst

