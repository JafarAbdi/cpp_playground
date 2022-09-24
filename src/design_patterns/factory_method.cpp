#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <vector>

// the factory method is a method, and an abstract factory is an object.
// Factory Method pattern uses inheritance and relies on a subclass to handle the desired object instantiation.
// Abstract Factory pattern a class delegates the responsibility of object instantiation to another object via composition

class Object {
 public:
  explicit Object(const char c) : c(c) {}
  virtual void print(std::ostream& out) const = 0;

 protected:
  char c;
};

class Square : public Object {
 public:
  explicit Square(const char c) : Object(c) {}
  void print(std::ostream& out) const override {
    out << fmt::format(
        "\n{c} {c} {c} {c}"
        "\n{c}     {c}"
        "\n{c}     {c}"
        "\n{c} {c} {c} {c}",
        fmt::arg("c", c));
  }
};

class Rectangle : public Object {
 public:
  explicit Rectangle(const char c) : Object(c) {}
  void print(std::ostream& out) const override {
    out << fmt::format(
        "\n{c} {c} {c} {c} {c} {c} {c} {c}"
        "\n{c}             {c}"
        "\n{c}             {c}"
        "\n{c} {c} {c} {c} {c} {c} {c} {c}",
        fmt::arg("c", c));
  }
};

class Triangle : public Object {
 public:
  explicit Triangle(const char c) : Object(c) {}
  void print(std::ostream& out) const override {
    out << fmt::format(
        "\n      {c}"
        "\n    {c}   {c}"
        "\n  {c}       {c}"
        "\n{c} {c} {c} {c} {c} {c} {c}",
        fmt::arg("c", c));
  }
};

/// Using Abstract Factory
template <char c = '*'>
class ObjectFactory {
 public:
  virtual std::unique_ptr<Object> MakeSquare() const { return std::make_unique<Square>(c); }
  virtual std::unique_ptr<Object> MakeRectangle() const { return std::make_unique<Rectangle>(c); }
  virtual std::unique_ptr<Object> MakeTriangle() const { return std::make_unique<Triangle>(c); }
};

using DashObjectFactory = ObjectFactory<'-'>;
using XObjectFactory = ObjectFactory<'x'>;

using Objects = std::vector<std::unique_ptr<Object>>;

// class ObjectPainter {
//  public:
//   template <char c>
//   Objects createObjects(ObjectFactory<c>& factory) const {
//     Objects objects;
//     objects.emplace_back(factory.MakeTriangle());
//     objects.emplace_back(factory.MakeSquare());
//     objects.emplace_back(factory.MakeRectangle());
//     return objects;
//   }
// };
/// Using Factory Methods
class ObjectPainter {
  const char c = '*';

 public:
  Objects createObjects() const {
    Objects objects;
    objects.emplace_back(MakeTriangle());
    objects.emplace_back(MakeSquare());
    objects.emplace_back(MakeRectangle());
    return objects;
  }
  virtual std::unique_ptr<Object> MakeSquare() const { return std::make_unique<Square>(c); }
  virtual std::unique_ptr<Object> MakeTriangle() const { return std::make_unique<Triangle>(c); }
  virtual std::unique_ptr<Object> MakeRectangle() const { return std::make_unique<Rectangle>(c); }
};

class DashObjectPainter : public ObjectPainter {
  const char c = '-';

 public:
  std::unique_ptr<Object> MakeSquare() const override { return std::make_unique<Square>(c); }
  std::unique_ptr<Object> MakeTriangle() const override { return std::make_unique<Triangle>(c); }
  std::unique_ptr<Object> MakeRectangle() const override { return std::make_unique<Rectangle>(c); }
};

class XObjectPainter : public ObjectPainter {
  const char c = 'x';

 public:
  std::unique_ptr<Object> MakeSquare() const override { return std::make_unique<Square>(c); }
  std::unique_ptr<Object> MakeTriangle() const override { return std::make_unique<Triangle>(c); }
  std::unique_ptr<Object> MakeRectangle() const override { return std::make_unique<Rectangle>(c); }
};

int main(int argc, char** argv) {
  DashObjectPainter object_painter;
  auto objects = object_painter.createObjects();
  //  XObjectFactory factory;
  //  auto objects = object_painter.createObjects(factory);
  for (const auto& object : objects) object->print(std::cout);
}
