#include <fmt/core.h>
#include <fmt/format.h>

#include <cstdio>
#include <memory>

struct my_sub_type {
  int value;

  my_sub_type() : value{100} { puts("sub constructed"); }
  my_sub_type(const my_sub_type&) = delete;
  my_sub_type(my_sub_type&&) = delete;
  ~my_sub_type() { puts("sub destroyed"); }
};

struct my_type {
  int value;
  my_sub_type sub_value;

  my_type() : value{42} { puts("constructed"); }
  my_type(const my_type&) = delete;
  my_type(my_type&&) = delete;
  ~my_type() { puts("destroyed"); }
};

struct verbose_type {
  verbose_type() { puts("verbose_type: ctor"); }
  verbose_type(const verbose_type& rhs) { puts("verbose_type: copy-ctor"); }
  verbose_type(verbose_type&& rhs) noexcept { puts("verbose_type: move-ctor"); }
  ~verbose_type() { puts("verbose_type: dtor"); }
  verbose_type& operator=(const verbose_type& rhs) {
    puts("verbose_type: copy-operator");
    return *this;
  }
  verbose_type& operator=(verbose_type&& rhs) noexcept {
    puts("verbose_type: move-operator");
    return *this;
  }
};

void f(std::shared_ptr<verbose_type> t) {
  fmt::print("Address: {} - count: {}\n", static_cast<void*>(&t), t.use_count());
}
void c_f(const std::shared_ptr<verbose_type> t) {
  fmt::print("Address: {} - count: {}\n", static_cast<const void*>(&t), t.use_count());
}
void cr_f(const std::shared_ptr<verbose_type>& t) {
  fmt::print("Address: {} - count: {}\n", static_cast<const void*>(&t), t.use_count());
}
void cr_f_c(const std::shared_ptr<const verbose_type>& t) {
  fmt::print("Address: {} - count: {}\n", static_cast<const void*>(&t), t.use_count());
}
void r_f_c(std::shared_ptr<const verbose_type>& t) {
  fmt::print("Address: {} - count: {}\n", static_cast<void*>(&t), t.use_count());
}
void f_c(std::shared_ptr<const verbose_type> t) {
  fmt::print("Address: {} - count: {}\n", static_cast<void*>(&t), t.use_count());
}

// Converts a compile-time string to basic_string_view.
template <size_t N>
constexpr std::string_view compile_string_to_view(const char (&s)[N]) {
  // Remove trailing NUL character if needed. Won't be present if this is used
  // with a raw character array (i.e. not defined as a string).
  return {s, N - (std::char_traits<char>::to_int_type(s[N - 1]) == 0 ? 1 : 0)};
}

#define FIXED_STRING(s)                                                                 \
  [] {                                                                                  \
    /* Adding explicit is causing failures      */                                      \
    /* Use a macro-like name to avoid shadowing warnings. */                            \
    struct COMPILE_STRING {                                                             \
      constexpr operator std::string_view() const { return compile_string_to_view(s); } \
    };                                                                                  \
    return COMPILE_STRING();                                                            \
  }()

template <typename T>
void print(T fixed_string) {
  static_assert(fixed_string == std::string_view("printable") || fixed_string == std::string_view("jafar"));
  fmt::print("`{}` is printable", fixed_string);
}

int main() {
  constexpr auto asd = FIXED_STRING("jafar");
  constexpr auto asd2 = FIXED_STRING("jafar");
  static_assert(std::string_view(asd) == asd2);
  print(FIXED_STRING("printable"));
  print(asd);
  auto t = std::make_shared<verbose_type>();
  auto t2 = std::make_shared<const verbose_type>();
  fmt::print("t: {}\n", static_cast<void*>(&t));
  fmt::print("t2: {}\n", static_cast<void*>(&t2));
  fmt::print("std::shared_ptr<verbose_type> -- ");
  f(t);
  fmt::print("const std::shared_ptr<verbose_type> -- ");
  c_f(t);
  fmt::print("const std::shared_ptr<verbose_type>& -- ");
  cr_f(t);
  fmt::print("const std::shared_ptr<const verbose_type>& -- ");
  cr_f_c(t);
  fmt::print("std::shared_ptr<const verbose_type>& -- ");
  r_f_c(t2);
  fmt::print("std::shared_ptr<const verbose_type> -- ");
  f_c(t);
  //  std::shared_ptr<my_sub_type> aliased = nullptr;
  //  {
  //    puts("hola3");
  //    auto instance = std::make_shared<my_type>();
  //    puts("hola1");
  //    aliased = std::shared_ptr<my_sub_type>(instance, &instance->sub_value);
  //    fmt::print("{} - {}\n", aliased.use_count(), instance.use_count());
  //  }
  //  puts("hola2");
  //  puts("hola2");
  //
  //  return aliased->value;
}
