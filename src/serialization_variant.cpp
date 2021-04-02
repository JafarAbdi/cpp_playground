#include <fmt/core.h>
#include <fmt/ranges.h>

#include <any>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/extended_type_info_typeid.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
//#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <istream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string_view>
#include <typeindex>
#include <unordered_map>
#include <variant>

#include "std_variant.h"

using parameter_type = std::variant<int, double, std::string, std::vector<int>,
                                    std::vector<std::string>>;

class parameter_evaluator_t {
 public:
  template <typename T>
  inline constexpr auto operator()(const std::vector<T>& parameter) const {
    fmt::print("vector: {}", fmt::join(parameter, ", "));
  }
  template <typename T>
  inline constexpr auto operator()(const T& parameter) const {
    fmt::print("type: {}", parameter);
  }
} parameter_evaluator;

template <typename T>
auto l = [](std::any const& a) -> T { return std::any_cast<T>(a); };
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

int main(int argc, char** argv) {
  // static_assert(std::is_same_v<decltype(l<int>), decltype(l<double>)>);
  //  std::any jafar{10.0};
  //  auto overloader = make_overloader(l<int>);
  //  double i = overloader(jafar);
  parameter_type data = 10;// std::vector<int>{1, 2, 3, 4};

  std::ostringstream oss;
  {
    boost::archive::text_oarchive oa(oss);
    oa << data;
  }

  fmt::print("{}\n", oss.str());
  parameter_type new_data;
  std::istringstream iss(oss.str());
  {
    boost::archive::text_iarchive ia(iss);
    ia >> new_data;
  }
  std::visit(parameter_evaluator, new_data);
}
