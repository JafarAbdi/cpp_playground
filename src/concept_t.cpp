#include <iostream>
#include <memory>
#include <vector>

template <typename... Args>
inline constexpr bool dependent_false = false;

template <typename, typename = std::void_t<>>
struct is_streamable : std::false_type {};

template <typename T>
struct is_streamable<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};

template <typename T, std::enable_if_t<is_streamable<T>::value, void*> = nullptr>
void draw(const T& x, std::ostream& os, std::size_t position) {
  os << std::string(position, ' ') << x << std::endl;
}

template <typename, typename = std::void_t<>>
struct has_draw_function : std::false_type {};

template <typename T>
struct has_draw_function<
    T,
    std::void_t<decltype(draw(std::declval<T>(), std::declval<std::ostream&>(), std::declval<std::size_t>()))>>
    : std::true_type {};

class object_t {
 public:
  template <typename T>
  explicit object_t(T x) : self_(std::make_shared<model<T>>(std::move(x))) {}
  friend void draw(const object_t& x, std::ostream& os, const std::size_t position) {
    x.self_->draw_impl(os, position);
  }

 private:
  struct concept_t {
    virtual ~concept_t() = default;
    virtual void draw_impl(std::ostream& os, std::size_t position) const = 0;
  };
  template <typename T>
  struct model : concept_t {
    explicit model(T x) : data_(std::move(x)) {}
    void draw_impl(std::ostream& os, const std::size_t position) const override {
      if constexpr (has_draw_function<T>::value)
        draw(data_, os, position);
      else {
        static_assert(dependent_false<T>,
                      "Cannot format an argument. To make type T formattable provide a "
                      "formatter<T> specialization: https://fmt.dev/latest/api.html#udt");
      }
    }
    T data_;
  };
  std::shared_ptr<const concept_t> self_;
};

using document_t = std::vector<object_t>;

void draw(const document_t& x, std::ostream& out, const std::size_t position) {
  out << std::string(position, ' ') << "<document>" << std::endl;
  for (const auto& e : x) draw(e, out, position + 2);
  out << std::string(position, ' ') << "</document>" << std::endl;
}

struct new_type {};
void draw(const new_type& x, std::ostream& out, const std::size_t position) {
  out << std::string(position, ' ') << "struct new_type {};" << std::endl;
}


int main(int argc, char** argv) {
  static_assert(has_draw_function<int>::value, "You need to create a function draw()");
  static_assert(has_draw_function<std::string>::value, "You need to create a function draw()");
  //  static_assert(is_streamable<new_type>::value, "You need to create a function draw()");
  //  static_assert(has_draw_function<new_type>::value, "You need to create a function draw()");
  //  static_assert(is_drawable<new_type>::value, "You need to create a function draw()");

  document_t document;
  document.emplace_back(std::string("Jafar"));
  document.emplace_back(100);
  document.emplace_back(document);
  document.emplace_back(std::string("Abdi"));
  document.emplace_back(document);
  document.emplace_back(std::string("Hola"));
  document.emplace_back(new_type{});
  draw(document, std::cout, 0);
}
