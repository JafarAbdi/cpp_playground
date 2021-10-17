#pragma once

#include <iostream>

// Copied from CTRE
template <size_t N>
struct fixed_string {
  char content[N] = {};
  size_t real_size{0};
  constexpr fixed_string(const char (&input)[N + 1]) noexcept {
    for (size_t i{0}; i < N; ++i) {
      content[i] = input[i];
      if ((i == (N - 1)) && (input[i] == 0)) break;
      real_size++;
    }
  }
  constexpr fixed_string(const fixed_string& other) noexcept {
    for (size_t i{0}; i < N; ++i) {
      content[i] = other.content[i];
    }
    real_size = other.real_size;
  }
  constexpr size_t size() const noexcept { return real_size; }
  constexpr const char* begin() const noexcept { return content; }
  constexpr const char* end() const noexcept { return content + size(); }
  constexpr char operator[](size_t i) const noexcept { return content[i]; }
  template <size_t M>
  constexpr bool is_same_as(const fixed_string<M>& rhs) const noexcept {
    if (real_size != rhs.size()) return false;
    for (size_t i{0}; i != real_size; ++i) {
      if (content[i] != rhs[i]) return false;
    }
    return true;
  }
  constexpr operator std::string_view() const noexcept { return std::string_view{content, size()}; }
};

template <std::size_t N>
inline __attribute__((always_inline)) std::ostream& operator<<(std::ostream& str, const fixed_string<N>& rhs) {
  return str << std::string_view(rhs);
}
template <std::size_t N1, std::size_t N2>
inline constexpr bool operator==(const fixed_string<N1>& rhs, const fixed_string<N2>& lhs) noexcept {
  return rhs.is_same_as(lhs);
}
template <std::size_t N1, std::size_t N2>
inline constexpr bool operator!=(const fixed_string<N1>& rhs, const fixed_string<N2>& lhs) noexcept {
  return !rhs.is_same_as(lhs);
}

template <size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N - 1>;

template <size_t N>
fixed_string(fixed_string<N>) -> fixed_string<N>;

// literals.hpp
template <char... input>
static inline constexpr auto fixed_string_reference = fixed_string<sizeof...(input)>({input...});

template <typename CharT, CharT... charpack>
__attribute__((flatten)) constexpr inline __attribute__((always_inline)) auto& operator""_fs() noexcept {
  return fixed_string_reference<charpack...>;
}
