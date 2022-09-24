#include <fmt/core.h>

#include <utility>
#include <vector>

class Base {
 public:
  Base(std::string name, const std::size_t age) : name(std::move(name)), age(age) { fmt::print("ctor"); }
  /* Base(const Base& rhs) : name(rhs.name), age(rhs.age) { fmt::print("Copy ctor"); } */
  /* Base& operator=(const Base& rhs) { */
  /*   fmt::print("Copy assignment"); */
  /*   name = rhs.name; */
  /*   age = rhs.age; */
  /*   return *this; */
  /* } */
  virtual ~Base() = default;

  virtual void print() const { fmt::print("\nname: {}\nage: {}\n", name, age); }

 private:
  std::string name;
  std::size_t age;
};

class Derived : public Base {
 public:
  Derived(const std::string& name, std::string surname, const std::size_t age)
      : Base(name, age), surname(std::move(surname)) {
    fmt::print("Derived ctor");
  }
  /* Derived(const Derived& rhs): surname(rhs.surname){} */
  /* Derived& operator=(const Derived& rhs) { */
  /*   Base::operator=(rhs); */
  /*   surname = rhs.surname; */
  /*   return *this; */
  /* } */
  void print() const override {
    Base::print();
    fmt::print("surname: {}\n", surname);
  }

 private:
  std::string surname;
};

class MyClass {
};

int main(int /*argc*/, char* /*argv*/[]) {
  std::vector<int>::value_type i = 0;
  fmt::print("asd");
  Derived d1("jafar", "abdi", 24);
  d1.print();
  Derived d2("hola", "mola", 16);
  d2.print();
  d2 = d1;
  d2.print();
  return 0;
}
