#include <fmt/core.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

#include <iterator>
#include <optional>
#include <range/v3/all.hpp>
// #include <range/v3/range/concepts.hpp>
// #include <range/v3/range/traits.hpp>
// #include <range/v3/view/iota.hpp>
#include <ranges>
#include <string>
#include <tl/expected.hpp>
#include <type_traits>
#include <vector>

namespace detail {
template <typename R>
using expected_value_t = std::remove_cvref_t<std::ranges::range_value_t<R>>;

template <typename R>
using expected_error_t = typename std::remove_cvref_t<std::ranges::range_value_t<R>>::error_type;
}  // namespace detail

/// Range adaptor that collects a range of expected values into a single expected vector
/// The adaptor will short-circuit on the first error encountered, returning that error wrapped in an unexpected.
struct collect_expected_fn {
  /// Collects a range of expected values into a single expected vector
  ///
  /// @tparam R Type of the range (deduced automatically)
  /// @param range Input range of expected values
  /// @return If successful, returns expected containing vector of all values.
  ///         If error occurs, returns unexpected with the first error encountered.
  ///
  /// Example:
  /// ```
  ///     std::vector<tl::expected<int, std::string>> values = {
  ///         tl::expected<int, std::string>{1},
  ///         tl::expected<int, std::string>{2}
  ///     };
  ///     auto result = values | collect_expected;
  ///     if (result) {
  ///         // result.value() is vector{1, 2}
  ///     }
  /// ```
  // template <typename R>
  template <std::ranges::sized_range R>
  auto operator()(R&& range) const {
    using T = detail::expected_value_t<R>;
    using E = detail::expected_error_t<R>;
    using Result = tl::expected<std::vector<T>, E>;

    std::vector<T> result;

    result.reserve(std::size(range));

    // Collect values until we hit an error
    for (auto&& exp : range) {
      if (!exp) {
        return Result(tl::make_unexpected(exp.error()));
      }
      result.push_back(std::move(exp).value());
    }

    return Result(std::move(result));
  }

  template <std::ranges::sized_range R>
  friend auto operator|(R&& range, const collect_expected_fn&) {
    return collect_expected_fn{}(std::forward<R>(range));
  }
};

inline constexpr collect_expected_fn collect_expected{};

template <typename T>
using Expected = tl::expected<T, std::string>;

template <typename T>
struct fmt::formatter<Expected<T>> {
  template <typename ParseContext>
  constexpr auto parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const Expected<T>& result, FormatContext& ctx) const {
    if (result.has_value()) {
      return format_to(ctx.out(), "[Expected<T>: value={}]", result.value());
    }
    return format_to(ctx.out(), "[Expected<T>: {}]", result.error());
  }
};

Expected<std::string> to_stringy_with_error(int i) {
  if (i == 5) {
    return tl::make_unexpected(fmt::format("Error: {}", i));
  }
  return std::to_string(i);
}

int main(int argc, char* argv[]) {
  const auto v = std::vector{1, 2, 3, 4, 5, 6};
  spdlog::info("{}", v | ranges::views::transform(to_stringy_with_error) | collect_expected);
  spdlog::info("{}", ranges::views::iota(0, 10) | ranges::views::transform(to_stringy_with_error));
  spdlog::info("{}", ranges::views::iota(0, 10) | ranges::views::transform(to_stringy_with_error) | collect_expected);
  spdlog::info("{}", ranges::views::iota(0, 10) | ranges::views::transform([](const int i) {
                       return Expected<std::string>(std::to_string(i));
                     }) | collect_expected);
  spdlog::info("{}", ranges::views::iota(0, 10) | ranges::views::transform([](const int i) {
                       return Expected<std::string>(std::to_string(i));
                     }) | collect_expected);

  std::vector<tl::expected<int, std::string>> vec = {1, 2, 3};
  std::vector<tl::expected<int, std::string>> error1 = {1, tl::make_unexpected("Error"), 2};
  std::vector<tl::expected<int, std::string>> error2 = {
      1, tl::make_unexpected("Error1"), tl::make_unexpected("Error2")};
  (error2 | collect_expected).or_else([](const std::string& error) { spdlog::error("{}", error); });

  spdlog::info("{}", vec | collect_expected);
  spdlog::info("{}", error1 | collect_expected);
  spdlog::info("{}", error2 | collect_expected);
  return 0;
}
