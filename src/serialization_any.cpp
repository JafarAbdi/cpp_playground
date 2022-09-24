#include <fmt/core.h>
#include <fmt/ranges.h>

#include <any>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/extended_type_info_typeid.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/vector.hpp>
#include <istream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string_view>
#include <typeindex>
#include <unordered_map>

template <typename T>
auto streamer = [](std::any const& a) { return std::any_cast<T>(a); };

template <typename T>
auto oserializer_t = [](std::any const& a, boost::archive::text_oarchive& os) {
  os& std::any_cast<T>(a);
};

template <typename T>
auto iserializer_t = [](std::any& a, boost::archive::text_iarchive& is) {
  a = std::make_any<T>();
  is& std::any_cast<T&>(a);
};

#define TYPES         \
  X(int)              \
  X(double)           \
  X(std::string)      \
  X(std::vector<int>) \
  X(std::vector<std::string>)

#define X(Type) {typeid(Type).name(), oserializer_t<Type>},
inline static const std::unordered_map<
    std::string_view, void (*)(std::any const&, boost::archive::text_oarchive&)>
    serializer{TYPES};
#undef X
#define X(Type) {typeid(Type).name(), iserializer_t<Type>},
inline static const std::unordered_map<
    std::string_view, void (*)(std::any&, boost::archive::text_iarchive&)>
    deserializer{TYPES};
#undef X
#undef TYPES

namespace boost::serialization {
template <class Archive>
void serialize(Archive& ar, std::any& value, const unsigned int version) {
  std::string type_name = value.type().name();
  ar& type_name;
  if constexpr (std::is_same_v<Archive, boost::archive::text_iarchive>)
    deserializer.at(type_name)(value, ar);
  else if (std::is_same_v<Archive, boost::archive::text_oarchive>)
    serializer.at(type_name)(value, ar);
}
}  // namespace boost::serialization

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
auto make_overloader(T&&... t)
{
  return Overloader<std::decay_t<T>...>(std::forward<T>(t)...);
}

int main(int argc, char** argv) {
  // static_assert(std::is_same_v<decltype(l<int>), decltype(l<double>)>);
//  std::any jafar{10.0};
//  auto overloader = make_overloader(l<int>);
//  double i = overloader(jafar);
  std::unordered_map<std::string, std::any> data;
  data["jafar"] = std::string("abdi");
  data["hola"] = 10;
  data["siko-miko"] = 450.2;
  data["vector<int>"] = std::vector<int>{0, 1, 2, 3, 4, 5};
  data["vector<std::string>"] =
      std::vector<std::string>{"10", "12", "14", "16", "18", "20"};

  std::ostringstream oss;
  {
    boost::archive::text_oarchive oa(oss);
    oa << data;
  }

  fmt::print("{}\n", oss.str());
  std::unordered_map<std::string, std::any> new_data;
  std::istringstream iss(oss.str());
  {
    boost::archive::text_iarchive ia(iss);
    ia >> new_data;
  }
  /* for (const auto& n : new_data) { */
  /*   if (n.second.type() == typeid(int)) */
  /*     fmt::print("{}: {}\n", n.first, std::any_cast<int>(n.second)); */
  /*   if (n.second.type() == typeid(std::string)) */
  /*     fmt::print("{}: {}\n", n.first, std::any_cast<std::string>(n.second)); */
  /*   if (n.second.type() == typeid(double)) */
  /*     fmt::print("{}: {}\n", n.first, std::any_cast<double>(n.second)); */
  /*   if (n.second.type() == typeid(std::vector<int>)) */
  /*     fmt::print("{}: {}\n", n.first, */
  /*                fmt::join(std::any_cast<std::vector<int>>(n.second), ", ")); */
  /*   if (n.second.type() == typeid(std::vector<std::string>)) */
  /*     fmt::print( */
  /*         "{}: {}\n", n.first, */
  /*         fmt::join(std::any_cast<std::vector<std::string>>(n.second), ", ")); */
  /* } */
}
