#include <fmt/core.h>
#include <type_traits>

// https://gist.github.com/krzysztof-jusiak/cb6633d6c11572c33928

template <typename... Ts>
struct [[deprecated]] print_types {};  // print_types<T1, T2, ...>{};

template <class, class>
struct pair {};

/* template <auto, class> */
/* struct pair_ {}; */

template <class... Ts>
struct map : Ts... {};

template <class T>
struct no_decay {
  using type = T;
};


/* template <class, auto TKey, class TValue> */
/* static no_decay<TValue> lookup(pair_<TKey, TValue>*); */

/* template <class TDefault, auto TKey, class T> */
/* using at_key_v = decltype(lookup<TDefault, TKey>((T*)0)); */

/* template <class T, auto TKey, class TDefault = void> */
/* using at_key_v_t = typename at_key_v<TDefault, TKey, T>::type; */
/* enum class MyTypes { */
/*   Int, */
/*   Double, */
/*   String, */
/* }; */

/* using m2 = map<pair_<MyTypes::Double, double>, pair_<MyTypes::Int, const int>>; */

template <class TDefault, class>
static no_decay<TDefault> lookup(...);

template <class, class TKey, class TValue>
static no_decay<TValue> lookup(pair<TKey, TValue>*);

// TODO: Why we need TDefault? to differentiate between template with 2 parameters and the other one with 3 ?
// TValue will be automatically inferred
// T* -> pair<...>* := conversion from derived to base
// We pick the overload if pair<key, value> is a base class of the map
template <class TDefault, class TKey, class T>
using at_key = decltype(lookup<TDefault, TKey>((T*)0));
template <class T, class TKey, class TDefault = void>
using at_key_t = typename at_key<TDefault, TKey, T>::type;

using m = map<pair<int, double>, pair<double, int>>;

void p1(...);
char p1(pair<int, char>*);
int p1(pair<double, std::string>*);


template <typename K>
using p = decltype(p1((pair<K, V>*)0));

int main() {
  print_types<p<int>>{};
  /* static_assert(std::is_same<at_key_v_t<m2, MyTypes::Int>, const int>{}, ""); */
  static_assert(std::is_same<at_key_t<m, int>, double>{}, "");
  static_assert(std::is_same<at_key_t<m, double>, int>{}, "");
}

// time clang++ -std=c++14 map.cpp
// real	0m0.095s
// user	0m0.082s
// sys	0m0.012s
