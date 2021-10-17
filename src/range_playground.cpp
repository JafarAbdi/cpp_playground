#include <iostream>
#include <range/v3/view/filter.hpp>
#include <range/v3/view/transform.hpp>
#include <string>
#include <tuple>
#include <vector>
using std::cout;
template <typename... Ts>
struct [[deprecated]] print_types {};  // print_types<T1, T2, ...>{};

template <class... T>
struct typelist {};

template <typename Tuple>
struct as_typelist;

template <template <class...> class Tuple, class... Types>
struct as_typelist<Tuple<Types...>> {
  using type = typelist<Types...>;
};

template <typename Tuple>
using as_typelist_t = typename as_typelist<Tuple>::type;

template <typename... Lists>
struct typelist_cat
{
};

template <typename... Lists>
using typelist_cat_t = typename typelist_cat<Lists...>::type;

template <>
struct typelist_cat<>
{
  using type = typelist<>;
};

template <typename... L1>
struct typelist_cat<typelist<L1...>>
{
using type = typelist<L1...>;
};

template <typename... L1, typename... L2>
struct typelist_cat<typelist<L1...>, typelist<L2...>>
{
using type = typelist<L1..., L2...>;
};

template <template <class...> class F>
struct meta_quote {
  template <typename... Ts>
  using apply = F<Ts...>;
};

namespace detail {
template <typename Ret, typename... Is, typename... Ks, typename Tuples>
Ret tuple_cat_(typelist<Is...>, typelist<Ks...>, Tuples tpls) {
  return Ret{std::get<Ks::value>(std::get<Is::value>(tpls))...};
}
}  // namespace detail

template <typename... Tuples,
          typename Res = typelist_apply_t<meta_quote<std::tuple>, typelist_cat_t<typelist<as_typelist_t<Tuples>...>>>>
Res tuple_cat(Tuples&&... tpls) {
  static constexpr std::size_t N = sizeof...(Tuples);
  // E.g. [0,0,0,2,2,2,3,3]
  using inner = typelist_cat_t<
      typelist_transform_t<typelist<as_typelist_t<Tuples>...>,
                           typelist_transform_t<as_typelist_t<std::make_index_sequence<N>>, meta_quote<meta_always>>,
                           meta_quote<typelist_transform_t>>>;
  // E.g. [0,1,2,0,1,2,0,1]
  using outer = typelist_cat_t<typelist_transform_t<typelist<as_typelist_t<Tuples>...>,
                                                    meta_compose<meta_quote<as_typelist_t>,
                                                                 meta_quote_i<std::size_t, std::make_index_sequence>,
                                                                 meta_quote<typelist_size_t>>>>;
  return detail::tuple_cat_<Res>(inner{}, outer{}, std::forward_as_tuple(std::forward<Tuples>(tpls)...));
}

int main() {

  std::vector<int> const vi{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  using namespace ranges;
  auto rng =
      vi | views::filter([](int i) { return i % 2 == 0; }) | views::transform([](int i) { return std::to_string(i); });
  // prints: [2,4,6,8,10]
  cout << rng << '\n';
}
