#include <fmt/core.h>

#include <chrono>
#include <functional>
// https://github.com/QuantlabFinancial/cpp_tip_of_the_week/blob/master/229.md
#include <fixed_string.hpp>
#include <iostream>

template <typename... Ts>
struct [[deprecated]] print_types {};  // print_types<T1, T2, ...>{};

template <const auto& name_fs>
struct named_field {
 private:
  template <class T>
  struct value_wrapper {
    static constexpr auto& name() { return name_fs; }
    T value{};
  };

 public:
  static constexpr auto& name = name_fs;

  template <class T>
  constexpr auto operator=(const T& t) {
    return value_wrapper<T>{.value = t};
  }
};

// template <char const* Str, std::size_t... indices>
// constexpr auto make_named_field_impl(std::index_sequence<indices...>) {
//   return named_field<Str[indices]...>{};
// }
//
// template <const char* Str, std::size_t size>
// constexpr auto make_named_field() {
//   return make_named_field_impl<Str>(std::make_index_sequence<size>());
// }

template <class TChar, TChar... Cs>
constexpr auto operator""_t() {
  return named_field<fixed_string_reference<Cs...>>{};
}

template <typename...>
inline constexpr auto is_unique = true;

template <typename T, typename... Rest>
inline constexpr auto is_unique<T, Rest...> = ((T::name() != Rest::name()) && ...) && is_unique<Rest...>;

template <class... Ts>
struct namedtuple : Ts... {
 private:
  static_assert(is_unique<Ts...>, "Named fields aren't unique");
  struct no_value {};
  friend constexpr auto operator,(no_value v, no_value) { return v; }
  template <class T>
  friend constexpr auto operator,(no_value, T&& t) -> decltype(auto) {
    return std::forward<T>(t);
  }
  template <class T>
  friend constexpr auto operator,(T&& t, no_value) -> decltype(auto) {
    return std::forward<T>(t);
  }

  template <class... Us, const auto& name>
  [[nodiscard]] constexpr auto lookup(const named_field<name>&) const -> decltype(auto) {
    return (
        [&]() -> decltype(auto) {
          if constexpr (Us::name() == named_field<name>::name) {
            return (static_cast<Us&>(const_cast<namedtuple&>(*this)).value);
          } else {
            return no_value{};
          }
        }(),
        ...,
        no_value{});
  }

 public:
  template <const auto& name>
  constexpr auto operator[](const named_field<name>& f) const -> decltype(auto) {
    return lookup<std::add_const_t<Ts>...>(f);
  }

  template <const auto& name>
  constexpr auto operator[](const named_field<name>& f) -> decltype(auto) {
    return lookup<Ts...>(f);
  }

  constexpr void print() const {
    std::cout << "---START---\n";
    ((std::cout << Ts::name() << ": " << lookup<Ts...>(named_field<Ts::name()>()) << "\n"), ...);
    std::cout << "---END---\n";
  }
};
template <class... Ts>
namedtuple(Ts&&...) -> namedtuple<Ts...>;

int main() {
  constexpr auto t1 = "price"_t = 20;
  constexpr auto t2 = "size"_t = 20;
  constexpr auto t3 = "name"_t = 20;
  constexpr auto t4 = "name"_t = 20;
  static_assert(is_unique<decltype(t1), decltype(t2), decltype(t3)>);
  static_assert(!is_unique<decltype(t1), decltype(t2), decltype(t3), decltype(t4)>);
  const auto nt = namedtuple{"price"_t = 42, "size"_t = 100};
  static_assert(std::is_same_v<decltype(nt["price"_t]), const int&>);
  nt.print();
  fmt::print("42 == nt[\"price\"_t]: {}\n", 42 == nt["price"_t]);
  fmt::print("100 == nt[\"size\"_t]: {}\n", 100 == nt["size"_t]);

  auto nt2 = namedtuple{"price"_t = int{}, "jafar"_t = int{}, "size"_t = std::size_t{}, "asd"_t = std::string{}};
  static_assert(std::is_same_v<decltype(nt2["price"_t]), int&>);
  nt2["price"_t] = 12;
  nt2["size"_t] = 34u;
  nt2["asd"_t] = "jafar";
  nt2["jafar"_t] = 100;
  fmt::print("nt[\"price\"_t]: {}\n", nt2["price"_t]);
  fmt::print("nt[\"size\"_t]: {}\n", nt2["size"_t]);
  fmt::print("nt2[\"asd\"_t]: {}\n", nt2["asd"_t]);
  fmt::print("nt2[\"jafar\"_t]: {}\n", nt2["jafar"_t]);
  nt2.print();
}
