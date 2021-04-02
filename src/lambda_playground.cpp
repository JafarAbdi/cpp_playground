#include <fmt/core.h>

// Template lambda variable
template <typename F>
constexpr auto when = [](auto&& obj, auto&& func) {
  if (auto* p = dynamic_cast<F*>(&obj)) {
    func(*p);
  }
};

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
  }
  else
    static_assert(invalid_argument<Op>::value, "Invalid argument");
};

int main(int argc, char** argv) {
}
