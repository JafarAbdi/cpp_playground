#include <cstdio>
#include <string>
#include <type_traits>
// friend injection
class foo {
 public:
  int get_data() const { return data; }

 private:
  int data;
};

template <int foo::*Ptr>
struct foo_access {
  friend int& get_data(foo& f) { return f.*Ptr; }
};

// This will generate the symbol for get_data otherwise we will have a linking error
template struct foo_access<&foo::data>;
// This's just a forward declaration otherwise the compiler will not find it
int& get_data(foo& f);

struct a {
  void func() {}
};

struct b {
  void not_func() {}
  int func;
};

struct f1 {
  void func() {}
};

struct f2 {
  void func() const {}
};

template <typename T, typename = std::void_t<>>
struct has_func : std::false_type {};

template <typename T>
struct has_func<T, std::void_t<decltype(std::declval<T>().func())>> : std::true_type {};

template <typename T, typename = std::void_t<>>
struct has_const_func : std::false_type {};

template <typename T>
struct has_const_func<T, std::void_t<decltype(std::declval<std::add_const_t<T>>().func())>> : std::true_type {};

template <typename T>
struct has_non_const_func {
  static constexpr bool value = has_func<T>::value && !has_const_func<T>::value;
};

template <typename... Ts>
struct [[deprecated]] print_types {};  // print_types<T1, T2, ...>{};

template <typename T>
struct type_t {
  using type = T;
};

template <typename T>
constexpr auto type = type_t<T>{};

template <typename T>
T value_t(type_t<T>);

int main() {
  std::string str;
  print_types< decltype(value_t(type<std::string>))>{};
  static_assert(has_func<a>::value);
  static_assert(!has_func<b>::value);
  static_assert(!has_const_func<f1>::value);
  static_assert(has_const_func<f2>::value);
  static_assert(has_non_const_func<f1>::value);
  static_assert(!has_non_const_func<f2>::value);
}
