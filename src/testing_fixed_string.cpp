#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstddef>
#include <fixed_string.hpp>
#include <string_view>
#include <utility>
#include <vector>

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
  fixed_string("afsd");
  static_assert(""_fs == ""_fs, "Not-equal!!");
  static_assert(!("asd"_fs == "far"_fs), "Not-equal!!");
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
