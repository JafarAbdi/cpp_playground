#include <fmt/core.h>
#include <fmt/ranges.h>

#include <cstddef>
#include <memory>
#include <string_view>
#include <utility>

struct Product1 {};
struct Product1A : Product1 {};
struct Product1B : Product1 {};

struct Product2 {};
struct Product2A : Product2 {};
struct Product2B : Product2 {};

struct AbstractFactory {
  virtual Product1 createProduct1() const = 0;
  virtual Product2 createProduct2() const = 0;
};

struct FactoryA : AbstractFactory {
  Product1 createProduct1() const override { return Product1A{}; }
  Product2 createProduct2() const override { return Product2A{}; }
};

struct FactoryB : AbstractFactory {
  Product1 createProduct1() const override { return Product1B{}; }
  Product2 createProduct2() const override { return Product2B{}; }
};

int main(int argc, char** argv) {
  auto abstract_factory = std::make_unique<FactoryA>();
  abstract_factory->createProduct1();
  fmt::print("Product1A created");
}
