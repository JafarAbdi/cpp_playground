#include <fmt/core.h>
#include <fmt/ranges.h>

#include <any>
#include <string_view>
#include <unordered_map>

template <typename T>
auto l = [](std::any const& a) -> std::optional<T> {
  if (typeid(T) == a.type()) return std::any_cast<T>(a);
  return std::nullopt;
};

auto l_out = [](std::any const& a, auto& out) -> bool {
  using T = std::remove_cv_t<std::remove_reference_t<decltype(out)>>;
  if (typeid(T) != a.type()) return false;
  out = std::any_cast<T>(a);
  return true;
};

template <typename... Ts>
struct [[deprecated]] print_types {};  // print_types<T1, T2, ...>{};

template <typename... T>
struct Overloader : public T... {
  Overloader(T... t) : T(std::move(t))... {}
  using T::operator()...;
};

template <typename... T>
auto make_overloader(T&&... t) {
  return Overloader<std::decay_t<T>...>(std::forward<T>(t)...);
}

// T should be functor
template <typename T>
struct helper : public T {
  explicit helper(T t) : T(std::move(t)) {}
  using T::operator();
};

template <typename T>
auto make_help(T&& t) {
  return helper<std::decay_t<T>>(std::forward<T>(t));
}

int main(int argc, char** argv) {
  // static_assert(std::is_same_v<decltype(l<int>), decltype(l<double>)>);
  std::any jafar{10};
  int i{};
  std::string as_string;
  l_out(jafar, i);
  fmt::print("{} - {}\n", i, l_out(jafar, as_string));
}
