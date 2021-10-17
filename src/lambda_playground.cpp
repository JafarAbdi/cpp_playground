#include <fmt/core.h>

#include <algorithm>
#include <iterator>
#include <variant>

#include "fmt/ranges.h"

// Template lambda variable
template <typename F>
constexpr auto when = [](auto&& obj, auto&& func) {
  if (auto* p = dynamic_cast<F*>(&obj)) {
    func(*p);
  }
};

template <typename F>
constexpr auto jafar = []() { return F{}; };

template <typename T>
struct invalid_argument : std::false_type {};

auto lambda = [](auto op) mutable {
  using Op = decltype(op);
  if constexpr (std::is_same_v<Op, int>) {
    return op + 10;
  } else if constexpr (std::is_same_v<Op, double>) {
    return op + 2;
  } else if constexpr (std::is_same_v<Op, std::string>) {
    return op + "_2";
  } else
    static_assert(invalid_argument<Op>::value, "Invalid argument");
};
// Usage: std::visit(overload{[](const T1& t) {}, ..., [](const Tn& t) {} }, std::variant<T1, ..., Tn>(...));
template <class... Ts>
struct overload : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

int main(int argc, char** argv) {
  std::variant<double, int> variant;
  variant = 10;
  std::visit(
      overload{[](const double d) { fmt::print("Double: {}\n", d); }, [](const int i) { fmt::print("Int: {}\n", i); }},
      variant);
  //  std::vector<std::string> v1{"asd"};
  //  std::vector<std::string> v2;
  //  std::vector<std::string> v{"jafar", "abdi", "hola", "mola"};
  //  const bool is_v1 = true;
  //  auto i = jafar<int>();
  //  std::for_each(v.cbegin(), v.cend(), [&out = is_v1 ? v1 : v2](const std::string& str) { out.push_back(str); });
  //  fmt::print("v1: {} - v2: {} - v: {}\n", fmt::join(v1, ", "), fmt::join(v2, ", "), fmt::join(v, ", "));
}
