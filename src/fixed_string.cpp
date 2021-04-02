#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstddef>
#include <string_view>
#include <utility>

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
  constexpr fixed_string(const fixed_string &other) noexcept {
    for (size_t i{0}; i < N; ++i) {
      content[i] = other.content[i];
    }
    real_size = other.real_size;
  }
  constexpr size_t size() const noexcept { return real_size; }
  constexpr const char *begin() const noexcept { return content; }
  constexpr const char *end() const noexcept { return content + size(); }
  constexpr char operator[](size_t i) const noexcept { return content[i]; }
  template <size_t M>
  constexpr bool is_same_as(const fixed_string<M> &rhs) const noexcept {
    if (real_size != rhs.size()) return false;
    for (size_t i{0}; i != real_size; ++i) {
      if (content[i] != rhs[i]) return false;
    }
    return true;
  }
  constexpr operator std::string_view() const noexcept { return std::string_view{content, size()}; }
};
template <std::size_t N1, std::size_t N2>
inline constexpr bool operator==(const fixed_string<N1> &rhs, const fixed_string<N2> &lhs) noexcept {
  return rhs.is_same_as(lhs);
}

template <size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N - 1>;

template <size_t N>
fixed_string(fixed_string<N>) -> fixed_string<N>;

template <char... input>
static inline constexpr auto fixed_string_reference = fixed_string<sizeof...(input)>({input...});

template <typename CharT, CharT... charpack>
__attribute__((flatten)) constexpr inline __attribute__((always_inline)) auto &operator""_fs() noexcept {
  return fixed_string_reference<charpack...>;
}

struct Message {
  std::string description;
  std::vector<std::string> roles;
  static constexpr char description_string[] = "message";
  static constexpr char roles_string[] = "roles";
  template <const auto &member_name>
  struct invalid_member_name : std::false_type {};

  template <const auto &member_name>
  constexpr auto &get_member() {
    if constexpr (std::string_view(member_name) == description_string)
      return description;
    else if constexpr (std::string_view(member_name) == roles_string)
      return roles;
    else
      static_assert(invalid_member_name<member_name>::value, "Invalid member variable name");
  }
};

int main(int argc, char **argv) {
  static_assert(""_fs == ""_fs, "Not-equal!!");
  Message msg;
  msg.get_member<"message"_fs>();
  constexpr auto &asd1 = "message"_fs;
  constexpr auto &asd2 = "message"_fs;
  static_assert(&asd1 == &asd2, "Not-equal!!");
  msg.get_member<asd1>() = "Description";
  msg.get_member<Message::roles_string>() = {"Role1", "Role2", "Role3"};
  static constexpr char name[] = "Hola";

  fmt::print("description string: {} - roles string: {}\n", Message::description_string, Message::roles_string);
  fmt::print("message description: {} - message roles: {}\n", msg.description, fmt::join(msg.roles, ", "));
}
