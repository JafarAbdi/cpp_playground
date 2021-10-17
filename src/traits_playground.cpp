#include <fmt/format.h>

#include <algorithm>
#include <boost/type_index.hpp>
#include <cassert>
#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

using namespace std::string_literals;

template <class T>
struct return_type {};

template <class C, class R, class... Args>
struct return_type<R (C::*)(Args...)> {
  using type = R;
};

template <class C, class R, class... Args>
struct return_type<R (C::*)(Args...) const> {
  using type = R;
};

template <class R, class... Args>
struct return_type<R (*)(Args...)> {
  using type = R;
};

template <class R, class... Args>
struct return_type<R(Args...)> {
  using type = R;
};

template <class T>
using return_type_t = typename return_type<T>::type;

template <class T>
struct remove_class {};

template <class C, class R, class... Args>
struct remove_class<R (C::*)(Args...)> {
  typedef R type(Args...);
};

template <class C, class R, class... Args>
struct remove_class<R (C::*)(Args...) const> {
  typedef R type(Args...);
};

template <class R, class... Args>
struct remove_class<R (*)(Args...)> {
  typedef R type(Args...);
};

template <class T>
using remove_class_t = typename remove_class<T>::type;

template <typename... Ts>
struct [[deprecated]] print_types {};

// print_types<
//    std::vector<bool>::reference,
//    std::vector<bool>::value_type,
//    std::vector<bool>::iterator
//    >{};

#define PRINT_TYPE(param) std::cout << boost::typeindex::type_id_with_cvr<decltype(param)>().pretty_name() << std::endl;
#define PRINT_TYPE_T(T) \
  std::cout << "param type is: " << boost::typeindex::type_id_with_cvr<T>().pretty_name() << std::endl;

struct Obj {
  Obj() : id_(id) {
    fmt::print("ctor{}\n", id_);
    id++;
  }
  ~Obj() { fmt::print("dtor{}\n", id_); }
  Obj(const Obj&) : id_(id) {
    fmt::print("copy-ctor{}\n", id_);
    id++;
  }
  Obj(Obj&&) noexcept : id_(id) {
    fmt::print("move-ctor{}\n", id_);
    id++;
  }
  Obj& operator=(const Obj&) {
    fmt::print("copy-assign{}\n", id_);
    return *this;
  }
  Obj& operator=(Obj&&) {
    fmt::print("move-assign{}\n", id_);
    return *this;
  }

  const size_t id_;
  inline static size_t id = 0;
};

struct ObjManager {
  explicit ObjManager(const Obj& obj) : obj_{std::make_unique<Obj>(obj)} {}
  ObjManager(ObjManager&&) = default;
  ObjManager& operator=(ObjManager&&) = default;
  ObjManager(const ObjManager&) = delete;
  void operator=(const ObjManager&) = delete;
  ~ObjManager() {
    if (obj_) fmt::print("Deleting the object {}\n", obj_->id_);
  }

 private:
  std::unique_ptr<const Obj> obj_;
};

std::optional<ObjManager> getObjManager(bool is) {
  if (is) return ObjManager{Obj{}};
  return std::nullopt;
}

std::optional<Obj> getObj(bool is) {
  if (is) return Obj{};
  return std::nullopt;
}

std::unique_ptr<Obj> getObjPtr(bool is) {
  if (is) return std::make_unique<Obj>();
  return nullptr;
}

template <typename Tag, typename U>
struct NamedType {
  using Underlying = U;
  explicit NamedType(Underlying u) : mUnderlying(std::move(u)) {}
  Underlying const& get() const { return mUnderlying; }

 private:
  Underlying mUnderlying;
};

template <typename Tag, typename Signature>
struct CallbackType;

template <typename Tag, typename Res, typename... Args>
struct CallbackType<Tag, Res(Args...)> : NamedType<Tag, std::function<Res(Args...)>> {
  using Signature = Res(Args...);
  using Underlying = typename NamedType<Tag, std::function<Signature>>::Underlying;
  explicit CallbackType(Underlying u) : CallbackType<Tag, Signature>::NamedType(u) {}
  Res operator()(Args&&... args) const { return this->get()(std::forward<Args>(args)...); }
  explicit operator bool() const noexcept { return static_cast<bool>(this->get()); }
};

template <typename V>
class Argument {
 public:
  using Underlying = typename V::Underlying;
  V operator=(Underlying&& v) const { return V(std::forward<Underlying>(v)); }
  Argument() = default;
  Argument(Argument const&) = delete;
  Argument(Argument&&) = delete;
  Argument& operator=(Argument const&) = delete;
  Argument& operator=(Argument&&) = delete;
};

using FailureCallback = CallbackType<struct FailureTag, int(int, int)>;
using SuccessCallback = CallbackType<struct SuccessTag, int(int)>;

static constexpr Argument<FailureCallback> failure_callback;
static constexpr Argument<SuccessCallback> success_callback;

template <typename TypeToPick, typename... Types>
TypeToPick pick(Types&&... args) {
  return std::get<TypeToPick>(std::make_tuple(std::forward<Types>(args)...));
}

struct SmartObject {
  template <typename T, typename A0, typename A1>
  explicit SmartObject(const T& obj, A0&& a0, A1&& a1)
      : obj_{std::make_unique<T>(obj)},
        failure_callback_(pick<FailureCallback>(a1, a0)),
        success_callback_(pick<SuccessCallback>(a1, a0))

  {}
  SmartObject(SmartObject&&) = default;
  SmartObject& operator=(SmartObject&&) = default;
  SmartObject(const SmartObject&) = delete;
  void operator=(const SmartObject&) = delete;
  ~SmartObject() {
    if (obj_) fmt::print("Deleteing the object {}\n", obj_->id_);
    fmt::print("Success: ");
    if (success_callback_) fmt::print("Damn it's working: {}\n", success_callback_(5));
    fmt::print("Failure: ");
    if (failure_callback_) fmt::print("Damn it's working: {}\n", failure_callback_(5, 5));
  }

 private:
  std::unique_ptr<const Obj> obj_;
  FailureCallback failure_callback_;
  SuccessCallback success_callback_;
};

void example_objects() {
  {
    auto obj = getObjManager(true);
    auto obj_2 = std::move(obj);
    auto obj_3 = std::move(obj_2);
    if (obj_2)
      fmt::print("Non-empty\n");
    else
      fmt::print("Empty\n");
  }
  fmt::print("---------------obj-------------------\n");
  {
    auto obj = getObj(true);
    auto obj_2 = std::move(obj);
    if (obj_2)
      fmt::print("Non-empty\n");
    else
      fmt::print("Empty\n");
  }
  fmt::print("---------------unique-ptr-------------------\n");
  {
    auto obj_ptr = getObjPtr(true);
    auto obj_ptr_2 = std::move(obj_ptr);
    if (obj_ptr_2)
      fmt::print("Non-empty\n");
    else
      fmt::print("Empty\n");
  }
}

void smart_object() {
  SmartObject obj(
      Obj{},
      failure_callback =
          [](int i1, int i2) {
            fmt::print("\nFailure it's working\n");
            return i1 * i2;
          },
      success_callback =
          [](int i1) {
            fmt::print("\nSuccess it's working\n");
            return i1 * 10;
          });
}

template <typename FunSignature>
class ScopedTimer;

template <typename Res, typename... Args>
class ScopedTimer<Res(Args...)> {
 public:
  using Clock = std::chrono::steady_clock;
  using ShutdownCallback = std::function<Res(double, Args...)>;

  template <std::enable_if_t<std::is_void_v<Res>, int> = 0>
  ScopedTimer(ShutdownCallback  cb, Args&&... args)
      : shutdown_cb_(std::move(cb)), start_(Clock::now()), args_(0.0, std::forward<Args>(args)...) {}
  ScopedTimer(const ScopedTimer&) = delete;
  ScopedTimer(ScopedTimer&&) = delete;
  ScopedTimer& operator=(const ScopedTimer&) = delete;
  ScopedTimer& operator=(ScopedTimer&&) = delete;
  ~ScopedTimer() {
    std::chrono::duration<double> elapsed = Clock::now() - start_;

    std::get<0>(args_) = elapsed.count();
    std::apply(shutdown_cb_, args_);
  }

 private:
  ShutdownCallback shutdown_cb_;
  const std::chrono::time_point<Clock> start_;
  std::tuple<double, Args...> args_;
};

template <class T>
struct remove_cvref {
  typedef std::remove_cv_t<std::remove_reference_t<T>> type;
};

template <class T>
using remove_cvref_t = typename remove_cvref<T>::type;

// template<typename T, typename = void>
// struct is_callable : std::is_function<T>
//{};

template <typename T, std::enable_if_t<std::is_same_v<decltype(void(&T::operator())), void>>>
struct is_callable : std::true_type {};

template <typename FunSignature>
struct discard_first_arg;

template <typename Res, typename Arg1, typename... Args>
struct discard_first_arg<Res(Arg1, Args...)> {
  using type = Res(Args...);
};

template <typename T>
using discard_first_arg_t = typename discard_first_arg<T>::type;

template <typename LambdaT>
struct lambda {
  using type = discard_first_arg_t<remove_class_t<decltype(&remove_cvref_t<LambdaT>::operator())>>;
};

template <typename T>
using lambda_t = typename lambda<T>::type;

template <typename Cb, typename... Args>
[[nodiscard]] inline ScopedTimer<lambda_t<Cb>> make_scoped_timer(Cb&& cb, Args&&... args) {
  return {std::forward<Cb>(cb), std::forward<Args>(args)...};
}

void func(double d, int i, std::string str) { fmt::print("{}:{}-Hello {}\n", d, i, str); }

struct A {
  void asd() {}
};

int main() {
  auto cb = [](double d, int i, std::string str) { fmt::print("{}:{}-Hello {}\n", d, i, str); };
  // print_types<decltype(&A::asd)>{};
  // print_types<remove_class_t<decltype(&decltype(cb)::operator())>>{};
  // print_types<lambda_t<decltype(cb)>>{};
  //  auto timer = make_scoped_timer(cb, 10, "Jafar"s);

  //  std::is_same_v<decltype(void(&A::operator())), void>;
  //  print_types<decltype(void(&A::operator()))>{};
  auto timer = make_scoped_timer(cb, 10, "Jafar"s);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // smart_object();
  // example_objects();
}
