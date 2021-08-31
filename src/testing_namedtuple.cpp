#include <fmt/core.h>

#include <functional>
#include <chrono>
// https://github.com/QuantlabFinancial/cpp_tip_of_the_week/blob/master/229.md

template <char... Cs>
struct named_field {
 private:
  static constexpr char name_storage[] = {Cs...};

  template <class T>
  struct value_wrapper {
    static constexpr auto name() { return named_field::name; }
    T value{};
  };

 public:
  static constexpr auto name = std::string_view{std::data(name_storage), std::size(name_storage)};

  template <class T>
  constexpr auto operator=(const T& t) {
    return value_wrapper<T>{.value = t};
  }
};

template <char const* Str, std::size_t... indices>
constexpr auto make_named_field_impl(std::index_sequence<indices...>) {
  return named_field<Str[indices]...>{};
}

template <const char* Str, std::size_t size>
constexpr auto make_named_field() {
  return make_named_field_impl<Str>(std::make_index_sequence<size>());
}

template <class TChar, TChar... Cs>
constexpr auto operator""_t() {
  return named_field<Cs...>{};
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

  template <class... Us, char... Cs>
  [[nodiscard]] constexpr auto lookup(const named_field<Cs...>&) const -> decltype(auto) {
    return (
        [&](const auto& u) -> decltype(auto) {
          using U = std::remove_cv_t<std::remove_reference_t<decltype(u)>>;
          if constexpr (U::name() == named_field<Cs...>::name) {
            return (static_cast<U&>(const_cast<namedtuple&>(*this)).value);
          } else {
            return no_value{};
          }
        }(Us{}),
        ...,
        no_value{});
  }

 public:
  template <char... Cs>
  constexpr auto operator[](const named_field<Cs...>& f) const -> decltype(auto) {
    return lookup<std::add_const_t<Ts>...>(f);
  }

  template <char... Cs>
  constexpr auto operator[](const named_field<Cs...>& f) -> decltype(auto) {
    return lookup<Ts...>(f);
  }

  constexpr void print() const {
    std::cout << "---START---\n";
    ((std::cout << Ts::name() << ": " << lookup<Ts...>(make_named_field<Ts::name().data(), Ts::name().size()>())
                << "\n"),
     ...);
    std::cout << "---END---\n";
  }
};
template <class... Ts>
namedtuple(Ts&&...) -> namedtuple<Ts...>;

// template <typename... Ts>
// void func(const namedtuple<Ts...>& nt) {
//   ((std::cout << Ts::name() << ": " << nt[make_named_field<Ts::name().data(), Ts::name().size()>()] << " - "), ...)
//       << "\n";
// }

int main() {
  constexpr auto t1 = "price"_t = 20;
  constexpr auto t2 = "size"_t = 20;
  constexpr auto t3 = "name"_t = 20;
  constexpr auto t4 = "name"_t = 20;
  static_assert(is_unique<decltype(t1), decltype(t2), decltype(t3)>);
  static_assert(!is_unique<decltype(t1), decltype(t2), decltype(t3), decltype(t4)>);
  const auto nt = namedtuple{"price"_t = 42, "size"_t = 100};
  fmt::print("HOLA: {}\n", creator<5>("jasdasd"));
  nt.print();
  fmt::print("42 == nt[\"price\"_t]: {}\n", 42 == nt["price"_t]);
  fmt::print("100 == nt[\"size\"_t]: {}\n", 100 == nt["size"_t]);

  auto nt2 = namedtuple{"price"_t = int{}, "jafar"_t = int{}, "size"_t = std::size_t{}, "asd"_t = std::string{}};
  nt2["price"_t] = 12;
  nt2["size"_t] = 34u;
  nt2["asd"_t] = "jafar";
  nt2["jafar"_t] = 100;
  std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> compute_start_time =
      std::chrono::steady_clock::now();
  nt2["price"_t] = 12;
  nt2["size"_t] = 34u;
  nt2["asd"_t] = "jafar";
  nt2["jafar"_t] = 100;
  std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> compute_stop_time =
      std::chrono::steady_clock::now();
  fmt::print("time: {}\n", (compute_stop_time - compute_start_time).count());
  fmt::print("nt[\"price\"_t]: {}\n", nt2["price"_t]);
  fmt::print("nt[\"size\"_t]: {}\n", nt2["size"_t]);
  fmt::print("nt2[\"asd\"_t]: {}\n", nt2["asd"_t]);
  fmt::print("nt2[\"jafar\"_t]: {}\n", nt2["jafar"_t]);
  nt2.print();
}
