#include <fmt/core.h>
#include <fmt/ranges.h>

#include <iostream>
#include <range/v3/all.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/range/conversion.hpp>

int main(int argc, char* argv[]) {
  ranges::getlines(std::cin);
  auto ints = ranges::views::iota(1);
  auto ints_squared = ints | ranges::views::transform([](const auto v) { return v * v; });
  const auto sum = ranges::accumulate(ints_squared | ranges::views::take(3), 0);
  return 0;
}
